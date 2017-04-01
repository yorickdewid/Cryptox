#pragma once

#include "Hash.h"

namespace Primitives
{

class PRIMAPI SHA1 : public Hash
{
public:
	SHA1();

	std::string CalcHash(const std::string& data) override;
	void CalcHash(unsigned char *output, char *data, size_t szdata) override;

	static std::string Calculate(const std::string& data);
};

}
