#include "Project.h"

#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

namespace ProjectBase
{

bool Project::StoreFileExist()
{
	return boost::filesystem::exists(m_name);
}

void Project::CommitToDisk()
{
	Store store;
	store.objectStores = m_objectStores.size();

	if (m_metaPtr) {
		store.hasMeta = true;
	}

	std::ofstream out{ m_name.c_str(), std::ios::out | std::ios::binary };
	out.write(reinterpret_cast<char *>(&store), sizeof(store));

	if (m_metaPtr) {
		out.write(reinterpret_cast<char *>(m_metaPtr.get()), sizeof(MetaData));
	}

	std::stringstream ss;
	boost::archive::text_oarchive oa{ ss };

	for (auto& objstore : m_objectStores) {
		std::cout << objstore.first << std::endl;

		oa << objstore.second.get();
	}

	out << ss.rdbuf();
}

void Project::ReadFromDisk()
{
	Store store;
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

	std::stringstream ss;

	ss << in.rdbuf();

	boost::archive::text_iarchive ia{ ss };

	for (size_t i = 0; i < store.objectStores; ++i) {
		std::shared_ptr<ObjectStore> x = std::make_shared<ObjectStore>();

		ia >> *x;

		/*ObjectStore::MakeStore(x, [=](const std::string& name, std::shared_ptr<ObjectStore> ptr) {
			m_objectStores.insert(std::make_pair(name, ptr));
		});*/
	}
}

}
