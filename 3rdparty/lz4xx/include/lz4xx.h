// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "lz4.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define LZ4BLOCKSIZE 8192


//namespace LZ4XX
//{

namespace Detail
{

class Filter
{
	virtual size_t getTotalByteRead() const = 0;
	virtual size_t getTotalByteWritten() const = 0;
};

}

class LZ4Exception
{
public:
	LZ4Exception()
	{
	}
};


// A unified output stream for file/byte array.
class LZ4OutStream
{
public:
	LZ4OutStream();

	// open destination buffer
	void open(char **inbuf, size_t *sizeloc);

	// open destination file
	void open(std::FILE *fd_in);

	// write to buffer/file
	size_t write(const void *in, const size_t inlen);

protected:
	void **mBuf;
	size_t *mSize;
	size_t mSpace;
	int mIsOpened;
};

class LZ4InStream
{
public:
	LZ4InStream();

	// open source buffer
	void open(const void *ptr, size_t inlen);

	// open source file
	void open(std::FILE *fd);

	// read from source file/buffer
	size_t read(void *ptr, size_t size);

protected:
	size_t mSpace;
	char *mBuf;
	char *mReadPtr;
	int mIsOpened;
};

class LZ4Encoder : protected Detail::Filter
{
public:
	LZ4Encoder();
	LZ4Encoder(const size_t blockSize);
	~LZ4Encoder();

	// open input file
	int open(std::FILE *out);

	// open input byte array
	int open(char** out, size_t* outlen);

	// encode to file
	size_t encode(std::FILE *in);

	// encode to byte array
	size_t encode(char* in, size_t inlen);
	//size_t encode(uint8_t *in, size_t inlen);

	// close the encoding pipe
	size_t close();

	// get total byte read and written
	size_t getTotalByteRead() const;
	size_t getTotalByteWritten() const;

protected:
	size_t mBlockSize;          // block size
	size_t mBlockSpace;         // availalble space in block
	char*  mBlock;              // I/O block

	size_t encode(size_t inlen);
	size_t flush();              // internal - write block to file
	void init(const size_t blockSize = LZ4BLOCKSIZE);

	LZ4_stream_t mStream;       // stream
	LZ4OutStream mOut;          // output stream
	LZ4InStream mIn;            // input stream

	size_t mTotalRead;
	size_t mTotalWritten;
};

class LZ4Decoder : protected Detail::Filter
{
public:
	LZ4Decoder();
	LZ4Decoder(const size_t blockSize); // customized blocksize

	// open input file
	int open(std::FILE *out);

	// open input byte array
	int open(char** out, size_t* outlen);

	// decode to file
	size_t decode(std::FILE *in);

	// decode to byte array
	size_t decode(char* in, size_t inlen);

	// get total byte read and written
	size_t getTotalByteRead() const;
	size_t getTotalByteWritten() const;

protected:
	// block size
	size_t mBlockSize;

	void init(const size_t blockSize = LZ4BLOCKSIZE);
	size_t decode(size_t inlen);

	LZ4InStream mIn;
	LZ4OutStream mOut;

	// decode stream
	LZ4_streamDecode_t mStreamDecode;

	size_t mTotalRead;
	size_t mTotalWritten;
};

//} // namespace LZ4XX
