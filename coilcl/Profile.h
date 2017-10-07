#pragma once

#include "coilcl.h" //TODO: should not be a required file

#include <string>
#include <memory>

namespace Compiler
{

struct Profile
{
	virtual std::string ReadInput() = 0;
	virtual bool Include(const std::string&) = 0;
	virtual std::shared_ptr<metainfo_t> MetaInfo() = 0;
	virtual void Error(const std::string& message, bool isFatal) = 0;
};

} // namespace Compiler
