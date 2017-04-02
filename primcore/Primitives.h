#pragma once

#include "NOT.h"
#include "AND.h"
#include "OR.h"
#include "XOR.h"
#include "NAND.h"
#include "NOR.h"
#include "XNOR.h"

#include "SHA0.h"
#include "SHA1.h"
#include "MD4.h"
#include "MD5.h"

#include <unordered_map>
#include <memory>

std::unordered_map<std::string, std::shared_ptr<Primitives::Logic>> logicList = {
	{"NOT", std::make_shared<Primitives::NOT>()},
	{"AND", std::make_shared<Primitives::AND>()},
	{"OR", std::make_shared<Primitives::OR>()},
	{"XOR", std::make_shared<Primitives::XOR>()},
	{"NAND", std::make_shared<Primitives::NAND>()},
	{"NOR", std::make_shared<Primitives::NOR>()},
	{"XNOR", std::make_shared<Primitives::XNOR>()},
};

std::unordered_map<std::string, std::shared_ptr<Primitives::Hash>> hashList = {
	{"SHA0", std::make_shared<Primitives::SHA0>()},
	{"SHA1", std::make_shared<Primitives::SHA1>()},
	{"MD4", std::make_shared<Primitives::MD4>()},
	{"MD5", std::make_shared<Primitives::MD5>()},
};
