// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Profile.h"
#include "Stage.h"
#include "EmitterStream.h"
#include "Sequencer.h"

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
		ReadOnly,    // Request data only (default)
		AppendData,  // Append non-executable data to the tree
		CopyOnWrite, // Copy-in new or altered node when touched
		Substitute,	 // Substitue the entire tree
	};

	// Request the required permission
	virtual ModulePerm RequestPermissionInfo()
	{
		return ModulePerm::ReadOnly;
	}

	// Call the module
	virtual void Invoke(AST::AST) = 0;
};

template<typename _SeqTy>
class Module : public ModuleInterface
{
	friend Module<Sequencer::Interface>;

	using _MyTy = Module<_SeqTy>;

	std::vector<std::shared_ptr<Stream::OutputStream>> m_streamOut;
	std::shared_ptr<Stream::InputStream> m_streamIn;
	std::shared_ptr<_SeqTy> m_sequencer;

public:
	Module()
	{
		const auto out = [this](uint8_t *data, size_t sz) {
			this->RelayOutput(data, sz);
		};
		const auto in = [this](uint8_t *data, size_t sz) {
			this->RelayInput(data, sz);
		};

		m_sequencer = std::make_shared<_SeqTy>(std::move(out), std::move(in));
	}

	// Only add stream to list of output streams
	void AddStreamOut(std::shared_ptr<Stream::OutputStream>&& ptr)
	{
		m_streamOut.push_back(std::move(ptr));
	}

	// Set input stream if none configured, otherwise skip
	void AddStreamIn(std::shared_ptr<Stream::InputStream>&& ptr)
	{
		if (!m_streamIn) {
			m_streamIn = std::move(ptr);
		}
	}

	template<typename _StreamTy>
	void AddStream(std::shared_ptr<_StreamTy>& ptr)
	{
		AddStreamOut(std::dynamic_pointer_cast<Stream::OutputStream>(ptr));
		AddStreamIn(std::dynamic_pointer_cast<Stream::InputStream>(ptr));
	}

	// Write output to streams
	void RelayOutput(uint8_t *data, size_t sz)
	{
		for (auto& outputStream : m_streamOut)
		{
			outputStream->Write(data, sz);
		}
	}

	// Read input from streams
	void RelayInput(uint8_t *data, size_t sz)
	{
		if (m_streamIn) {
			m_streamIn->Read(data, sz);
		}
	}

	// Should only invoke from the sequencer interface
	virtual void Invoke(AST::AST)
	{
	}
};

template<>
class Module<Sequencer::Interface> : public ModuleInterface
{
	std::shared_ptr<Sequencer::Interface> m_sequencer;

public:
	Module() = default;

	template<typename _SeqTy>
	Module(Module<_SeqTy>&& sequencer)
		: m_sequencer{ std::move(sequencer.m_sequencer) }
	{
	}

	ModulePerm RequestPermissionInfo() override
	{
		//TODO: ask m_sequencer for permissions
		return ModuleInterface::RequestPermissionInfo();
	}

	virtual void Invoke(AST::AST astWrapper)
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
class Emitter : public Stage<Emitter>
{
	using ModulePair = std::pair<ModuleInterface::ModulePerm, Module<Sequencer::Interface>>;

public:
	Emitter(std::shared_ptr<CoilCl::Profile>& profile, AST::AST&& ast);

	std::string Name() const { return "Emitter"; }

	Emitter& CheckCompatibility();
	Emitter& Process();

	template<typename _Ty>
	Emitter& AddModule(Module<_Ty>& ptr)
	{
		Module<Sequencer::Interface> modIface{ std::move(ptr) };

		return RegisterModule(std::move(modIface));
	}

private:
	Emitter & RegisterModule(Module<Sequencer::Interface>&&);
	AST::AST Strategy(ModuleInterface::ModulePerm);

private:
	AST::AST m_ast;
	std::vector<ModulePair> m_mods;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace Emit
} // namespace CoilCl
