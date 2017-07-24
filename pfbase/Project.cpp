#include "Project.h"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace ProjectBase
{

struct StoreList
{
	size_t size;
	char name[64];
	unsigned int type;

	//StoreList()
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

	std::ofstream out{ m_name.c_str(), std::ios::out | std::ios::binary };
	out.write(reinterpret_cast<char *>(&store), sizeof(store));

	if (m_metaPtr) {
		out.write(reinterpret_cast<char *>(m_metaPtr.get()), sizeof(MetaData));
	}

	if (!m_objectStores.size()) {
		return;
	}

	for (auto& objstore : m_objectStores) {
		StoreList sl;
		sl.size = objstore.first.size();
		sl.type = objstore.second->Type();
		::strcpy_s(sl.name, 64, objstore.first.c_str());
		out.write(reinterpret_cast<char *>(&sl), sizeof(StoreList));
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

	if (store.hasMeta) {
		m_metaPtr = std::make_unique<MetaData>();
		in.read(reinterpret_cast<char *>(m_metaPtr.get()), sizeof(MetaData));
	}

	if (!store.objectStores) {
		return;
	}

	for (size_t i = 0; i < store.objectStores; ++i) {
		StoreList sl;
		in.read(reinterpret_cast<char *>(&sl), sizeof(StoreList));

		Store::MakeStore(static_cast<Store::FactoryObjectType>(sl.type), [=](std::shared_ptr<Store> ptr) {
			m_objectStores.insert(std::make_pair(std::string{ sl.name, sl.size }, ptr));
		});
	}
}

}
