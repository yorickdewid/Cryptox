#pragma once

#include <string>

struct Reader
{
	virtual std::string FetchNextChunk(size_t) = 0;

};
