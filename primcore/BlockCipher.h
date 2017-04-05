#pragma once

#include "Algorithm.h"

namespace Primitives
{

class PRIMAPI BlockCipher : public Algorithm
{
	const unsigned short m_keySize;
	const unsigned short m_blockSize;
	unsigned short m_Rounds; //TODO
	const char *m_structure; //TODO

protected:
	void SetStructure(const char *structure)
	{
		m_structure = structure;
	}

	void SetRounds(int rounds)
	{
		m_Rounds = rounds;
	}

public:
	virtual unsigned short GetBlockSize() const
	{
		return m_blockSize;
	}

	virtual unsigned short GetKeySize() const
	{
		return m_keySize;
	}

public:
	BlockCipher(const char *name, unsigned short blockSize = 0, unsigned short keySize = 0)
		: Algorithm{name}
		, m_blockSize{blockSize}
		, m_keySize{keySize}
	{
	}

	virtual std::string Encrypt(const std::string& data, const std::string& iv, const std::string& key) = 0;
	virtual void Encrypt(unsigned char *output, char *data, size_t szdata, char *iv, size_t sziv, char *key, size_t szkey) = 0;
	virtual std::string Decrypt(const std::string& data, const std::string& iv, const std::string& key) = 0;
	virtual void Decrypt(unsigned char *output, char *data, size_t szdata, char *iv, size_t sziv, char *key, size_t szkey) = 0;

};

}
