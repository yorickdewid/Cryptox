#pragma once

class BuiltinType
{
public:
	enum class Specifier
	{
		VOID,
		CHAR,
		SHORT,
		INT,
		LONG,
		FLOAT,
		DOUBLE,
		BOOL,
	};

	BuiltinType(Specifier specifier)
		: m_specifier{ specifier }
	{
	}

private:
	Specifier m_specifier;
};
