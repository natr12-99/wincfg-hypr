#include "include/ruleconfig.h"
#include "include/rule.h"
#include "include/saver.h"

void RuleConfig::InitRule(std::string wName, std::string wClass, Rule* rul)
{
    if (nullptr != rule)
        delete rule;

    rule = rul; 
    rule->windowClass = wClass;
    rule->windowName = wName;
}

void RuleConfig::ChangeRuleStr(std::string strC, std::string strN)
{
    rule->windowClass = strC;
    rule->windowName = strN;
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
