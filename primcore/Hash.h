#pragma once

#include "Algorithm.h"

namespace Primitives
{

class PRIMAPI Hash : public Algorithm
{
	const unsigned short m_blockSize;
	const unsigned short m_digestSize;
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

	virtual unsigned short GetDigestSize() const
	{
		return m_digestSize;
	}

public:
	Hash(const char *name, unsigned short blockSize = 0, unsigned short digestSize = 0)
		: Algorithm{name}
		, m_blockSize{blockSize}
		, m_digestSize{digestSize}
	{
	}

	virtual std::string CalcHash(const std::string& data) = 0;
	virtual void CalcHash(unsigned char *output, char *data, size_t szdata) = 0;

};

}
