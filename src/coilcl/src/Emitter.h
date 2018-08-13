// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Profile.h"
#include "EmitterStream.h"
#include "Sequencer.h"

// Project includes.
#include <CryCC/Program.h>
#include <CryCC/AST.h>

#include <vector>

namespace CoilCl
{
namespace Emit
{

//TODO
struct ModuleException : public std::exception
{
};

struct ModuleInterface
{
	enum ModulePerm
	{
		ReadOnly,    // Request data only (default).
		AppendData,  // Append non-executable data to the tree.
		CopyOnWrite, // Copy-in new or altered node when touched.
		Substitute,	 // Substitue the entire tree.
	};

	// Request the required permission.
	virtual ModulePerm RequestPermissionInfo()
	{
		return ModulePerm::ReadOnly;
	}

	// Call the module.
	virtual void Invoke(CryCC::AST::AST) = 0;
};

template<typename SequenceType>
class Module : public ModuleInterface
{
	friend Module<Sequencer::Interface>;

	using self_type = Module<SequenceType>;

	std::list<std::shared_ptr<Stream::OutputStream>> m_streamOut;
	std::shared_ptr<Stream::InputStream> m_streamIn;
	std::shared_ptr<SequenceType> m_sequencer;

	// Write output to streams all registered streams.
	void RelayOutput(uint8_t *data, size_t sz)
	{
		for (auto& outputStream : m_streamOut) {
			outputStream->Write(data, sz);
		}
	}

	// Read input from stream.
	void RelayInput(uint8_t *data, size_t sz)
	{
		if (m_streamIn) {
			m_streamIn->Read(data, sz);
		}
	}

	// ...
	void DoneOutput()
	{
		for (auto& outputStream : m_streamOut) {
			outputStream->WriteDone();
		}
	}

	// ...
	void DoneInput()
	{
		if (m_streamIn) {
			m_streamIn->ReadDone();
		}
	}

	// Should only invoke from the sequencer interface.
	virtual void Invoke(CryCC::AST::AST)
	{
		throw Cry::Except::UnsupportedOperationException{ "Module::Invoke" };
	}

public:
	Module()
	{
		const auto out = [this](uint8_t *data, size_t sz)
		{
			this->RelayOutput(data, sz);
		};
		const auto in = [this](uint8_t *data, size_t sz)
		{
			this->RelayInput(data, sz);
		};

		m_sequencer = std::make_shared<SequenceType>(std::move(out), std::move(in));
	}

	~Module()
	{
		DoneOutput();
		DoneInput();
	}

	// Only add stream to list of output streams.
	void AddStreamOut(std::shared_ptr<Stream::OutputStream>&& ptr)
	{
		m_streamOut.emplace_back(std::move(ptr));
	}

	// Set input stream if none configured, otherwise skip.
	void AddStreamIn(std::shared_ptr<Stream::InputStream>&& ptr)
	{
		if (!m_streamIn) {
			m_streamIn = std::move(ptr);
		}
	}

	template<typename StreamTy>
	void AddStream(std::shared_ptr<StreamTy>& ptr)
	{
		AddStreamOut(std::dynamic_pointer_cast<Stream::OutputStream>(ptr));
		AddStreamIn(std::dynamic_pointer_cast<Stream::InputStream>(ptr));
	}
};

template<>
class Module<Sequencer::Interface> : public ModuleInterface
{
	std::shared_ptr<Sequencer::Interface> m_sequencer;

public:
	Module() = default;

	template<typename SequenceType>
	Module(Module<SequenceType>&& sequencer)
		: m_sequencer{ std::move(sequencer.m_sequencer) }
	{
	}

	ModulePerm RequestPermissionInfo() override
	{
		//TODO: ask m_sequencer for permissions
		return ModuleInterface::RequestPermissionInfo();
	}

	virtual void Invoke(CryCC::AST::AST astWrapper)
	{
		m_sequencer->Execute(astWrapper);
	}
};

//FUTURE: Magic to work with dynamic loader
class ExtModule : public ModuleInterface
{
public:
	ExtModule() = default;

	virtual void Invoke()
	{
		//
	}
};

// The emitter serves as a forfront for the modules. The modules
// can update, alter or copy the abstract syntax tree. Depending
// on the module operation the invokation order is determined.
class Emitter : public CryCC::Program::Stage<Emitter>
{
	using ModulePair = std::pair<ModuleInterface::ModulePerm, Module<Sequencer::Interface>>;

public:
	Emitter(std::shared_ptr<CoilCl::Profile>& profile, CryCC::AST::AST&& ast, CryCC::Program::ConditionTracker::Tracker&);

	std::string Name() const { return "Emitter"; }

	Emitter& CheckCompatibility();
	Emitter& Process();

	template<typename ModuleType>
	Emitter& AddModule(Module<ModuleType>& ptr)
	{
		Module<Sequencer::Interface> modIface{ std::move(ptr) };

		return RegisterModule(std::move(modIface));
	}

private:
	Emitter & RegisterModule(Module<Sequencer::Interface>&&);
	CryCC::AST::AST Strategy(ModuleInterface::ModulePerm);

private:
	CryCC::AST::AST m_ast;
	std::vector<ModulePair> m_mods;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace Emit
} // namespace CoilCl
