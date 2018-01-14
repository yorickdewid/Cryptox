// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "lz4xx.h"

LZ4Encoder::LZ4Encoder()
{
	init(LZ4BLOCKSIZE);
}

LZ4Encoder::LZ4Encoder(const size_t blockSize)
{
	init(blockSize);
}

void LZ4Encoder::init(const size_t blockSize)
{
	mBlockSize = blockSize;
	mBlock = NULL;
	mTotalWritten = 0;
	mTotalRead = 0;
	mBlock = new char[mBlockSize];
}

int LZ4Encoder::open(std::FILE* out)
{
	// set file
	mOut.open(out);
	// init LZ4
	LZ4_resetStream(&mStream);
	// init block size
	mBlockSpace = mBlockSize;
	return 0;
}

int LZ4Encoder::open(char** out, size_t* outlen)
{
	// init LZ4
	mOut.open(out, outlen);
	LZ4_resetStream(&mStream);
	// init block size
	mBlockSpace = mBlockSize;
	return 0;
}

size_t LZ4Encoder::encode(char* in, size_t inlen)
{
	mIn.open(in, inlen);
	return encode(inlen);
}

size_t LZ4Encoder::encode(std::FILE *in)
{
	mIn.open(in);
	return encode(-1);
}

size_t LZ4Encoder::encode(size_t inlen)
{
	size_t remain = inlen;
	while (1) {
		char* bufPtr = mBlock + mBlockSize - mBlockSpace;

		size_t toRead = (remain > mBlockSpace || inlen < 0) ? mBlockSpace : remain;

		size_t readSize = mIn.read(bufPtr, toRead);
		if (readSize == 0) {
			break;
		}
		remain -= readSize;
		mTotalRead += readSize;
		mBlockSpace -= readSize;
		if (remain <= 0) {
			break;
		}
		if (mBlockSpace == 0) {
			mTotalWritten += flush();
		}
	}

	return mTotalWritten;
}

size_t LZ4Encoder::close()
{
	int zero = 0;
	mTotalWritten += flush();
	mTotalWritten += mOut.write(&zero, sizeof(zero));
	return mTotalWritten;
}

size_t LZ4Encoder::flush()
{
	// current block size
	size_t written = 0;
	size_t rawBlockSize = mBlockSize - mBlockSpace;

	char *cBlock;
	cBlock = (char *)malloc(LZ4_COMPRESSBOUND(rawBlockSize));

	int cBlockSize = LZ4_compress_default(mBlock, cBlock, rawBlockSize, LZ4_COMPRESSBOUND(rawBlockSize));
	if (cBlockSize <= 0) {
		throw LZ4Exception{};
	}

	// write to mOutPtr
	written += mOut.write(&cBlockSize, sizeof(cBlockSize));
	written += mOut.write(&cBlock, (size_t)cBlockSize);

	// reset block
	mBlockSpace = mBlockSize;
	return written;
}

size_t LZ4Encoder::getTotalByteWritten() const
{
	return mTotalWritten;
}

size_t LZ4Encoder::getTotalByteRead() const
{
	return mTotalRead;
}

LZ4Encoder::~LZ4Encoder()
{
	if (mBlock != NULL) {
		delete[] mBlock;
	}
}

