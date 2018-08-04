// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be
// copied and/or distributed without the express of the author.

#pragma once

#include "Profile.h"
#include "Tokenizer.h"

#include <CryCC/Program.h>

#include <boost/optional.hpp>

#include <map>
#include <deque>
#include <functional>

namespace CoilCl
{

namespace LocalMethod
{

class AbstractDirective;

} // namespace LocalMethod

//TODO: Move into interface file

// TokenProcessor interface is an interface which all 
// preprocessors must implement. The calling proxies
// assume any preprocessor to have this interface, even
// if the methods are a no-op.
struct TokenProcessor
{
	using TokenType = int;
	using DataType = Tokenizer::ValuePointer;

	template<typename TokenType, typename DataType>
	struct TokenDataPair
	{
		//TODO: already defined
		using token_type = TokenType;
		using data_type = DataType;

		constexpr TokenDataPair(token_type token)
			: m_token{ token }
		{
		}

		constexpr TokenDataPair(token_type token, data_type data)
			: m_token{ token }
			, m_data{ data }
		{
		}

		//
		// Query properties.
		//

		inline bool HasToken() const { return m_token.is_initialized(); }
		inline bool HasData() const { return m_data.is_initialized(); }
		inline bool HasTokenQueue() const { return m_tokenQueue.operator bool(); }
		inline bool HasTokenChanged() const noexcept { return tokenChangeCounter; }
		inline bool HasDataChanged() const noexcept { return dataChangeCounter; }

		//
		// Reset values.
		//

		inline void ResetToken() { m_token = boost::none; }
		inline void ResetData() { m_data = boost::none; }

		void AssignToken(token_type token) noexcept
		{
			m_token = token;
			++tokenChangeCounter;
		}

		void AssignData(const data_type& data) noexcept
		{
			// NOTE: Reset the data member first because we do not
			//       wish to assign a new value, but replace the current
			//       value with a new value. Value assignments are only
			//       for new internal values an not types.
			ResetData();
			m_data = data;
			++dataChangeCounter;
		}

		inline void EmplaceTokenQueue(std::unique_ptr<std::deque<TokenDataPair<token_type, const data_type>>>&& queue) noexcept
		{
			m_tokenQueue = std::move(queue);
		}

		const token_type& Token() const { return m_token.get(); }
		const data_type& Data() const { return m_data.get(); }

		inline int TokenChanges() const { return tokenChangeCounter; }
		inline int DataChanges() const { return dataChangeCounter; }

		inline std::unique_ptr<std::deque<TokenDataPair<token_type, const data_type>>> TokenQueue()
		{
			return std::move(m_tokenQueue);
		}

	private:
		std::unique_ptr<std::deque<TokenDataPair<token_type, const data_type>>> m_tokenQueue;
		int tokenChangeCounter = 0;
		int dataChangeCounter = 0;
		boost::optional<token_type> m_token; //TODO: token is never optional?
		boost::optional<data_type> m_data;
	};

	// Default token and data pair for most methods.
	using DefaultTokenDataPair = TokenDataPair<TokenType, DataType>;

	// This method is called is called for every token
	// and allows hooks to alter the token and data.
	// Preprocessors may override this method to
	// receive tokens.
	virtual void Propagate(bool, DefaultTokenDataPair&) {}

	//TODO: accept DefaultTokenDataPair instead
	// At the heart of the processor is the dispatch
	// method. Called on preprocessor directive and
	// can therefore not be ignored.
	virtual void Dispatch(DefaultTokenDataPair&) = 0;

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
	: public CryCC::Program::Stage<Preprocessor>
	, public TokenProcessor
{
public:
	Preprocessor(std::shared_ptr<CoilCl::Profile>&, CryCC::Program::ConditionTracker::Tracker);

	// Implement stage interface
	virtual std::string Name() const { return "Preprocessor"; }

	// Implement stage interface
	Preprocessor& CheckCompatibility();

	virtual void Propagate(bool isDirective, DefaultTokenDataPair& tokeData) override;
	virtual void Dispatch(DefaultTokenDataPair& tokenData);
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
