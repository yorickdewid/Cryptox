#include "Project.h"

#include <boost/filesystem.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

namespace ProjectBase
{

struct StoreList
{
	size_t nameSize;
	char name[64];
	unsigned int type;
	size_t contentSize;

	StoreList() = default;
	StoreList(size_t _size, unsigned int _type)
		: nameSize{ _size }
		, type{ _type }
	{
	}
	StoreList(const std::string& _name, unsigned int _type)
		: nameSize{ _name.size() }
		, type{ _type }
	{
		::strcpy_s(name, 64, _name.c_str());
	}
};

bool Project::StoreFileExist()
{
	return boost::filesystem::exists(m_name);
}

void Project::CommitToDisk()
{
	StoreFormat store;
	store.objectStores = m_objectStores.size();

	if (m_metaPtr) {
		store.hasMeta = true;
	}

	if (created) {
		store.createdAt = created;
	}
	else {
		created = store.createdAt;
		updated = store.updatedAt;
	}

	std::ofstream out{ m_name.c_str(), std::ios::out | std::ios::binary };
	out.write(reinterpret_cast<char *>(&store), sizeof(store));

	if (m_metaPtr) {
		out.write(reinterpret_cast<char *>(m_metaPtr.get()), sizeof(MetaData));
	}

	// Skip file stores and contents when there are none
	if (!m_objectStores.size()) {
		return;
	}

	for (auto& objstore : m_objectStores) {
		std::stringstream ss;
		StoreList sl{ objstore.first.size(), static_cast<unsigned int>(objstore.second->Type()) };
		::strcpy_s(sl.name, 64, objstore.first.c_str());

		ss << *objstore.second;
		sl.contentSize = ss.str().size();

		out.write(reinterpret_cast<char *>(&sl), sizeof(StoreList));
		out.write(ss.str().c_str(), sl.contentSize);
	}
}

void Project::ReadFromDisk()
{
	StoreFormat store;
	store.Reset();
	m_metaPtr.reset();

	std::ifstream in{ m_name.c_str(), std::ios::in | std::ios::binary };
	in.read(reinterpret_cast<char *>(&store), sizeof(store));

	store.Validate();

	created = store.createdAt;
	updated = store.updatedAt;

	if (store.hasMeta) {
		m_metaPtr = std::make_unique<MetaData>();
		in.read(reinterpret_cast<char *>(m_metaPtr.get()), sizeof(MetaData));
	}

	// Skip file stores and contents when there are none
	if (!store.objectStores) {
		return;
	}

	for (size_t i = 0; i < store.objectStores; ++i) {
		StoreList sl;
		in.read(reinterpret_cast<char *>(&sl), sizeof(StoreList));

		std::string content;
		content.resize(sl.contentSize);
		in.read(const_cast<char*>(content.data()), sl.contentSize);

		Store::MakeStore(static_cast<Store::FactoryObjectType>(sl.type), content, [=](std::shared_ptr<Store> ptr) {
			m_objectStores.insert(std::make_pair(std::string{ sl.name, sl.nameSize }, ptr));
		});
	}
}

}
