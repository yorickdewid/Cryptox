// pfbase.cpp : Defines the exported functions for the DLL application.
//

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
	std::ofstream out{ m_name.c_str(), std::ios::out | std::ios::binary };
	out.write(reinterpret_cast<char *>(&store), sizeof(store));
}

void Project::ReadFromDisk()
{
	Store store;
	std::ifstream in{ m_name.c_str(), std::ios::in | std::ios::binary };
	in.read(reinterpret_cast<char *>(&store), sizeof(store));
}

}
