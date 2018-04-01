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

class Env;

// Compile and run single source file.
void RunSourceFile(Env&, const std::string&);
// Compile and run multiple source files.
void RunSourceFile(Env&, const std::vector<std::string>&);
// Compile and run memory stream.
void RunMemoryString(Env&, const std::string&);

// Compile source file from source file.
void CompileSourceFile(Env&, const std::string&);
// Compile multiple source files.
void CompileSourceFile(Env&, const std::vector<std::string>&);
// Compile source file from memory.
void CompileMemoryString(Env&, const std::string&);
