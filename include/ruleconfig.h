#pragma once
#include "include/regextype.h"
#include "string"
#include <vector>
class Rule;
class RuleConfig
{
  public:
    void InitRule(std::string wName, std::string wClass);
    void SetLines(std::vector<int>& lines);
    void ChangeRuleStr(std::string strC, std::string strN);
    void ChangeWinRegEx(RegexType type);
    void ChangeClsRegEx(RegexType type);
    void ChangeOpacity(float active, float inactive);
    void ChangeFloating(bool floating);
    void ChangeSize(std::string x, std::string y);
    void RefreshRulesList();
    void ChangePos(std::string x, std::string y);
    bool Save(std::string path);

  private:
    Rule* rule = nullptr;
};
