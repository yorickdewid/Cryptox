// pfbase.cpp : Defines the exported functions for the DLL application.
//

#include "Project.h"

namespace ProjectBase
{

struct ProjectMeta
{
	unsigned int ts_create;
	unsigned int ts_update;

	ProjectMeta() {
		ts_create = 0;
		ts_update = 0;
	}
};

}
