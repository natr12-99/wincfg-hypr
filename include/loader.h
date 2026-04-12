#pragma once

#include "string"
#include "vector"
#include <vector>
struct Rule;

class Loader {
public:
  bool Load(std::string path, std::vector<Rule> &rules);

private:
  int isWindowRule(const std::string &line);
  std::string Trim(std::string &input);
};
