// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef PROJECT_H_
#define PROJECT_H_

#include "pfbase.h"

#include "Store.h"
#include "File.h"

#include <string>
#include <memory>

namespace ProjectBase
{

class Project
{
public:
	Project() = default;

	Project(const std::string& name)
		: m_name{ name }
	{
		// Check if the given filename reflects a existing file. If so
		// read contents. When the file does not exist, create
		// a new store and flush to disk.
		if (StoreFileExist()) {
			ReadFromDisk();
		}
		else {
			CommitToDisk();
		}
	}

	Project(const std::string& name, const MetaData& meta)
		: Project{name}
	{
	}

	void Save() {} //TODO
	void SaveAs() {} //TODO
	void Open() {} //TODO
	void Close() {} //TODO

	bool isEncrypted() const {
		return false;
	}

	bool isBackCompat() const {
		return false;
	}

	// Return the number of files in the project. This number is not
	// calculated on the fly.
	int FileCount() const {
		return 1;
	}

	inline std::string Name() const {
		return m_name;
	}

	inline std::string ProjectName() const {
		return "";//TODO
	}

	void Append(File& file)
	{
		//TODO
	}

	File Retrieve(const std::string& name)
	{
		File f{ name };
		return f;//TODO
	}

	static std::unique_ptr<Project> LoadFile(const std::string& fileName) {
		return std::move(std::make_unique<Project>(fileName));
	}

private:
	PFBASEAPI bool StoreFileExist();
	PFBASEAPI void CommitToDisk();
	PFBASEAPI void ReadFromDisk();

private:
	std::string m_name;

};

}

#endif  // PROJECT_H_
