// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>
#include <Cry/ByteStream.h>

using namespace Cry;

//
// ByteInStream.
//

ByteInStream& ByteInStream::operator>>(short& v)
{
	v = static_cast<short>(this->Deserialize<Short>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned short& v)
{
	v = static_cast<unsigned short>(this->Deserialize<Short>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(int& v)
{
	v = static_cast<int>(this->Deserialize<Word>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned int& v)
{
	v = static_cast<unsigned int>(this->Deserialize<Word>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(long& v)
{
	v = static_cast<long>(this->Deserialize<Word>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned long& v)
{
	v = static_cast<unsigned long>(this->Deserialize<Word>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(long long& v)
{
	v = static_cast<long long>(this->Deserialize<DoubleWord>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned long long& v)
{
	v = static_cast<unsigned long long>(this->Deserialize<DoubleWord>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(float& v)
{
	Word w = this->Deserialize<Word>();
	v = static_cast<float>(reinterpret_cast<float&>(w));
	return (*this);
}
ByteInStream& ByteInStream::operator>>(double& v)
{
	DoubleWord w = this->Deserialize<DoubleWord>();
	v = static_cast<double>(reinterpret_cast<double&>(w));
	return (*this);
}
ByteInStream& ByteInStream::operator>>(long double& v)
{
	DoubleWord w = this->Deserialize<DoubleWord>();
	v = static_cast<double>(reinterpret_cast<double&>(w));
	return (*this);
}
ByteInStream& ByteInStream::operator>>(bool& v)
{
	v = static_cast<bool>(this->Deserialize<Byte>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(ByteInStream& v)
{
	size_t vsz = static_cast<size_t>(this->Deserialize<Word>());
	std::copy(this->begin() + this->Offset(), this->begin() + this->Offset() + vsz, std::back_inserter(v));
	//TODO: fix offset
	return (*this);
}

//
// ByteOutStream.
//

ByteOutStream& ByteOutStream::operator<<(short v)
{
	this->SerializeAs<Cry::Short>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned short v)
{
	this->SerializeAs<Cry::Short>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(int v)
{
	this->SerializeAs<Cry::Word>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned int v)
{
	this->SerializeAs<Cry::Word>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(long v)
{
	this->SerializeAs<Cry::Word>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned long v)
{
	this->SerializeAs<Cry::Word>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(long long v)
{
	this->SerializeAs<Cry::DoubleWord>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned long long v)
{
	this->SerializeAs<Cry::DoubleWord>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(float v)
{
	this->Serialize(reinterpret_cast<Word&>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(double v)
{
	this->Serialize(reinterpret_cast<DoubleWord&>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(long double v)
{
	this->Serialize(reinterpret_cast<DoubleWord&>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(bool v)
{
	this->SerializeAs<Byte>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(ByteOutStream v)
{
	this->SerializeAs<Word>(v.size());
	this->insert(this->cend(), v.cbegin(), v.cend());
	return (*this);
}

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
