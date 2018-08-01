// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Copyright (c) 2018 Blub Corp. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

// Include local headers first
#include "Style.h"

// Include project-wide source
#include <Project/SomeSource.h>

// Include 3rd party soruce
#include <boost/filesystem.hpp>

// Include globals
#include <iostream>

// Unit definitions
#define SOME_CONST  1
#define SOME_CONST2 2
#define SOME_CONST3 10

namespace DefaultNamespace
{

namespace Subspace
{

template<typename TemplateType>
int HelperCall()
{
    const auto& ptr = std::make_shared<TemplateType>(12, "test");
    ptr->Initialize();
    return ptr->NativeAPICode(); 
}
    
} // namespace Subspace

class SomeClass
{
    int m_size;
    size_t m_offset;

public:
    // Set length of size.
    inline void SetLenth(int sz) { m_size = sz; }
    // Set offset of offset.
    inline void SetOffset(size_t offset) { m_offset = offset; }

};

// Decouple functions from classes if they are 
// only usefull in the current unit.
void MyFreeFunc()
{
    while (true) {
        Subspace::HelperCall<int>();
    }
}

} // namespace DefaultNamespace
