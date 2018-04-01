#pragma once

#include <string>

struct Reader
{
	virtual std::string FetchNextChunk(size_t) = 0;
	virtual std::string FetchMetaInfo() = 0;
	virtual void SwitchSource(const std::string& source) = 0;
};
