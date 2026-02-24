#include "include/ruleconfig.h"
#include "include/rule.h"
#include "include/saver.h"

void RuleConfig::InitRule(Rule *rul) {
  if (nullptr != rule)
    delete rule;

  rule = rul;
}

void RuleConfig::SetEffectsString(std::string keyword, std::string str) {
  rule->effects[keyword] = str;
}

void RuleConfig::SetPropsString(std::string keyword, std::string str) {
  rule->props[keyword] = str;
}

void RuleConfig::RemoveProp(std::string key) { rule->props.erase(key); }

void RuleConfig::RemoveEffect(std::string key) { rule->effects.erase(key); }

bool RuleConfig::Save(std::string path) {
  Saver saver;
  return saver.SaveStruct(rule, path);
}
