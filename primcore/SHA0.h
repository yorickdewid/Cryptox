#pragma once

#include "Hash.h"

namespace Primitives
{

class PRIMAPI SHA0 : public Hash
{
public:
	SHA0();

	std::string CalcHash(const std::string& data) override;

	static std::string Calculate(const std::string& data);
};

}
