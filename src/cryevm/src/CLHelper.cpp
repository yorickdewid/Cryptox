// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include <string>
#include <vector>

// The compiler helper module extends the standard library and
// offers tools for debugging, trouble-shooting and diagnostics.
// The functions can only be called from an CIL source unit as
// the behaviour is compiler specific.

//
// Language extensions
//

int EvalString(const std::string&) { return 1; }
int CompileString(const std::string&) { return 1; }
void ThrowException(int) {}

//
// Hooks
//

void SetErrorHook(void(*func)(void)) {}
void SetDebugHook(void(*func)(const char *)) {}
void SetStartupHook(void(*func)(void)) {}
void SetExitHook(void(*func)(void)) {}
void SetSignalHook(void(*func)(int)) {}

//
// Diagnostics
//

void DumpVariable() {}
void DumpNode() {}
void SetDebugMode(bool) {}
void SetProfileMode(bool) {}
bool QueryDebugMode() { return false; }
void QueryProfileMode() { }

//
// Reflection
//

std::vector<std::string> ContextDeclarations() { return {}; }
std::vector<std::string> ProgramSymbols() { return {}; }
std::string TypeInfo() { return ""; }
std::string CurrentCallStack() { return ""; }
std::string CurrentMethod() { return ""; }
std::string ParentMethod() { return ""; }
std::string MainMethod() { return ""; }
std::string CurrentUnit() { return ""; }
std::string CurrentProgram() { return ""; }
