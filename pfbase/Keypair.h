#pragma once

#include "Blob.h"

#include <string>
#include <vector>

namespace ProjectBase
{

typedef std::basic_string<unsigned char> contentVector;

class Keypair : public Blob
{
public:
	Keypair() = default;

	Keypair(const char name[], const unsigned char priv[], const unsigned char pub[])
		: Blob{ name }
		, privKey{ priv }
		, pubKey{ pub }
	{
	}

	Keypair(const char name[], std::pair<const unsigned char*, const unsigned char*>& pair)
		: Blob{ name }
		, privKey{ pair.first }
		, pubKey{ pair.second }
	{
	}

	Keypair(const char name[], const char algo[], std::pair<const unsigned char*, const unsigned char*>& pair)
		: Blob{ name }
		, algName{ algo }
		, privKey{ pair.first }
		, pubKey{ pair.second }
	{
	}

	std::string& Algorithm()
	{
		return algName;
	}

	/*Blob& operator<<(const std::string& content)
	{
		m_content += content;
		m_size = m_content.size();
		return *this;
	}*/

private:
	std::string algName;
	contentVector privKey;
	contentVector pubKey;
};

}
