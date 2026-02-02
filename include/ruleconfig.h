#pragma once

#include "rule.h"
#include <string>
#include <vector>

class RuleConfig {
public:
  void InitRule(std::string wName, std::string wClass, Rule *rul);
  void SetLines(std::vector<int> &lines) { rule->lineNum = lines; }
  bool Save(std::string path);

  void SetEffectsString(std::string keyword, std::string str);
  void SetPropsString(std::string keyword, std::string str);

  void RemoveProp(std::string key);
  void RemoveEffect(std::string key);

private:
  Rule *rule = nullptr;
};
