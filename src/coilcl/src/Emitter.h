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
	virtual void Invoke(ASTNode *node) = 0;
};

template<typename _SeqTy>
class Module : public ModuleInterface
{
	friend Module<Sequencer::Interface>;

	using _MyTy = Module<_SeqTy>;

	std::vector<std::shared_ptr<Stream::OutputStream>> m_streamOut;
	std::shared_ptr<_SeqTy> m_sequencer;

public:
	Module()
	{
		m_sequencer = std::make_shared<_SeqTy>([this](uint8_t *data, size_t sz) {
			this->RelayOutput(data, sz);
		});
	}

	template<typename _StreamTy>
	void AddStream(std::shared_ptr<_StreamTy>& ptr)
	{
		m_streamOut.push_back(std::dynamic_pointer_cast<Stream::OutputStream>(ptr));
	}

	// Write output to streams
	void RelayOutput(uint8_t *data, size_t sz)
	{
		for (auto& outputStream : m_streamOut)
		{
			outputStream->Write(data, sz);
		}
	}

	// Should only invoke from the sequencer interface
	virtual void Invoke(ASTNode *node)
	{
		CRY_UNUSED(node);
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

	virtual void Invoke(ASTNode *node)
	{
		m_sequencer->Execute(node);
	}
};

//Future: Magic to work with dynamic loader
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

		const auto permInfo = modIface.RequestPermissionInfo();

		m_mods.push_back({ permInfo, std::move(modIface) });
		return (*this);
	}

private:
	AST::AST Strategy(ModuleInterface::ModulePerm);

private:
	AST::AST m_ast;
	std::vector<ModulePair> m_mods;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace Emit
} // namespace CoilCl
