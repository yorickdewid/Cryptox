#include "Project.h"

#include <boost/filesystem.hpp>

#include <fstream>

namespace ProjectBase
{

bool Project::StoreFileExist()
{
	return boost::filesystem::exists(m_name);
}

void Project::CommitToDisk()
{
	Store store;

	if (m_metaPtr) {
		store.hasMeta = true;
	}

	std::ofstream out{ m_name.c_str(), std::ios::out | std::ios::binary };
	out.write(reinterpret_cast<char *>(&store), sizeof(store));

	if (m_metaPtr) {
		out.write(reinterpret_cast<char *>(m_metaPtr.get()), sizeof(MetaData));
	}
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
}

void Project::Save()
{
	CommitToDisk();
}

void Project::Close()
{
	CommitToDisk();
}

}
