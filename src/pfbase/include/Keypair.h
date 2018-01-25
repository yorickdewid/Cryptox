// Copyright (c) 2017 Quenza Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#pragma once

#include "Blob.h"

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

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
		, algName{ algo }
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

	explicit Keypair(const char name[], std::pair<const char*, const char*>&& pair)
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

	explicit Keypair(const char name[], const char algo[], std::pair<const char*, const char*>&& pair)
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

	std::string Algorithm() const
	{
		return algName;
	}

	size_t Size() const
	{
		return privKey.size() + pubKey.size() + secret.size();
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

	friend std::istream& operator>>(std::istream& in, Keypair& pair)
	{
		std::string token;
		std::getline(in, token, '!');
		pair.type = static_cast<KeyType>(boost::lexical_cast<int>(token));
		std::getline(in, token, '!');
		pair.origName = token;
		std::getline(in, token, '!');
		pair.algName = token;
		std::getline(in, token, '!');
		pair.privKey = token;
		std::getline(in, token, '!');
		pair.pubKey = token;
		std::getline(in, token, '!');
		pair.secret = token;

		return in;
	}

private:
	std::string algName;
	std::string privKey;
	std::string pubKey;
	std::string secret;
	KeyType type;
};

} // namespace ProjectBase
