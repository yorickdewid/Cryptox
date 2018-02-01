#include "Preprocessor.h"
#include "DirectiveScanner.h"

#include <cassert>
#include <iostream>

//#define BITSET(b) (m_bitset & b)

using namespace CoilCl;

static std::map<std::string, std::string> g_definitionList;

Preprocessor::Preprocessor(std::shared_ptr<CoilCl::Profile>& profile)
	: Stage{ this }
	, m_profile{ profile }
{
}

Preprocessor& Preprocessor::CheckCompatibility()
{
	return (*this);
}

namespace CoilCl
{
namespace LocalMethod
{

class AbstractDirective
{
public:
	AbstractDirective() = default;
	virtual void Dispence(int token, void *data) = 0;

protected:
	class DirectiveException : public std::runtime_error
	{
	public:
		DirectiveException(const std::string& message) noexcept
			: std::runtime_error{ message.c_str() }
		{
		}

		explicit DirectiveException(const std::string& directive, const std::string& message) noexcept
			: std::runtime_error{ (directive + ": " + message).c_str() }
		{
		}
	};

	class UnexpectedTokenException : public DirectiveException
	{
	public:
		//TODO ...
	};

	template<typename _Ty>
	inline _Ty ConvertDataAs(void *data)
	{
		assert(data);
		return static_cast<Valuedef::Value*>(data)->As<_Ty>();
	}

	void RequireData(void *data)
	{
		// Data was expected, throw if not found
		if (!data) { throw DirectiveException{ "expected constant" }; }
	}
};

class ImportSource : public AbstractDirective
{
	bool hasBegin = false;
	std::string tempSource;

	// Request input source push from the frontend
	void Import(const std::string& source)
	{
		std::cout << "import " << source << std::endl;

		//TODO: call push import sources
	}

public:
	void Dispence(int token, void *data)
	{
		switch (token) {
		case TK_LESS_THAN: // Global includes begin
			hasBegin = true;
			break;
		case TK_GREATER_THAN: // Global includes end
			if (!hasBegin) throw;
			Import(tempSource);
			break;
		case TK_CONSTANT: // Local include
			RequireData(data);
			Import(ConvertDataAs<std::string>(data));
			break;
		default:
			if (hasBegin) {
				RequireData(data);

				//TODO: stringify tokens

				tempSource.append(ConvertDataAs<std::string>(data));
				break;
			}
			throw DirectiveException{ "include", "expected constant or '<' after 'include'" };
		}
	}
};

// Definition and expansion
class DefinitionTag : public AbstractDirective
{
	std::string m_definitionName;

public:
	void Dispence(int token, void *data)
	{
		if (m_definitionName.empty()) {
			RequireData(data);
			m_definitionName = ConvertDataAs<std::string>(data);
			return;
		}

		//TODO: add shit
	}

	~DefinitionTag()
	{
		auto result = g_definitionList.insert({ m_definitionName, "kaas" });
		if (!result.second) {
			std::cout << "def " << m_definitionName << " already exists " << std::endl;
		}
		else {
			std::cout << "created def " << m_definitionName << std::endl;
		}
	}
};

// Remove definition from list
class DefinitionUntag : public AbstractDirective
{
public:
	void Dispence(int token, void *data)
	{
		RequireData(data);
		auto it = g_definitionList.find(ConvertDataAs<std::string>(data));
		if (it == g_definitionList.end()) {
			//TODO: Ouch?
			return;
		}
		g_definitionList.erase(it);
	}
};

// Conditional compilation
class ConditionalStatement : public AbstractDirective
{
public:
	void Dispence(int token, void *data)
	{
		//
	}
};

// Set source location to fixed line,col pair
class FixLocation : public AbstractDirective
{
public:
	void Dispence(int token, void *data)
	{
		//
	}
};

// Report linquistic error
class LinguisticError : public AbstractDirective
{
	const bool m_isFatal;

public:
	LinguisticError(bool fatal = true)
		: m_isFatal{ fatal }
	{
	}

	void Dispence(int token, void *data)
	{
		if (token != TK_CONSTANT) {
			throw DirectiveException{ "error", "expected constant after 'error'" };
		}

		if (m_isFatal) {
			throw DirectiveException{ ConvertDataAs<std::string>(data) };
		}
	}
};

} // namespace LocalMethod
} // namespace CoilCl

template<typename _Ty, typename... _ArgsTy>
auto MakeMethod(_ArgsTy... args) -> std::shared_ptr<_Ty>
{
	return std::make_shared<_Ty>(std::forward<_ArgsTy>(args)...);
}

void Preprocessor::MethodFactory(int token)
{
	using namespace LocalMethod;

	switch (token) {
	case TK_PP_INCLUDE:
		std::cout << "TK_PP_INCLUDE" << std::endl;
		m_method = MakeMethod<ImportSource>();
		break;
	case TK_PP_DEFINE:
		std::cout << "TK_PP_DEFINE" << std::endl;
		m_method = MakeMethod<DefinitionTag>();
		break;
	case TK_PP_UNDEF:
		std::cout << "TK_PP_UNDEF" << std::endl;
		m_method = MakeMethod<DefinitionUntag>();
		break;
	case TK_PP_IF:
		std::cout << "TK_PP_IF" << std::endl;
		m_method = MakeMethod<ConditionalStatement>();
		break;
		/*case TK_PP_IFDEF:
		std::cout << "TK_PP_IFDEF" << std::endl;
		break;
		case TK_PP_ELSE:
		std::cout << "TK_PP_ELSE" << std::endl;
		break;
		case TK_PP_ELIF:
		std::cout << "TK_PP_ELIF" << std::endl;
		break;
		case TK_PP_ENDIF:
		std::cout << "TK_PP_ENDIF" << std::endl;
		break;
		case TK_PP_PRAGMA:
		std::cout << "TK_PP_PRAGMA" << std::endl;
		m_method = std::make_unique<LocalMethod::CILExtension>();
		break;*/
	case TK_PP_LINE:
		std::cout << "TK_PP_LINE" << std::endl;
		m_method = MakeMethod<FixLocation>();
		break;
	case TK_PP_WARNING:
		std::cout << "TK_PP_WARNING" << std::endl;
		m_method = MakeMethod<LinguisticError>(false);
		break;
	case TK_PP_ERROR:
		std::cout << "TK_PP_ERROR" << std::endl;
		m_method = MakeMethod<LinguisticError>();
		break;
	default:
		throw StageBase::StageException{ Name(), "invalid preprocessing directive" };
	}
}

void Preprocessor::Dispatch(int token, void *data)
{
	// Call the method factory and store the next method as continuation
	if (!m_method) {
		MethodFactory(token);
		return;
	}

	// If continuation is set, continue on
	m_method->Dispence(token, data);
}

void Preprocessor::EndOfLine()
{
	// Reste directive method for next preprocessor line
	m_method.reset();
}
