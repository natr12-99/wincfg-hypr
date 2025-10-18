#pragma once
#include "include/regextype.h"
#include "include/windowtype.h"
#include "rule.h"
#include "string"
#include <vector>
class RuleConfig
{
  public:
    void InitRule(std::string wName, std::string wClass, Rule* rul);
    void SetLines(std::vector<int>& lines){rule->lineNum = lines;}
    void ChangeRuleStr(std::string strC, std::string strN);
    void ChangeWinRegEx(RegexType type){rule->win = type;}
    void ChangeClsRegEx(RegexType type){rule->cls = type;}
    void ChangeOpacity(float active, float inactive);
    void ChangeWindowType(WindowType type){rule->winType = type;}
    void ChangeSize(std::string x, std::string y);
    void RefreshRulesList();
    void ChangePos(std::string x, std::string y);
    bool Save(std::string path);
    void ChangePinned(bool isPinned){rule->isPinned = isPinned;}
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
