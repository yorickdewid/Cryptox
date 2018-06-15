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
// ByteInStream
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
ByteInStream& ByteInStream::operator>>(long&)
{
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned long&)
{
	return (*this);
}
ByteInStream& ByteInStream::operator>>(long long&)
{
	return (*this);
}
ByteInStream& ByteInStream::operator>>(unsigned long long&)
{
	return (*this);
}
ByteInStream& ByteInStream::operator>>(float&)
{
	return (*this);
}
ByteInStream& ByteInStream::operator>>(double&)
{
	return (*this);
}
ByteInStream& ByteInStream::operator>>(long double&)
{
	return (*this);
}
ByteInStream& ByteInStream::operator>>(bool& v)
{
	v = static_cast<bool>(this->Deserialize<Byte>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(ByteInStream&)
{
	return (*this);
}

//
// ByteOutStream
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
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned long v)
{
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(long long /*v*/)
{
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(unsigned long long /*v*/)
{
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(float /*v*/)
{
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(double /*v*/)
{
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(long double /*v*/)
{
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(bool v)
{
	this->SerializeAs<Cry::Byte>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(ByteOutStream /*v*/)
{
	//TODO
	return (*this);
}
