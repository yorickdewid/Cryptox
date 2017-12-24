// plgex.cpp : Defines the exported functions for the DLL application.
//

#include "plgex.h"

// This class is exported from the plgex.dll
class PLGEX_API Cplgex {
public:
	Cplgex(void);
	// TODO: add your methods here.
};

// This is an example of an exported function.
PLGEX_API int fnplgex(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see plgex.h for the class definition
Cplgex::Cplgex()
{
}
