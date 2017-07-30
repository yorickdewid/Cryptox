#pragma once

#include "Blob.h"

#include <string>
#include <vector>

namespace ProjectBase
{

enum class KeyType
{
	KeyTypePair,
	KeyTypeSecret,
	keyTypeParameter,
	KeyTypeGenerator,
};

class Keypair : public Blob
{
public:
	Keypair() = default;

	Keypair(const char name[], const char algo[], const char priv[], const char pub[])
		: Blob{ name }
		, privKey{ priv }
		, pubKey{ pub }
		, type{ KeyType::KeyTypePair }
	{
	}

	explicit Keypair(const char name[], std::pair<const char*, const char*>& pair)
		: Blob{ name }
		, privKey{ pair.first }
		, pubKey{ pair.second }
		, type{ KeyType::KeyTypePair }
	{
	}

	explicit Keypair(const char name[], const char algo[], std::pair<const char*, const char*>& pair)
		: Blob{ name }
		, algName{ algo }
		, privKey{ pair.first }
		, pubKey{ pair.second }
		, type{ KeyType::KeyTypePair }
	{
	}

	explicit Keypair(const char name[], const char algo[], const char *sec)
		: Blob{ name }
		, algName{ algo }
		, secret{ sec }
		, type{ KeyType::KeyTypeSecret }
	{
	}

	std::string& Algorithm()
	{
		return algName;
	}

	friend std::ostream& operator<<(std::ostream& out, const Keypair& pair)
	{
		return out << static_cast<int>(pair.type) << '!'
			<< pair.origName << '!'
			<< pair.algName << '!'
			<< pair.privKey << '!'
			<< pair.pubKey << '!'
			<< pair.secret << '!'
			<< "KP";
	}

	friend std::istream& operator>>(std::istream& in, Keypair& file)
	{
		/*std::string token;
		std::getline(in, token, ':');
		file.m_size = boost::lexical_cast<size_t>(token);
		std::getline(in, token, ':');
		file.origName = token;
		std::getline(in, token, ':');
		file.m_content = token;*/

		return in;
	}

private:
	KeyType type;
	std::string algName;
	std::string privKey;
	std::string pubKey;
	std::string secret;
};

}
