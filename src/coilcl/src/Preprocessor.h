#pragma once

#include "Profile.h"
#include "Stage.h"

#include <map>
#include <queue>
#include <functional>
#include <unordered_map>

namespace CoilCl
{

class Preprocessor;

struct SubscriptionEvent
{
public:
	enum class TokenDesignator
	{
		TOKEN_ERASE,
		TOKEN_REPLACE,
	};

public:
	SubscriptionEvent() = default;

	//void Subscribe(std::function<void()> callback, Subscription subscription, Priority prio = Priority::UNDEFINED);

	void Invoke() const
	{

	}

public:
	TokenDesignator TokenResult = TokenDesignator::TOKEN_ERASE;

private:
	Preprocessor *m_parent;
	std::function<void()> m_subscrCallback;
	//Subscription m_subscription;
};

class PreProcSubscription
{
public:
	enum _SubScrType
	{
		ON_ALL_TOKENS,
		ON_EVERY_LINE,
	};

	enum class Priority
	{
		FIRST,
		UNDEFINED,
		LAST,
	};

public:
	PreProcSubscription() = default;

	void Subscribe(_SubScrType event)
	{
		/*switch (event) {
		case ON_ALL_TOKENS:
			m_allTokenSub.push(SubscriptionEvent{});
			break;
		case ON_EVERY_LINE:
			m_allTokenSub.push(SubscriptionEvent{});
			break;
		}*/
	}

	void Unsubscribe()
	{

	}

	void Fire(_SubScrType event)
	{
		/*switch (event) {
		case ON_ALL_TOKENS:
			while (!m_allTokenSub.empty()) {
				m_allTokenSub.top().Invoke();
			}
			break;
		case ON_EVERY_LINE:
			while (!m_everyLineSub.empty()) {
				m_allTokenSub.top().Invoke();
			}
			break;
		}*/
	}

private:
	//std::priority_queue<SubscriptionEvent> m_allTokenSub;
	//std::priority_queue<SubscriptionEvent> m_everyLineSub;
};

class Preprocessor : public Stage<Preprocessor>
{
public:
	using location = std::pair<size_t, size_t>;

	enum Option
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
	};

public:
	Preprocessor(std::shared_ptr<CoilCl::Profile>&);

	virtual std::string Name() const { return "Preprocessor"; }

	Preprocessor& Options(int optionSet)
	{
		m_bitset = optionSet;
		return (*this);
	}

	Preprocessor& CheckCompatibility();
	void Transform(std::string&);

	std::string DumpTranslationUnitChunk()
	{
		std::string tmp;
		Transform(std::ref(tmp));
		return tmp;
	}

private:
	void ImportSource(std::string);
	void Definition(const std::shared_ptr<SubscriptionEvent>& op, std::string);
	void DefinitionUntag(std::string);
	void ConditionalStatement();
	bool SkipWhitespace(char c);
	void ReplaceTokenDefinition();
	std::shared_ptr<SubscriptionEvent> ProcessStatement(const std::string& str);

private:
	std::map<std::string, std::string> m_definitionList;

	/*void RegisterSubscription(StatementOperation::Subscription subscr) const
	{
		if (m_subscriptonTrap.find(subscr) == m_subscriptonTrap.end()) {
			std::priority_queue<std::function<void()>> queue;
			queue.push(nullptr);
			m_subscriptonTrap[subscr] = queue;
		}
		else {
			m_subscriptonTrap[subscr].push(nullptr);
		}
	}*/

private:
	int m_bitset;
	PreProcSubscription m_subscriptionNotifiers;
	std::unordered_map<std::string, std::function<void(std::shared_ptr<SubscriptionEvent>&, std::string)>> m_keywords;
	std::shared_ptr<CoilCl::Profile> m_profile;
};

} // namespace CoilCl
