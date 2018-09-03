// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>
#include <Cry/ByteStream.h>

#include <algorithm>

using namespace Cry;

//
// ByteInStream.
//

ByteInStream& ByteInStream::operator>>(char& v)
{
	v = static_cast<char>(Buffer().Deserialize<Cry::Byte>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned char& v)
{
	v = static_cast<unsigned char>(Buffer().Deserialize<Cry::Byte>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(short& v)
{
	v = static_cast<short>(Buffer().Deserialize<Short>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned short& v)
{
	v = static_cast<unsigned short>(Buffer().Deserialize<Short>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(int& v)
{
	v = static_cast<int>(Buffer().Deserialize<Word>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned int& v)
{
	v = static_cast<unsigned int>(Buffer().Deserialize<Word>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(long& v)
{
	v = static_cast<long>(Buffer().Deserialize<Word>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned long& v)
{
	v = static_cast<unsigned long>(Buffer().Deserialize<Word>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(long long& v)
{
	v = static_cast<long long>(Buffer().Deserialize<DoubleWord>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned long long& v)
{
	v = static_cast<unsigned long long>(Buffer().Deserialize<DoubleWord>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(float& v)
{
	Word w = Buffer().Deserialize<Word>();
	v = static_cast<float>(reinterpret_cast<float&>(w));
	return (*this);
}
ByteInStream& ByteInStream::operator>>(double& v)
{
	DoubleWord w = Buffer().Deserialize<DoubleWord>();
	v = static_cast<double>(reinterpret_cast<double&>(w));
	return (*this);
}
ByteInStream& ByteInStream::operator>>(long double& v)
{
	DoubleWord w = Buffer().Deserialize<DoubleWord>();
	v = static_cast<double>(reinterpret_cast<double&>(w));
	return (*this);
}
ByteInStream& ByteInStream::operator>>(bool& v)
{
	v = static_cast<bool>(Buffer().Deserialize<Byte>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(std::byte& v)
{
	//FUTURE: remove temporary variable.
	unsigned char va;
	this->operator>>(va);
	v = static_cast<std::byte>(va);
	return (*this);
}
ByteInStream& ByteInStream::operator>>(std::string& va)
{
	const size_t vsz = static_cast<size_t>(Buffer().Deserialize<Word>());
	va.resize(vsz);
	for (auto& v : va) {
		this->operator>>(v);
	}
	return (*this);
}
ByteInStream& ByteInStream::operator>>(ByteInStream& v)
{
	const size_t vsz = static_cast<size_t>(Buffer().Deserialize<Word>());
	std::copy(Buffer().begin() + Buffer().Offset(), Buffer().begin() + Buffer().Offset() + vsz, std::back_inserter(v.Buffer()));
	//TODO: fix offset
	return (*this);
}
ByteInStream& ByteInStream::operator>>(FlagType v) //TODO:
{
	CRY_UNUSED(v);
	return (*this);
}

//
// ByteOutStream.
//

ByteOutStream& ByteOutStream::operator<<(char v)
{
	Buffer().SerializeAs<Cry::Byte>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned char v)
{
	Buffer().SerializeAs<Cry::Byte>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(short v)
{
	Buffer().SerializeAs<Cry::Short>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned short v)
{
	Buffer().SerializeAs<Cry::Short>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(int v)
{
	Buffer().SerializeAs<Cry::Word>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned int v)
{
	Buffer().SerializeAs<Cry::Word>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(long v)
{
	Buffer().SerializeAs<Cry::Word>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned long v)
{
	Buffer().SerializeAs<Cry::Word>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(long long v)
{
	Buffer().SerializeAs<Cry::DoubleWord>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned long long v)
{
	Buffer().SerializeAs<Cry::DoubleWord>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(float v)
{
	Buffer().Serialize(reinterpret_cast<Word&>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(double v)
{
	Buffer().Serialize(reinterpret_cast<DoubleWord&>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(long double v)
{
	Buffer().Serialize(reinterpret_cast<DoubleWord&>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(bool v)
{
	Buffer().SerializeAs<Byte>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(std::byte v)
{
	this->operator<<(static_cast<unsigned char>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(const std::string& v)
{
	this->WriteIterator(v.begin(), v.end());
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(ByteOutStream v)
{
	Buffer().SerializeAs<Word>(v.Buffer().size());
	Buffer().insert(Buffer().cend(), v.Buffer().cbegin(), v.Buffer().cend());
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(FlagType v) //TODO:
{
	CRY_UNUSED(v);
	return (*this);
}

//
// FileStream.
//
#if 0
FileStream::FileStream(const char *filename)
	: m_stream{ filename }
{
	//
}
FileStream::FileStream(const std::string filename)
	: m_stream{ filename }
{
	//
}
#endif