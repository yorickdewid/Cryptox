#pragma once

namespace CryDB {

	class Algorithm
	{
		const bool m_isWeak;
		const char *m_name;
		unsigned int m_year;

	protected:
		void SetYear(unsigned int year)
		{
			m_year = year;
		}

	public:
		// Is algorithm broken
		bool IsBroken()
		{
			return m_isWeak;
		}

		const char *GetName() const
		{
			return m_name;
		}

		const unsigned int GetYear() const
		{
			return m_year;
		}

	public:
		Algorithm(const char *name, unsigned int year = 0, bool isWeak = false) : m_name(name), m_year(year), m_isWeak(isWeak) {}
	};

	class Hash : public Algorithm
	{
		const unsigned short m_blockSize;
		const unsigned short m_digestSize;
		unsigned short m_Rounds;
		const char *m_structure;

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
		virtual int GetBlockSize() const
		{
			return m_blockSize;
		}

		virtual int GetDigestSize() const
		{
			return m_digestSize;
		}

	public:
		Hash(const char *name, unsigned short blockSize = 0, unsigned short digestSize = 0)
			: Algorithm(name), m_blockSize(blockSize), m_digestSize(digestSize)
		{
		}

		virtual void CalculateHash(char *output, char *data, size_t szdata) = 0;
	};

	class SHA0 : public Hash
	{
	public:
		SHA0() : Hash("SHA0", 512, 160)
		{
			SetYear(1993);
			SetStructure("Merkle–Damgård");
			SetRounds(80);
		}

		void CalculateHash(char *output, char *data, size_t szdata)
		{
			//
		}
	};

	class SHA : public Hash
	{
	public:
		SHA() : Hash("SHA1", 512, 160)
		{
			SetYear(1995);
			SetStructure("Merkle–Damgård");
			SetRounds(80);
		}

		void CalculateHash(char *output, char *data, size_t szdata)
		{
			//
		}
	};

	typedef SHA	SHA1;

	class MD5 : public Hash
	{
	public:
		MD5() : Hash("MD5", 512, 128)
		{
			SetYear(1992);
			SetStructure("Merkle–Damgård");
			SetRounds(54);
		}

		void CalculateHash(char *output, char *data, size_t szdata)
		{
			//
		}
	};

	template<typename T> class AlgorithmCollector
	{
		std::vector<T *> list;

	public:
		void Add(T *item)
		{
			list.push_back(item);
		}

		T *Get()
		{
			return list.at(0);
		}
	};

	typedef AlgorithmCollector<Hash> HashCollector;
}
