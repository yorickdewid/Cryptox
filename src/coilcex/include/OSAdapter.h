// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#pragma once

#include "Exportable.h"

#include <string>
#include <atomic>

namespace CryExe
{

template<typename _Ty = long long>
class COILCEXAPI OSFilePositionImpl
{
	using _MyTy = OSFilePositionImpl<_Ty>;

private:
	_Ty _pos;

public:
	using type = _Ty;

public:
	OSFilePositionImpl(_Ty pos = -1)
		: _pos{ pos }
	{
	}

	_Ty NativePosition() const { return _pos; }

	template<typename _PTy> _PTy Position() const
	{
		return static_cast<_PTy>(_pos);
	}

	operator bool() const { return _pos != 0; }

	inline OSFilePositionImpl& operator++() { ++_pos; return (*this); }
	inline OSFilePositionImpl operator++(int) { _MyTy orig{ (*this) }; ++(*this); return orig; }

	inline OSFilePositionImpl& operator--() { --_pos; return (*this); }
	inline OSFilePositionImpl operator--(int) { _MyTy orig{ (*this) }; --(*this); return orig; }

	inline OSFilePositionImpl& operator+=(int i) { _pos += i; return (*this); }
	inline OSFilePositionImpl& operator-=(int i) { _pos -= i; return (*this); }

#define FRIEND_OP(op,ret) \
	friend ret operator op (const OSFilePositionImpl& lhs, const OSFilePositionImpl& rhs) \
	{ return lhs._pos op rhs._pos; }

	FRIEND_OP(+, OSFilePositionImpl);
	FRIEND_OP(-, OSFilePositionImpl);
	FRIEND_OP(%, OSFilePositionImpl);

	FRIEND_OP(== , bool);
	FRIEND_OP(!= , bool);
	FRIEND_OP(< , bool);
	FRIEND_OP(> , bool);
	FRIEND_OP(<= , bool);
	FRIEND_OP(>= , bool);

#define FRIEND_OP_SEC(op,ret,type) \
	friend ret operator op (const OSFilePositionImpl& lhs, type pos) \
	{ return lhs._pos op static_cast<decltype(lhs._pos)>(pos);  }

	FRIEND_OP_SEC(+, OSFilePositionImpl, int);
	FRIEND_OP_SEC(-, OSFilePositionImpl, int);
	FRIEND_OP_SEC(%, OSFilePositionImpl, int);

	FRIEND_OP_SEC(+, OSFilePositionImpl, size_t);
	FRIEND_OP_SEC(-, OSFilePositionImpl, size_t);
	FRIEND_OP_SEC(%, OSFilePositionImpl, size_t);

	FRIEND_OP_SEC(== , bool, int);
	FRIEND_OP_SEC(!= , bool, int);
	FRIEND_OP_SEC(< , bool, int);
	FRIEND_OP_SEC(> , bool, int);
	FRIEND_OP_SEC(<= , bool, int);
	FRIEND_OP_SEC(>= , bool, int);

	FRIEND_OP_SEC(== , bool, size_t);
	FRIEND_OP_SEC(!= , bool, size_t);
	FRIEND_OP_SEC(< , bool, size_t);
	FRIEND_OP_SEC(> , bool, size_t);
	FRIEND_OP_SEC(<= , bool, size_t);
	FRIEND_OP_SEC(>= , bool, size_t);
};

using OSFilePosition = OSFilePositionImpl<>;
using OSFilePositionSafe = OSFilePositionImpl<std::atomic_llong>;

class COILCEXAPI OSAdapter
{
	std::FILE *m_fpImage = nullptr;
	OSFilePosition m_fpOffset = 0;

public:
	OSAdapter() = default;
	~OSAdapter();

	bool IsOpen() const { return m_fpImage; }
	void Open(const std::string& file, const char mode[]);
	void Close();
	void Flush();
	void Rewind();
	void Forward();
	void Position(const OSFilePosition&);

	inline const OSFilePosition& Offset() const { return m_fpOffset; }

	template<typename _Ty>
	inline void Read(_Ty& buffer, size_t size = sizeof(_Ty), size_t count = 1)
	{
		ReadRaw(static_cast<void *>(std::addressof(buffer)), size, count);
	}

	template<typename _Ty>
	inline void Write(_Ty& buffer, size_t size = sizeof(_Ty), size_t count = 1)
	{
		WriteRaw(static_cast<const void *>(std::addressof(buffer)), size, count);
	}

	template<typename _Ty>
	inline void Rewrite(_Ty& buffer, size_t size = sizeof(_Ty), size_t count = 1)
	{
		auto curPos = m_fpOffset;
		WriteRaw(static_cast<const void *>(std::addressof(buffer)), size, count);
		m_fpOffset = curPos;
	}

private:
	void ReadRaw(void *buffer, size_t size, size_t count);
	void WriteRaw(const void *buffer, size_t size, size_t count);
};

} // namespace CryExecutable
