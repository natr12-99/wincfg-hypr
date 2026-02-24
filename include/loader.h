#pragma once

#include "include/regextype.h"
#include "string"
#include "vector"
struct Rule;

class Loader {
public:
  bool LoadOnlyProps(std::vector<std::string> &winProps,
                     std::vector<std::string> &rulesStrings,
                     std::vector<std::vector<int>> &lineNum, std::string path);

  void LoadFull(std::string &input, Rule *rule);

private:
  RegexType GetRType(std::string &input);
  bool isWindowRule(const std::string &line);
};
