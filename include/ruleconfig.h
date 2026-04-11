#pragma once

#include "rule.h"
#include <string>

class RuleConfig {
public:
  static void SetName(std::string name);
  static void InitRule(Rule &rul);

  static int Save(std::string path);

  static void SetEffectsString(std::string keyword, std::string str);
  static void SetPropsString(std::string keyword, std::string str);

  static void RemoveProp(std::string key);
  static void RemoveEffect(std::string key);

private:
  inline static Rule rule;
};
