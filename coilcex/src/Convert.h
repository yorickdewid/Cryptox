// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <vector>

namespace CryExe
{

struct Converter
{
	enum Operations
	{
		CO_NONE = 1 << 0,
		CO_ENCRYPT = 1 << 1,
		CO_COMPRESS = 1 << 2,
	};

	virtual void SetProcessOptions() = 0;
	virtual void Convert() = 0;
};

class ConvertToPersistent : public Converter
{
public:
	ConvertToPersistent(const std::vector<uint8_t>& inData)
	{
	}

	void SetProcessOptions()
	{
		//
	}

	void Convert()
	{
		//
	}
};

class ConvertFromPersistent : public Converter
{
public:
	ConvertFromPersistent(const std::vector<uint8_t>& inData)
	{
	}

	void SetProcessOptions()
	{
		//
	}

	void Convert()
	{
		//
	}
};

} // namespace CryExe
