#pragma once

#include "include/regextype.h"
#include "string"
#include "vector"
#include <vector>
struct Rule;

class Loader {
public:
  bool Load(std::string path, std::vector<Rule> &rules);

private:
  RegexType GetRType(std::string &input);
  int isWindowRule(const std::string &line);
};
