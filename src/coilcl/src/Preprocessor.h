#pragma once

#include "Profile.h"
#include "Stage.h"

#include <map>
#include <functional>

namespace CoilCl
{

namespace LocalMethod
{

class AbstractDirective;

} // namespace LocalMethod

// TokenProcessor interface is an interface which all 
// preprocessors must implement. The calling proxies
// assume any preprocessor to have this interface, even
// if the methods are a no-op.
struct TokenProcessor
{
	// This method is called is called for every token
	// and allows hooks to alter the token and data.
	// Preprocessors may override this method to
	// receive tokens.
	virtual void Propagate(int token, void *data) {}

	// At the heart of the processor is the dispatch
	// method. Called on preprocessor directive and
	// can therefore not be ignored.
	virtual void Dispatch(int token, void *data) = 0;
};

class Preprocessor :
	public Stage<Preprocessor>,
	public TokenProcessor
{
public:
	//using location = std::pair<size_t, size_t>;

	/*enum Option
	{
		PARSE_INCLUDE = 0x1,
		PARSE_DEFINE = 0x2,
		PARSE_MACRO = 0x4,
		PARSE_PRAGMA = 0x8,
		PARSE_ALL = 0xff,
	};

	enum class TokenDesignator
	{
		TOKEN_ERASE,
		TOKEN_REPLACE,
	};*/

public:
	Preprocessor(std::shared_ptr<CoilCl::Profile>&);

	virtual std::string Name() const { return "Preprocessor"; }

	Preprocessor& CheckCompatibility();

	void Dispatch(int token, void *data);
	void EndOfLine();

private:
	void MethodFactory(int token);

private:
	std::map<std::string, std::string> m_definitionList;
	std::shared_ptr<LocalMethod::AbstractDirective> m_method = nullptr;

private:
	std::shared_ptr<Profile> m_profile;
};

} // namespace CoilCl
