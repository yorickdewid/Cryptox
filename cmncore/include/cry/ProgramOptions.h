// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include "Cry.h"

#if WIN32
#define CRY_CLI_DELIMITER "/"
#else
#define CRY_CLI_DELIMITER "-"
#endif // WIN32

#define CRY_PROGOPT_STYLE(prog) \
	style(prog::command_line_style::default_style \
	| prog::command_line_style::case_insensitive \
	| prog::command_line_style::allow_slash_for_short \
	| prog::command_line_style::allow_long_disguise)