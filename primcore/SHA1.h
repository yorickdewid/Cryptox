#pragma once

#include "Hash.h"

namespace Primitives
{

class SHA1 : public Hash
{
public:
	SHA1();
	~SHA1();

	void CalculateHash(unsigned char *output, char *data, size_t szdata);
};

}
