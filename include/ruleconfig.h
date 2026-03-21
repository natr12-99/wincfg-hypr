#pragma once

#include "rule.h"
#include <string>
#include <vector>

class RuleConfig {
public:
  static void InitRule(Rule *rul);
  static void SetLines(std::vector<int> &lines) { rule->lineNum = lines; }
  static int Save(std::string path);

  static void SetEffectsString(std::string keyword, std::string str);
  static void SetPropsString(std::string keyword, std::string str);

  static void RemoveProp(std::string key);
  static void RemoveEffect(std::string key);

private:
  inline static Rule *rule = nullptr;
};
