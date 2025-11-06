#include "include/saver.h"
#include "include/regextype.h"
#include "include/rule.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

bool Saver::SaveStruct(Rule* rule, std::string path)
{
    setlocale(LC_ALL, "C");

    if (!rule->lineNum.empty())
    {
        std::vector<std::string> content;
        if (!DeleteStrings(rule->lineNum, content, path))
            return false;

        std::vector<std::string> strings;
        GetStrings(rule, strings);
        int lineCount = rule->lineNum.back();
        rule->lineNum.clear();
        for (auto str : strings)
        strToVector(str, content, lineCount, rule);

        std::ofstream outfile(path);
        for (auto str : content)
        {
            outfile << str << std::endl;
        }
        outfile.close();
    }
    else
    {
        int lineCount = 0;
        std::ifstream lineCounter(path);
        if (!lineCounter.good())
            return false;
        std::string uselessStr;
        while (std::getline(lineCounter, uselessStr))
            lineCount++;
        lineCounter.close();

        std::ofstream file(path, std::ios::app);
        std::vector<std::string> strings;
        GetStrings(rule, strings);

        std::vector<int> lineNum;
        
        for (auto str : strings)
        appendToFile(str, file, lineNum, lineCount);

        rule->lineNum = lineNum;

        file.close();
    }
    return true;
}

bool Saver::DeleteRule(std::vector<int>& ruleLineNum, std::string path)
{
    std::vector<std::string> content;
    if (!DeleteStrings(ruleLineNum, content, path))
        return false;
    std::ofstream outfile(path);
    for (auto str : content)
    {
        outfile << str << std::endl;
    }
    outfile.close();
    return true;
}

bool Saver::DeleteStrings(std::vector<int>& ruleLineNum, std::vector<std::string>& content, std::string path)
{
    std::ifstream infile(path);
    if (!infile.good())
        return false;
    std::string tmp;
    while (std::getline(infile, tmp))
        content.push_back(tmp);
    infile.close();

    std::sort(ruleLineNum.rbegin(), ruleLineNum.rend());
    for (int index : ruleLineNum)
    {
        if (index >= 0 && index < content.size())
        {
            content.erase(content.begin() + index);
        }
    }
    return true;
}

void Saver::GetStrings(Rule* rule,std::vector<std::string>& strings)
{
    std::string ruleCondition;
    std::string ruleStart = "windowrule = ";
    bool nameExist = false;

    switch (rule->win)
    {
    case RegexType::fullMatch:
        ruleCondition += "title:^" + rule->windowName + "$";
        nameExist = true;
        break;
    case RegexType::contain:
        ruleCondition += "title:.*" + rule->windowName + ".*";
        nameExist = true;
        break;
    case RegexType::containLeft:
        ruleCondition += "title:.*" + rule->windowName;
        nameExist = true;
        break;
    case RegexType::containRight:
        ruleCondition += "title:" + rule->windowName + ".*";
        nameExist = true;
        break;
    }

    switch (rule->cls)
    {
    case RegexType::fullMatch:
        if (nameExist)
        {
            ruleCondition += ", ";
        }
        ruleCondition += "class:^" + rule->windowClass + "$";
        break;
    case RegexType::contain:
        if (nameExist)
        {
            ruleCondition += ", ";
        }
        ruleCondition += "class:.*" + rule->windowClass + ".*";
        break;
    case RegexType::containLeft:
        if (nameExist)
        {
            ruleCondition += ", ";
        }
        ruleCondition += "class:.*" + rule->windowClass;
        break;
    case RegexType::containRight:
        if (nameExist)
        {
            ruleCondition += ", ";
        }
        ruleCondition += "class:" + rule->windowClass + ".*";
        break;
    }

    switch (rule->winType)
    {
    case WindowType::floating:
        strings.push_back(ruleStart + "float," + ruleCondition);
        break;
    case WindowType::fullscreen:
        strings.push_back(ruleStart + "fullscreen," + ruleCondition);
        break;
    case WindowType::tile:
        strings.push_back(ruleStart + "tile," + ruleCondition);
        break;
    case WindowType::maximize:
        strings.push_back(ruleStart + "maximize," + ruleCondition);
        break;
    };

    if (rule->opacityActive != -1)
    {
        strings.push_back(ruleStart + "opacity " + ToString(rule->opacityActive) + " " + ToString(rule->opacityInactive) +
                     "," + ruleCondition);
    }

    if (!rule->posX.empty() || !rule->posY.empty())
    {
        strings.push_back(ruleStart + "move " + rule->posX + " " + rule->posY + "," + ruleCondition);
    }

    if (!rule->sizeX.empty() || !rule->sizeY.empty())
    {
        strings.push_back(ruleStart + "size " + rule->sizeX + " " + rule->sizeY + "," + ruleCondition);
    }

    if (rule->isPinned)
        strings.push_back(ruleStart + "pin," + ruleCondition);

    if (rule->noMaxSize)
        strings.push_back(ruleStart + "nomaxsize," + ruleCondition);

    if (rule->stayFocused)
        strings.push_back(ruleStart + "stayfocused," + ruleCondition);

    if (rule->noInitialFocus)
        strings.push_back(ruleStart + "noinitialfocus," + ruleCondition);
}

std::string Saver::ToString(float value)
{
    std::stringstream s;
    s.imbue(std::locale("C"));
    s << value;
    return s.str();
}
void Saver::strToVector(std::string& str, std::vector<std::string>& content, int& lineCount, Rule* rule)
{
    if (!str.empty())
    {
        content.insert(content.begin() + lineCount, str);
        rule->lineNum.push_back(lineCount);
        lineCount++;
    }
}

void Saver::appendToFile(std::string& str, std::ofstream& file, std::vector<int> lineNum, int& lineCount)
{
    if (!str.empty())
    {
        file << str << std::endl;
        lineNum.push_back(lineCount);
        lineCount++;
    }
}
