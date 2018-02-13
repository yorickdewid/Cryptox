// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#pragma once

#include "Profile.h"
#include "Stage.h"
#include "Tokenizer.h"

#include <map>
#include <functional>

#include <boost/optional.hpp>

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
	using TokenType = int;
	using DataType = Tokenizer::ValuePointer;

	template<typename _TokenTy, typename _DataTy>
	struct TokenDataPair
	{
		using token_type = _TokenTy;
		using data_type = _DataTy;

		constexpr TokenDataPair(_TokenTy token, _DataTy data)
			: m_token{ token }
			, m_data{ data }
		{
		}

		inline bool HasToken() const { return m_token.is_initialized(); }
		inline bool HasData() const { return m_data.is_initialized(); }
		
		inline bool HasTokenChanged() const noexcept { return tokenChangeCounter; }
		inline bool HasDataChanged() const noexcept { return dataChangeCounter; }

		inline void ResetToken() { m_token = boost::optional<_TokenTy>{}; }
		inline void ResetData() { m_data = boost::optional<_DataTy>{}; }

		void AssignToken(token_type token)
		{
			m_token = token;
			++tokenChangeCounter;
		}

		void AssignData(const data_type& data)
		{
			m_data = data;
			++dataChangeCounter;
		}

		const token_type& Token() const { return m_token.get(); }
		const data_type& Data() const { return m_data.get(); }

		inline int TokenChanges() const { return tokenChangeCounter; }
		inline int DataChanges() const { return dataChangeCounter; }

	private:
		int tokenChangeCounter = 0;
		int dataChangeCounter = 0;
		boost::optional<_TokenTy> m_token;
		boost::optional<_DataTy> m_data;
	};

	// Default token and data pair for most methods.
	using DefaultTokenDataPair = TokenDataPair<TokenType, DataType>;

	// This method is called is called for every token
	// and allows hooks to alter the token and data.
	// Preprocessors may override this method to
	// receive tokens.
	virtual void Propagate(bool, DefaultTokenDataPair&) {}

	// At the heart of the processor is the dispatch
	// method. Called on preprocessor directive and
	// can therefore not be ignored.
	virtual void Dispatch(TokenType token, const DataType data) = 0;

	// When end of preprocessor directive line is
	// reached, this method is called to signal end
	// of line. Preprocessors may ignore this operation.
	virtual void EndOfLine() {};
};

// The preprocessor is a separate stage and must therefore
// inherit from the stage base. All preprocessors are bound
// by a token processor contract and must implement certain
// methods. The preprocessor will fan any of the directives
// towards specialized structures via a factory.
class Preprocessor
	: public Stage<Preprocessor>
	, public TokenProcessor
{
public:
	Preprocessor(std::shared_ptr<CoilCl::Profile>&);

	// Implement stage interface
	virtual std::string Name() const { return "Preprocessor"; }

	// Implement stage interface
	Preprocessor& CheckCompatibility();

	virtual void Propagate(bool isDirective, DefaultTokenDataPair& tokeData) override;
	virtual void Dispatch(TokenType token, const DataType data);
	virtual void EndOfLine() override;

private:
	void MethodFactory(TokenType);

private:
	std::map<std::string, std::string> m_definitionList;
	std::shared_ptr<LocalMethod::AbstractDirective> m_method;

private:
	std::shared_ptr<Profile> m_profile;
};

} // namespace CoilCl
