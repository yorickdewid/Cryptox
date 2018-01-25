#pragma once

#include <string>
#include <vector>

class Env;

void RunSourceFile(Env&, const std::string&);
void RunSourceFile(Env&, const std::vector<std::string>&);
void RunMemoryString(Env&, const std::string&);
