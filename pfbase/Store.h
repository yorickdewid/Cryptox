// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORE_H_
#define STORE_H_

#include "StoreConfig.h"

#include <stdint.h>
#include <memory.h>
#include <string.h>

namespace ProjectBase {

typedef uint64_t SequenceNumber;

// Defines project options which are required upfront in order to 
// read the store contents.
enum StoreOptions
{
	StoreOptPlain = 0x0,
	StoreOptFullEncrypt = 0x1,
	StoreOptBackComp = 0x2,
};

struct Store
{
	// Magic value contains a readable string so that accidential ascii
	// conversions return a tracable dump
	unsigned char magic[16];

	// File format version number, this is only increased if the format changes
	unsigned short version;

	// Project store options
	enum StoreOptions projopt;

	// The sequence number is increased every time the store is altered
	// this makes it possible to compare to project files of the same type
	SequenceNumber seqNum;

	// Size of the file store header. Even if the structure is unknown, the parser knows the size
	// of the header and can skip to the actual contents
	unsigned hdrsz;

	Store()
		: projopt{ StoreOptPlain }
		, version{ Config::version }
		, seqNum{ 0 }
	{
		memcpy_s(magic, 16, Config::BANNER, sizeof(Config::BANNER));
		hdrsz = sizeof(this);
	}

private:
	void Validate()
	{
		//
	}
};

class MetaData
{
	unsigned int m_created_at;
	unsigned int m_updated_at;
	char m_projectName[128];
	char m_author[64];

public:
	MetaData(const char *projectName, const char *author)
	{
		memcpy_s(m_projectName, 128, projectName, strlen(projectName));
		memcpy_s(m_author, 64, author, strlen(author));
	}
};

} // namespace ProjectBase

#endif // STORE_H_
