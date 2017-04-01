#include "Hash.h"

#include "SHA0.h"
#include "SHA1.h"
#include "MD5.h"

#include <unordered_map>
#include <memory>

std::unordered_map<std::string, std::shared_ptr<Primitives::Hash>> hashList = {
	{"SHA0", std::make_shared<Primitives::SHA0>()},
	{"SHA1", std::make_shared<Primitives::SHA1>()},
	{"MD5", std::make_shared<Primitives::MD5>()},
};

