#pragma once

#include "vector"
#include <string>
#include <unordered_map>

struct Rule {
  std::pmr::unordered_map<std::string, std::string> effects;
  std::pmr::unordered_map<std::string, std::string> props;
  std::string name;
  std::vector<int> lineNum;
};
