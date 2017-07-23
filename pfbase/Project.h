// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef PROJECT_H_
#define PROJECT_H_

#include "pfbase.h"

#include "Store.h"
#include "File.h"
#include "ObjectStore.h"

#include <string>
#include <memory>
#include <map>

namespace ProjectBase
{

class Project
{
public:
	Project() = default;

	Project(const std::string& name)
		: m_name{ name }
	{
		StartProject();
	}

	Project(const std::string& name, const MetaData& meta)
		: m_name{ name }
		, m_metaPtr{ std::make_unique<MetaData>(meta) }
	{
		StartProject();
	}

	void Save()
	{
		CommitToDisk();
	}

	void Close()
	{
		CommitToDisk();
	}

	bool IsEncrypted() const {
		return false;
	}

	bool IsBackCompat() const {
		return false;
	}

	// Return the number of files in the project. This number is not
	// calculated on the fly.
	int FileCount() const {
		return 1;//TODO
	}

	inline std::string Name() const {
		return m_name;
	}

	inline std::string ProjectName() const {
		return "";//TODO
	}

	template <typename T>
	void AddStore(const std::string name) {
		m_objectStores.insert(std::make_pair(name, std::make_shared<T>()));
	}

	std::shared_ptr<ObjectStore> GetStore(const std::string& name) {
		return m_objectStores[name];
	}

	//void Append(File& file) {
	//	TODO
	//}

	//File Retrieve(const std::string& name) {
	//	File f{ name };
	//	return f;//TODO
	//}

	static std::unique_ptr<Project> LoadFile(const std::string& fileName) {
		return std::move(std::make_unique<Project>(fileName));
	}

private:
	void StartProject()
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

	~Project()
	{
		Close();
	}

private:
	PFBASEAPI bool StoreFileExist();
	PFBASEAPI void CommitToDisk();
	PFBASEAPI void ReadFromDisk();

private:
	std::string m_name;
	std::map<std::string, std::shared_ptr<ObjectStore>> m_objectStores;
	std::unique_ptr<MetaData> m_metaPtr = nullptr;

};

}

#endif  // PROJECT_H_
