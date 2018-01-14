// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "lz4xx.h"

LZ4OutStream::LZ4OutStream()
{
	mIsOpened = 0;
}

void LZ4OutStream::open(char** ptr, size_t* sizeloc)
{
	*sizeloc = 0;
	*ptr = NULL;
	mSize = sizeloc;
	mBuf = (void**)ptr;
	mSpace = 0;
	mIsOpened = 1;
}

void LZ4OutStream::open(FILE* fd)
{
	mBuf = (void**)fd;
	mSpace = 0;
	mSize = NULL;
	mIsOpened = 2;
}

size_t LZ4OutStream::write(const void* in, const size_t inlen)
{
	if (mIsOpened == 1) {
		if (*mSize + inlen > mSpace) {
			mSpace = ((*mSize) + inlen) << 1;
			*mBuf = (void*)realloc(*mBuf, mSpace);
			if (*mBuf == NULL) {
				return 0;
			}
		}
		memcpy((char*)*mBuf + *mSize, in, inlen);
		*mSize += inlen;
		return inlen;
	}
	if (mIsOpened == 2) {
		return fwrite(in, 1, inlen, (FILE*)mBuf);
	}
	return 0;
}


LZ4InStream::LZ4InStream()
{
	mIsOpened = 0;
}

void LZ4InStream::open(const void* ptr, size_t inlen)
{
	mSpace = inlen;
	mBuf = (char*)ptr;
	mReadPtr = mBuf;
	mIsOpened = 1;
}

void LZ4InStream::open(FILE *fd)
{
	mBuf = (char*)fd;
	mReadPtr = NULL;
	mIsOpened = 2;
}

size_t LZ4InStream::read(void* ptr, size_t size)
{
	if (mIsOpened == 1) {
		if ((((size_t)(mBuf + mSpace - mReadPtr)) < size) && (mSpace > 0)) {
			size = mBuf + mSpace - mReadPtr;
			if (size < 0) {
				return 0;
			}
		}
		memcpy(ptr, mReadPtr, size);
		mReadPtr += size;
		return size;
	}
	if (mIsOpened == 2) {
		return fread(ptr, 1, size, (FILE*)mBuf);
	}
	return 0;
}
