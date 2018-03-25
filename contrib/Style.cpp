// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Copyright (c) 2018 Blub Corp. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

namespace DefaultNamespace
{

namespace Subspace
{

template<typename _Ty>
int helper_call()
{
    const auto& ptr = std::make_shared<_Ty>(12, "test");
    ptr->Initialize();
    return ptr->NativeAPICode(); 
}
    
} // namespace Subspace

class SomeClass
{
    int m_size;
    size_t m_offset;

public:
    inline void SetLenth(int sz) { m_size = sz; }
    inline void SetOffset(size_t offset) { m_offset = offset; }

};

void MyFreeFunc()
{
    while (true) {
        Subspace::helper_call<int>();
    }
}

} // namespace DefaultNamespace
