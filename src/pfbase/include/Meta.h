// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once

#include <Cry/Indep.h>

#include <memory.h>
#include <string.h>
#include <string>

namespace ProjectBase {

class MetaData
{
	char m_projectName[128];
	char m_author[64];

public:
	MetaData() = default;

	MetaData(const char *projectName, const char *author = nullptr)
	{
		memset(m_projectName, '\0', 128);
		memset(m_author, '\0', 64);

		CRY_MEMCPY(m_projectName, 128, projectName, strlen(projectName));
		if (author) {
			CRY_MEMCPY(m_author, 64, author, strlen(author));
		}
	}

	inline std::string Author() const
	{
		return std::string{ m_author };
	}

	inline std::string ProjectName() const
	{
		return std::string{ m_projectName };
	}
};

} // namespace ProjectBase
