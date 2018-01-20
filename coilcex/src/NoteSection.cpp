// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#include "Executable.h"
#include "NoteSection.h"

#include <cassert>
#include <sstream>

using namespace CryExe;

#ifdef CXX_17

template<typename _Ty, typename = void>
struct is_container : std::false_type {};

template<typename _Ty>
struct is_container<_Ty
	, std::void_t<decltype(std::declval<_Ty>().data())
	, decltype(std::declval<_Ty>().size())>>
	: std::true_type {};

#endif

template<typename _CharTy>
struct NodeSerializer : private std::basic_stringstream<_CharTy>
{
	using _BaseTy = std::basic_stringstream<_CharTy>;

public:
	struct EOIMark {};

public:
	NodeSerializer& operator<<(const std::string& content)
	{
		auto size = content.size();
		_BaseTy::write(static_cast<_CharTy*>(static_cast<void*>(&size)), sizeof(std::uint32_t));
		_BaseTy::write(content.c_str(), content.size());
		return (*this);
	}

	NodeSerializer& operator<<(EOIMark k)
	{
		((void)k);

		_BaseTy::put(0x0);
		return (*this);
	}

#ifdef CXX_17
	template<typename _Ty, typename = std::enable_if<is_container<_Ty>::value>::type>
#else
	template<typename _Ty>
#endif
	_Ty Dump()
	{
		const std::string& str = _BaseTy::str();
		return _Ty{ str.cbegin(), str.cend() };
	}
};

using NodeStringSerializer = NodeSerializer<char>;
using NodeWstringSerializer = NodeSerializer<wchar_t>;

#define EOI EOIMark{}

NoteSection::NoteSection()
	: Section{ Section::SectionType::NOTE }
{
}

NoteSection::NoteSection(const std::string& name, const std::string& description)
	: Section{ Section::SectionType::NOTE }
	, m_name{ name }
	, m_description{ description }
{
}

void NoteSection::DataSwap()
{
	if (!Empty() || m_context.empty()) { return; }

	NodeStringSerializer serialize;
	serialize << m_name << NodeStringSerializer::EOI;
	serialize << m_description << NodeStringSerializer::EOI;
	serialize << m_context << NodeStringSerializer::EOI;

	Emplace(std::move(serialize.Dump<ByteArray>()));
}

void NoteSection::Clear()
{
	m_context.clear();
	Section::Clear();
}
