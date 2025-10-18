#pragma once
#include "include/regextype.h"
#include "include/windowtype.h"
#include "rule.h"
#include "string"
#include <vector>
class RuleConfig
{
  public:
    void InitRule(std::string wName, std::string wClass);
    void SetLines(std::vector<int>& lines);
    void ChangeRuleStr(std::string strC, std::string strN);
    void ChangeWinRegEx(RegexType type);
    void ChangeClsRegEx(RegexType type);
    void ChangeOpacity(float active, float inactive);
    void ChangeWindowType(WindowType type);
    void ChangeSize(std::string x, std::string y);
    void RefreshRulesList();
    void ChangePos(std::string x, std::string y);
    bool Save(std::string path);
    void ChangePinned(bool isPinned);
    void ChangeStayFocused(bool stayFocused)
    {
        rule->stayFocused = stayFocused;
    }
    void ChangeNoMaxSize(bool noMaxSize)
    {
        rule->noMaxSize = noMaxSize;
    }
    void ChangeNoInitialFocus(bool noInitialFocus)
    {
        rule->noInitialFocus = noInitialFocus;
    }

  private:
    Rule* rule = nullptr;
};
