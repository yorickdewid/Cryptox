// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <Cry/Cry.h>
#include <Cry/Config.h>

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>

#define DEFUALT_VERSION 1
#define DEFAULT_NAMESPACE ""

const std::string resultingSource =
R"(// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <array>
#include <string>

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //
// AUTOGENERATED SOURCE, DO NOT EDIT MANUALLY. RUN THE MESSAGE
// GENERATOR TO UPDATE THE EVENT LOGGER.
//
// Manifest version : %d
// Event items      : %d
// Manifest         : %s
// Generated        : %s
//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! //

#define _EV_MIN_ITEM_ID %d
#define _EV_MAX_ITEM_ID %d

// List of aliases matching the error code. The alias is optional
// and is generated by the message generator.
enum struct AliasErrorCode
{
	//
};

struct EventLevel
{
	enum Level
	{%s
	};

	// Get shortcode from level
	static char GetCharCode(Level level) noexcept
	{
		switch (level)
		{%s
		}

		return '\0';
	}
};

struct EventItem final
{
	EventLevel::Level level{ EventLevel::%s };
	const int errorCode;
	const std::string title;
	const std::string desc;

public:
	EventItem(int code, const std::string& title)
		: errorCode{ code }
		, title{ title }
	{
	}
	EventItem(int code, const std::string& title, const std::string& desc)
		: errorCode{ code }
		, title{ title }
		, desc{ desc }
	{
	}
	EventItem(int code, EventLevel::Level level, const std::string& title)
		: level{ level }
		, errorCode{ code }
		, title{ title }
	{
	}
	EventItem(int code, EventLevel::Level level, const std::string& title, const std::string& desc)
		: level{ level }
		, errorCode{ code }
		, title{ title }
		, desc{ desc }
	{
	}

	std::string InfoLine() const
	{
		return ""
			+ std::string{ "Error: " }
			+ EventLevel::GetCharCode(level)
			+ std::to_string(errorCode)
			+ ": " + title;
	}
};

// List of events generated by the message generator.
static const std::array<EventItem, %d> g_eventItemList = std::array<EventItem, %d>
{%s
};

namespace Detail
{

struct MatchEvent final
{
	int m_code;

	inline MatchEvent(int code)
		: m_code{ code }
	{
	}

	inline bool operator()(const EventItem& event) const
	{
		return m_code == event.errorCode;
	}
};

// Retrieve event by error code
const EventItem& FetchEvent(int code)
{
	MatchEvent matcher{ code };
	if (code < _EV_MIN_ITEM_ID || code > _EV_MAX_ITEM_ID) {
		throw 1;
	}
	const auto result = std::find_if(g_eventItemList.cbegin(), g_eventItemList.cend(), matcher);
	if (result == g_eventItemList.cend()) {
		throw 1;
	}
	return (*result);
}

} // namespace Detail

namespace EventLog
{

struct FatalException : public std::exception
{
	std::string tmpStorage;
	const EventItem& m_eventLink;

public:
	FatalException(int code)
		: m_eventLink{ Detail::FetchEvent(code) }
	{
		tmpStorage = m_eventLink.InfoLine();
	}

	explicit FatalException(AliasErrorCode code)
		: m_eventLink{ Detail::FetchEvent(static_cast<int>(code)) }
	{
		tmpStorage = m_eventLink.InfoLine();
	}

	inline const EventItem Event() const noexcept
	{
		return m_eventLink;
	}

	virtual char const *what() const
	{
		return tmpStorage.c_str();
	}
};

template<typename... ArgsType>
FatalException MakeException(ArgsType&&... args)
{
	return FatalException{ std::forward<ArgsType>(args)... };
}

// Get the number of registered events in the static array.
inline size_t EventItemCount() noexcept { return %d; }
// Check if fatal exception has description.
inline bool HasDescription(const FatalException& ev) { return !ev.Event().desc.empty(); }
// Get the event message info line from an fatal exception.
inline std::string GetInfoLine(const FatalException& ev) { return ev.Event().InfoLine(); }
// Get the title from an fatal exception.
inline std::string GetTitle(const FatalException& ev) { return ev.Event().title; }
// Get the description from an fatal exception, if any.
inline std::string GetDescription(const FatalException& ev) { return ev.Event().desc; }

} // namespace EventLog

)";

static void AssertVersion(int version)
{
	if (version != DEFUALT_VERSION) {
		throw std::runtime_error{ "invalid manifest" };
	}
}

static std::string DateTime()
{
	auto t = std::time(nullptr);
	struct tm tm;
	CRY_LOCALTIME(&tm);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
	return oss.str();
}

struct Manifest final
{
	struct Level
	{
		char shortHand;
		const std::string name;
	};

	struct Event
	{
		const Level& level;
		const std::string title;
		const std::string description;
	};

	std::map<int, Level> m_levels;
	std::map<int, Event> m_events;

	// Load the manifest file.
	void Load(const std::string&);

	// Dump the events into the resulting source.
	void Dump(const std::string&, const std::string&);

private:
	int MinimumId();
	int MaximumId();
	std::string EventLevelLevelStub();
	std::string EventLevelGetCharCodeStub();
	std::string EventLevelDefaultValue();
	std::string ItemListStub();
};

int Manifest::MinimumId()
{
	return std::max_element(m_events.begin(), m_events.end(), [](const auto &lhs, const auto &rhs) -> bool
	{
		return (lhs.first > rhs.first);
	})->first;
}

int Manifest::MaximumId()
{
	return std::max_element(m_events.begin(), m_events.end(), [](const auto &lhs, const auto &rhs) -> bool
	{
		return (lhs.first < rhs.first);
	})->first;
}

std::string Manifest::EventLevelLevelStub()
{
	std::string out;
	for (const auto& level : m_levels) {
		out += "\n\t\t" + level.second.name + " = " + std::to_string(level.first) + ",";
	}
	return out;
}

std::string Manifest::EventLevelGetCharCodeStub()
{
	std::string out;
	for (const auto& level : m_levels) {
		out += "\n\t\tcase " + level.second.name + ": return '" + level.second.shortHand + "';";
	}
	return out;
}

std::string Manifest::EventLevelDefaultValue()
{
	return m_levels.begin()->second.name;
}

std::string Manifest::ItemListStub()
{
	std::string out;
	for (const auto& event : m_events) {
		if (event.second.description.empty()) {
			out += "\n\tEventItem{ " + std::to_string(event.first) + ", EventLevel::" + event.second.level.name + ", \"" + event.second.title + "\" },";
		}
		else {
			out += "\n\tEventItem{ " + std::to_string(event.first) + ", EventLevel::" + event.second.level.name + ", \"" + event.second.title + "\", \"" + event.second.description + "\" },";
		}
	}
	return out;
}

void Manifest::Load(const std::string& filename)
{
	using namespace boost::property_tree;

	// Create empty property tree object.
	ptree tree;

	// Parse the XML into the property tree.
	read_xml(filename, tree);

	// Assert manifest version.
	AssertVersion(tree.get<int>("manifest.<xmlattr>.version"));

	// Register the levels in the object.
	for (const auto& element : tree.get_child("manifest.levels")) {
		int id = element.second.get<int>("<xmlattr>.id");
		char code = element.second.get<char>("<xmlattr>.code");
		const std::string name = element.second.data();
		m_levels.emplace(std::pair<int, Manifest::Level>(id, { code, name }));
	}

	// Register events and match the levels.
	for (const auto& element : tree.get_child("manifest.events")) {
		int id = element.second.get<int>("<xmlattr>.id");
		int resolveLevel = element.second.get<int>("level");
		const std::string title = element.second.get<std::string>("title");
		if (m_levels.find(resolveLevel) == m_levels.end()) {
			throw std::runtime_error{ "referece level not found" };
		}
		const auto& levelObject = m_levels[resolveLevel];
		m_events.emplace(std::pair<int, Manifest::Event>(id, { levelObject, title }));
	}

	if (m_levels.empty()) {
		throw std::runtime_error{ "Specify at least one level" };
	}
	if (m_events.empty()) {
		throw std::runtime_error{ "Specify at least one event" };
	}
}

void Manifest::Dump(const std::string& inFile, const std::string& outFile)
{
	std::ofstream sourceFile;
	sourceFile.open(outFile);
	sourceFile << boost::format{ resultingSource }
		% DEFUALT_VERSION
		% m_events.size()
		% inFile
		% DateTime()
		% MinimumId()
		% MaximumId()
		% EventLevelLevelStub()
		% EventLevelGetCharCodeStub()
		% EventLevelDefaultValue()
		% m_events.size()
		% m_events.size()
		% ItemListStub()
		% m_events.size();
	sourceFile.close();
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		std::cerr << PROGRAM_UTIL_HEADER << std::endl << std::endl;
		std::cerr << argv[0] << ": MANIFEST OUTFILE" << std::endl;
		return 1;
	}

	try {
		Manifest manifest;
		manifest.Load(argv[1]);
		manifest.Dump(argv[1], argv[2]);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}