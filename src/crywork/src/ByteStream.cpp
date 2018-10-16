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

ByteInStream& ByteInStream::operator>>(CharType::alias& v)
{
	v = static_cast<CharType::alias>(Buffer().Deserialize<std::make_unsigned_t<CharType::storage_type>>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(UnsignedCharType::alias& v)
{
	v = static_cast<UnsignedCharType::alias>(Buffer().Deserialize<UnsignedCharType::storage_type>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(ShortType::alias& v)
{
	v = static_cast<short>(Buffer().Deserialize<std::make_unsigned_t<ShortType::storage_type>>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(UnsignedShortType::alias& v)
{
	v = static_cast<unsigned short>(Buffer().Deserialize<UnsignedShortType::storage_type>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(IntegerType::alias& v)
{
	v = static_cast<int>(Buffer().Deserialize<std::make_unsigned_t<IntegerType::storage_type>>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(UnsignedIntegerType::alias& v)
{
	v = static_cast<unsigned int>(Buffer().Deserialize<UnsignedIntegerType::storage_type>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(LongType::alias& v)
{
	v = static_cast<long>(Buffer().Deserialize<std::make_unsigned_t<LongType::storage_type>>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(UnsignedLongType::alias& v)
{
	v = static_cast<unsigned long>(Buffer().Deserialize<std::make_unsigned_t<UnsignedLongType::storage_type>>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(LongLongType::alias& v)
{
	v = static_cast<long long>(Buffer().Deserialize<LongLongType::serialize_type>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(UnsignedLongLongType::alias& v)
{
	v = static_cast<unsigned long long>(Buffer().Deserialize<UnsignedLongLongType::serialize_type>());
	return (*this);
}
ByteInStream& ByteInStream::operator>>(FloatType::alias& v)
{
	Word w = Buffer().Deserialize<Word>();
	v = static_cast<FloatType::alias>(reinterpret_cast<float&>(w));
	return (*this);
}
ByteInStream& ByteInStream::operator>>(DoubleType::alias& v)
{
	DoubleWord w = Buffer().Deserialize<DoubleWord>();
	v = static_cast<DoubleType::alias>(reinterpret_cast<double&>(w));
	return (*this);
}
ByteInStream& ByteInStream::operator>>(LongDoubleType::alias& v)
{
	DoubleWord w = Buffer().Deserialize<DoubleWord>();
	v = static_cast<LongDoubleType::alias>(reinterpret_cast<double&>(w));
	return (*this);
}
ByteInStream& ByteInStream::operator>>(BoolType::alias& v)
{
	v = static_cast<BoolType::alias>(Buffer().Deserialize<Byte>());
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

ByteOutStream& ByteOutStream::operator<<(CharType::alias v)
{
	Buffer().SerializeAs<CharType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(UnsignedCharType::alias v)
{
	Buffer().SerializeAs<UnsignedCharType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(ShortType::alias v)
{
	Buffer().SerializeAs<ShortType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(UnsignedShortType::alias v)
{
	Buffer().SerializeAs<UnsignedShortType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(IntegerType::alias v)
{
	Buffer().SerializeAs<IntegerType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(UnsignedIntegerType::alias v)
{
	Buffer().SerializeAs<UnsignedIntegerType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(LongType::alias v)
{
	Buffer().SerializeAs<LongType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(UnsignedLongType::alias v)
{
	Buffer().SerializeAs<UnsignedLongType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(LongLongType::alias v)
{
	Buffer().SerializeAs<LongLongType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(UnsignedLongLongType::alias v)
{
	Buffer().SerializeAs<UnsignedLongLongType::serialize_type>(v);
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(FloatType::alias v)
{
	Buffer().Serialize(reinterpret_cast<Word&>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(DoubleType::alias v)
{
	Buffer().Serialize(reinterpret_cast<DoubleWord&>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(LongDoubleType::alias v)
{
	Buffer().Serialize(reinterpret_cast<DoubleWord&>(v));
	return (*this);
}
ByteOutStream& ByteOutStream::operator<<(BoolType::alias v)
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
