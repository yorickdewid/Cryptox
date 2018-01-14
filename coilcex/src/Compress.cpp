// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <lz4xx.h>

#include "Compress.h"

#define LOWER_LIMIT 1 << 6

using namespace CryExe;

namespace LZ4
{

class Compress
{
public:
	Compress(char **dest, size_t *destSize)
		: m_dest{ dest }
		, m_destSize{ destSize }
	{
	}

	void Run(const char *source, size_t size)
	{
		char *dest = new char[LZ4_COMPRESSBOUND(size) + 4];
		int cBlockSize = LZ4_compress_default(source, dest + 4, size, LZ4_COMPRESSBOUND(size));
		if (cBlockSize < 1) {
			throw std::exception{};
		}

		dest[0] = (size >> 24) & 0xff;
		dest[1] = (size >> 16) & 0xff;
		dest[2] = (size >> 8) & 0xff;
		dest[3] = size & 0xff;

		*m_dest = dest;
		*m_destSize = LZ4_COMPRESSBOUND(size) + 4;
	}

	template<typename _Ty>
	void Run(_Ty source, size_t size)
	{
		this->Run(reinterpret_cast<const char *>(source), size);
	}

private:
	char **m_dest;
	size_t *m_destSize;
};

class Decompress
{
public:
	Decompress(char **dest, size_t *destSize)
		: m_dest{ dest }
		, m_destSize{ destSize }
	{
	}

	void Run(const char *source, size_t size)
	{
		size_t origSze = (source[0] << 24) & 0xff
			| (source[1] << 16) & 0xff
			| (source[2] << 8) & 0xff
			| source[3] & 0xff;

		char *dest = new char[origSze];
		int outBlockSize = LZ4_decompress_safe(source + 4, dest, size - 4, origSze);

		*m_dest = dest;
		*m_destSize = origSze;
	}

private:
	char **m_dest;
	size_t *m_destSize;
};

} // namespace LZ4

void CompressConv::operator()(std::vector<uint8_t>& data)
{
	// Skip below lower limit
	if (data.size() < LOWER_LIMIT) {
		return;
	}

	//TOOD: compress when data is large enough
}
