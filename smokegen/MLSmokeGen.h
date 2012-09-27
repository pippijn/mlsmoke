#pragma once

#include <map>
#include <string>

class ClassInfo;

void genML (char const *targetDir, std::map<std::string, ClassInfo> const &classes);
