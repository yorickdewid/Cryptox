// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CoilCl/coilcl.h>

// Project includes.
#include <CryCC/AST.h>
#include <CryCC/Program.h>

// Framework includes.
#include <Cry/Except.h>

//TODO:
// - Split into separate units.

namespace CoilCl
{
namespace Emit
{
namespace Sequencer
{

struct Interface
{
	// Abstract result section which must be implemented by the sequencers.
	// The result section is tagged as such that other components can request
	// this specific result set and thus access the associated data.
	template<CryCC::Program::ResultInterface::slot_type Slot>
	struct AbstractResultSection : public CryCC::Program::ResultInterface
	{
		constexpr const static CryCC::Program::ResultInterface::slot_type slot_tag = Slot;

		// Implement abstract method.
		virtual size_type Size() const = 0;
		// Implement abstract method.
		virtual value_type& Data() = 0;
	};

	// Execute the sequencer.
	virtual void Execute(CryCC::AST::AST) = 0;
};

class CASM : public Interface
{
public:
	class ResultSection : public AbstractResultSection<result_section_tag::CASM>
	{
		value_type m_content;

	public:
		// Get size of section content.
		inline size_type Size() const noexcept { return m_content.size(); }
		// Get context object.
		inline value_type& Data() noexcept { return m_content; }
	};

public:
	virtual void Execute(CryCC::AST::AST)
	{
		throw Cry::Except::NotImplementedException{ "CASM::Execute" };
	}
};

class AIIPX : public Interface
{
	using IOCallback = std::function<void(uint8_t *data, size_t sz)>;

	// Input/Output stream callbacks.
	IOCallback m_outputCallback;
	IOCallback m_inputCallback;

public:
	class ResultSection : public AbstractResultSection<result_section_tag::AIIPX>
	{
		value_type m_content;

	public:
		// Get size of section content.
		inline size_type Size() const noexcept { return m_content.size(); }
		// Get context object.
		inline value_type& Data() noexcept { return m_content; }
	};

public:
	AIIPX(IOCallback outputCallback, IOCallback inputCallback)
		: m_outputCallback{ outputCallback }
		, m_inputCallback{ inputCallback }
	{
	}

	// Implement interface.
	virtual void Execute(CryCC::AST::AST tree)
	{
		PackAST(tree);
	}

	// Convert tree into output stream.
	void PackAST(CryCC::AST::AST);
	// Convert input stream into tree.
	void UnpackAST(CryCC::AST::AST&);
};

} // namespace Sequencer
} // namespace Emit
} // namespace CoilCl
