// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <CryCC/Program/Result.h>
#include <CryCC/Program/Program.h>

#include <sstream>

namespace CryCC
{
namespace Program
{

enum
{
    SCRATCHPAD_1 = -1,
    SCRATCHPAD_2 = -2,
    SCRATCHPAD_3 = -3,
    SCRATCHPAD_4 = -4,
};

template<ResultInterface::slot_type Slot>
class ScratchPad
{
    // FUTURE: write data into buffer.
    class ScratchPadProxy : public ResultInterface
    {
        value_type m_buffer;
    
    public:
        size_type Size() const
        {
            return m_buffer.size();
        }

        value_type& Data()
        {
            return m_buffer;
        }

		std::stringstream ss;
    };

    ScratchPadProxy& pad;

    ScratchPadProxy& Initalize(ProgramType& program)
    {
        ResultInterface& sp = program->ResultSectionSlot<ScratchPadProxy, Slot>();
	    return dynamic_cast<ScratchPadProxy&>(sp);
    }

    ScratchPadProxy& Initalize(Program& program)
    {
        ResultInterface& sp = program.ResultSectionSlot<ScratchPadProxy, Slot>();
	    return dynamic_cast<ScratchPadProxy&>(sp);
    }

public:
    template<typename TProgramType>
    ScratchPad(TProgramType& program)
        : pad{ Initalize(program) }
    {
    }

	template<typename Type>
	ScratchPad& operator<<(Type&& stream)
	{
		pad.ss << stream;
        return (*this);
	}

	std::string ToString() const
    {
        return pad.ss.str();
    }
};

} // namespace Program
} // namespace CryCC
