#include "include/ruleconfig.h"
#include "include/rule.h"
#include "include/saver.h"

void RuleConfig::InitRule(std::string wName, std::string wClass)
{
    if (nullptr != rule)
        delete rule;

    rule = new Rule();
    rule->windowClass = wClass;
    rule->windowName = wName;
}

void RuleConfig::SetLines(std::vector<int>& lines)
{
    rule->lineNum = lines;
}

void RuleConfig::ChangeRuleStr(std::string strC, std::string strN)
{
    rule->windowClass = strC;
    rule->windowName = strN;
}

void RuleConfig::ChangeWinRegEx(RegexType type)
{
    rule->win = type;
}

void RuleConfig::ChangeClsRegEx(RegexType type)
{
    rule->cls = type;
}

void RuleConfig::ChangeOpacity(float active, float inactive)
{
    if (active != -1)
    {
        rule->opacityActive = active / 100;
        rule->opacityInactive = inactive / 100;
    }
    else
    {
        rule->opacityActive = -1;
        rule->opacityInactive = -1;
    }
}

void RuleConfig::ChangeWindowType(WindowType type)
{
    rule->winType = type;
}

void RuleConfig::ChangeSize(std::string x, std::string y)
{
    rule->sizeX = x;
    rule->sizeY = y;
}

void RuleConfig::ChangePos(std::string x, std::string y)
{
    rule->posX = x;
    rule->posY = y;
}

bool RuleConfig::Save(std::string path)
{
    Saver saver;
    return saver.SaveStruct(rule, path);
}
