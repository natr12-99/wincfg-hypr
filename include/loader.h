#pragma once

#include "include/regextype.h"
#include "string"
#include "vector"
struct Rule;

class Loader {
public:
  bool LoadOnlyProps(std::vector<std::string> &winProps,
                     std::vector<std::vector<int>> &lineNum, std::string path);

  bool LoadFull(std::vector<int> &ruleLineNum, Rule *rule, std::string path);

private:
  bool FindMatches(std::vector<std::string> &vec,
                   std::vector<std::string> &vec2,
                   std::vector<RegexType> &type1, std::vector<RegexType> &type2,
                   std::string str, std::string str2, RegexType tp1,
                   RegexType tp2, int &index);

  RegexType GetRType(std::string &input);
};
