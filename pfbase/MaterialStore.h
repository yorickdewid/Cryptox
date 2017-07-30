#pragma once

#include "ObjectStore.h"
#include "Keypair.h"

namespace ProjectBase
{

class MaterialStore : public ObjectStore<Keypair>
{
public:
	MaterialStore()
		: ObjectStore{ Store::ObjectTypeMaterialStore }
	{
	}




	void AddKeypair(const char name[], const unsigned char priv[], const unsigned char pub[])
	{
		AddKeypair(Keypair(name, priv, pub));
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
		out << "KP";
	}

	void Parse(const std::string content)
	{
		//
	}

};

}
