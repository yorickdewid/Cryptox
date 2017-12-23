#pragma once

#include "ObjectStore.h"
#include "Keypair.h"

#include <boost/algorithm/string.hpp>

#include <iostream>

namespace ProjectBase
{

class MaterialStore : public ObjectStore<Keypair>
{
public:
	MaterialStore()
		: ObjectStore{ Store::ObjectTypeMaterialStore }
	{
	}

	void AddKeypair(const char name[], const char algo[], const char priv[], const char pub[])
	{
		AddKeypair(Keypair(name, algo, priv, pub));
	}

	void AddKeypair(Keypair& pair)
	{
		AddNode(pair);
	}

	Keypair& GetKeypair(const char name[])
	{
		return GetNode(name);
	}

	void DeleteKeypair(const char name[])
	{
		DeleteNode(name);
	}

	void Print(std::ostream& out) const override
	{
		for (auto& pair : nodeList) {
			out << '@' << pair;
		}
	}

	void Parse(const std::string content)
	{
		std::stringstream ss;
		ss.str(content);

		std::string token;
		while (std::getline(ss, token, '@')) {
			if (token.empty()) {
				continue;
			}

			std::istringstream oss;
			oss.str(token);

			Keypair pair;
			oss >> pair;

			AddKeypair(pair);
		}
	}

};

}
