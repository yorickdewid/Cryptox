// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#ifdef CRY_WINDOWS
#define CRY_CLI_DELIMITER "/"
#else
#define CRY_CLI_DELIMITER "-"
#endif // CRY_WINDOWS

#define VERSION_OPTION_LONG "version"
#define VERSION_OPTION_SHORT "v"

namespace Cry
{

using namespace boost::program_options;

class OptionParser
{
	command_line_parser parser;
	options_description options;
	positional_options_description positional;
	std::stringstream helpText;

	// Windows commandline style
	constexpr static int WindowsStyle = (command_line_style::allow_short
		| command_line_style::short_allow_adjacent
		| command_line_style::short_allow_next
		| command_line_style::allow_long
		| command_line_style::long_allow_adjacent
		| command_line_style::long_allow_next
		| command_line_style::allow_sticky
		| command_line_style::allow_guessing
		| command_line_style::allow_dash_for_short
		| command_line_style::case_insensitive
		| command_line_style::allow_slash_for_short
		| command_line_style::allow_long_disguise);

	// Unix and alike command like style
	constexpr static int UnixStyle = (command_line_style::allow_short
		| command_line_style::short_allow_adjacent
		| command_line_style::short_allow_next
		| command_line_style::allow_long
		| command_line_style::long_allow_adjacent
		| command_line_style::long_allow_next
		| command_line_style::allow_sticky
		| command_line_style::allow_guessing
		| command_line_style::allow_dash_for_short
		| command_line_style::allow_long_disguise);

	// Fetch commandline options style per platform
	inline int GetStyle() const
	{
#ifdef CRY_WINDOWS
		return WindowsStyle;
#else
		return UnixStyle;
#endif
	}

	// Shape helper output for current platform
	template<typename Input, typename = typename std::enable_if_t<std::is_move_constructible_v<Input>>>
	static Input&& PlatformHelperStyle(Input&& str)
	{
		if (str.size() > 0 && str[0] == '\n') { str.erase(0, 1); }
		if (str.back() == '\n' && str.at(str.size() - 1) == '\n') { str.pop_back(); }
		boost::algorithm::replace_all(str, "--", CRY_CLI_DELIMITER);
		return std::move(str);
	}

	// Add default options
	void AppendDefaultOptions()
	{
#ifdef CRY_WINDOWS
		options.add_options()("?", "");
#endif
		options.add_options()("help", "");
		options.add_options()(VERSION_OPTION_LONG "," VERSION_OPTION_SHORT, "Print version information and exit");
	}

public:
	inline OptionParser(int argc, const char *argv[])
		: parser{ argc, argv }
	{
	}

	// Run the commmandline parser
	void Run(variables_map& vm)
	{
		AppendDefaultOptions();

		try {
			const auto& parsedOptions = parser
				.options(options)
				.positional(positional)
				.style(GetStyle())
				.run();

			store(parsedOptions, vm, true);
		}
		catch (error& parser_error) {
			std::string str{ parser_error.what() };
			throw error{ PlatformHelperStyle(std::move(str)) };
		}
	}

	// This helper
	inline OptionParser& Options() { return (*this); }

	// Check if vesion is requested
	inline bool Version(variables_map& vm) const { return vm.count(VERSION_OPTION_LONG); }

	// Add parameters
	OptionParser& operator()(options_description& desc, bool show = true)
	{
		options.add(desc);
		if (show) { helpText << desc; }
		return (*this);
	}

	// Add positional parameters
	OptionParser& operator()(positional_options_description& pos)
	{
		positional = pos;
		return (*this);
	}

	// Print the help text
	friend std::ostream& operator<<(std::ostream& os, const OptionParser& other)
	{
		os << other.PlatformHelperStyle(other.helpText.str());
		return os;
	}
};

} // namespace Cry

#undef CRY_CLI_DELIMITER
