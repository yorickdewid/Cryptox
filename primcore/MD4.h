#pragma once

#include "Hash.h"

namespace Primitives
{

class PRIMAPI MD4 : public Hash
{
public:
	MD4();

	std::string CalcHash(const std::string& data) override;

	static std::string Calculate(const std::string& data);
};

}
