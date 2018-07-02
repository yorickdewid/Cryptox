// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#pragma once

#include <string>
#include <vector>

#define EXIT_BACKEND_FAILLURE 2
#define EXIT_LOCAL_FAILLURE 3
#define EXIT_NO_OPERATION 5

namespace Version
{

std::string Compiler();

} // namespace Version

class Env;

// Compile and run single source file.
int RunSourceFile(Env&, const std::string&, const std::vector<std::string>&);
// Compile and run multiple source files.
int RunSourceFile(Env&, const std::vector<std::string>&, const std::vector<std::string>&);
// Compile and run memory stream.
int RunMemoryString(Env&, const std::string&, const std::vector<std::string>&);

// Compile source file from source file.
int CompileSourceFile(Env&, const std::string&);
// Compile multiple source files.
int CompileSourceFile(Env&, const std::vector<std::string>&);
// Compile source file from memory.
int CompileMemoryString(Env&, const std::string&);
