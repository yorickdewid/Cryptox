#pragma once

#include <string>

#include "Store.h"

namespace ProjectBase
{

enum ProjectType {
	PROJECT_PROG,
	PROJECT_PROG,
};

class Project
{
public:
	void Save();
	void Load();

private:
	std::string name;
	Store store;

};

}
