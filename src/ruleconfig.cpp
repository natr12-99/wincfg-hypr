#include "include/ruleconfig.h"
#include "include/rule.h"
#include "include/saver.h"

void RuleConfig::InitRule(Rule &rul) { rule = rul; }
void RuleConfig::SetName(std::string name) { rule.name = name; }
void RuleConfig::SetEffectsString(std::string keyword, std::string str) {
  rule.effects[keyword] = str;
}

void RuleConfig::SetPropsString(std::string keyword, std::string str) {
  rule.props[keyword] = str;
}

void RuleConfig::RemoveProp(std::string key) { rule.props.erase(key); }

void RuleConfig::RemoveEffect(std::string key) { rule.effects.erase(key); }

int RuleConfig::Save(std::string path) {
  Saver saver;
  return saver.SaveStruct(&rule, path);
}
