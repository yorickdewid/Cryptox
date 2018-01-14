// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "lz4xx.h"

LZ4Decoder::LZ4Decoder()
{
	init(LZ4BLOCKSIZE);
}

LZ4Decoder::LZ4Decoder(const size_t blockSize)
{
	init(blockSize);
}

void LZ4Decoder::init(const size_t blockSize)
{
	mBlockSize = blockSize;
	mTotalWritten = 0;
	mTotalRead = 0;
}

int LZ4Decoder::open(FILE* out)
{
	mOut.open(out);
	LZ4_setStreamDecode(&mStreamDecode, NULL, 0);
	return 0;
}

int LZ4Decoder::open(char** out, size_t* outlen)
{
	mOut.open(out, outlen);
	LZ4_setStreamDecode(&mStreamDecode, NULL, 0);
	return 0;
}

size_t LZ4Decoder::decode(FILE *in)
{
	mIn.open(in);
	return decode(-1);
}

size_t LZ4Decoder::decode(char* in, size_t inlen)
{
	mIn.open(in, inlen);
	return decode(inlen);
}

size_t LZ4Decoder::decode(size_t inlen)
{
	char *outBuf = nullptr;
	outBuf = (char *)malloc(sizeof(size_t) * mBlockSize);

	char *cmpBuf = nullptr;
	cmpBuf = (char *)malloc(LZ4_COMPRESSBOUND(mBlockSize));

	int inBlockSize;
	size_t remain = inlen;
	while (1) {
		size_t readCount = mIn.read(&inBlockSize, sizeof(inBlockSize));
		mTotalRead += readCount;
		if (inBlockSize <= 0 || readCount != sizeof(inBlockSize)) {
			return 0;
		}
		size_t readCountBuf = mIn.read(cmpBuf, inBlockSize);
		mTotalRead += readCountBuf;
		if ((int)readCountBuf != inBlockSize) {
			break;
		}
		//int outBlockSize = LZ4_decompress_safe_continue(&mStreamDecode, cmpBuf, outBuf, inBlockSize, mBlockSize);
		int outBlockSize = LZ4_decompress_safe(cmpBuf, outBuf, inBlockSize, mBlockSize);
		mTotalWritten += mOut.write(outBuf, outBlockSize);
		remain = inlen - outBlockSize;
		if (remain < mBlockSize) {
			break;
		}
	}

	return inlen - remain;
}

size_t LZ4Decoder::getTotalByteWritten() const
{
	return mTotalWritten;
}

size_t LZ4Decoder::getTotalByteRead() const
{
	return mTotalRead;
}
