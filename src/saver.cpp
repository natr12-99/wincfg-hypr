#include "include/saver.h"
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

        std::string floatStr, opacityStr, posStr, sizeStr;
        GetStrings(rule, floatStr, opacityStr, posStr, sizeStr);
        int lineCount = rule->lineNum.back();
        rule->lineNum.clear();

        if (floatStr.length() > 0)
        {
            content.insert(content.begin() + lineCount, floatStr);
            rule->lineNum.push_back(lineCount);
            lineCount++;
        }

        if (opacityStr.length() > 0)
        {
            content.insert(content.begin() + lineCount, opacityStr);
            rule->lineNum.push_back(lineCount);
            lineCount++;
        }

        if (posStr.length() > 0)
        {
            content.insert(content.begin() + lineCount, posStr);
            rule->lineNum.push_back(lineCount);
            lineCount++;
        }
        if (sizeStr.length() > 0)
        {
            content.insert(content.begin() + lineCount, sizeStr);
            rule->lineNum.push_back(lineCount);
            lineCount++;
        }

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

        std::string floatStr, opacityStr, posStr, sizeStr;
        GetStrings(rule, floatStr, opacityStr, posStr, sizeStr);

        std::vector<int> tvec;

        if (floatStr.length() > 0)
        {
            file << floatStr << std::endl;
            tvec.push_back(lineCount);
            lineCount++;
        }

        if (opacityStr.length() > 0)
        {
            file << opacityStr << std::endl;
            tvec.push_back(lineCount);
            lineCount++;
        }

        if (posStr.length() > 0)
        {
            file << posStr << std::endl;
            tvec.push_back(lineCount);
            lineCount++;
        }
        if (sizeStr.length() > 0)
        {
            file << sizeStr << std::endl;
            tvec.push_back(lineCount);
            lineCount++;
        }

        rule->lineNum = tvec;

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

void Saver::GetStrings(Rule* rule, std::string& floatStr, std::string& opacityStr, std::string& posStr,
                       std::string& sizeStr)
{
    std::string ruleCondition;
    std::string ruleStart = "windowrule = ";
    bool nameExist = false;
    if (rule->win == RegexType::fullMatch)
    {
        ruleCondition += "title:^" + rule->windowName + "$";
        nameExist = true;
    }
    else if (rule->win == RegexType::contain)
    {
        ruleCondition += "title:.*" + rule->windowName + ".*";
        nameExist = true;
    }

    if (rule->cls == RegexType::fullMatch)
    {
        if (nameExist)
        {
            ruleCondition += ", ";
        }
        ruleCondition += "class:^" + rule->windowClass + "$";
    }
    else if (rule->cls == RegexType::contain)
    {
        if (nameExist)
        {
            ruleCondition += ", ";
        }
        ruleCondition += "class:.*" + rule->windowClass + ".*";
    }

    if (rule->floating)
        floatStr = ruleStart + "float," + ruleCondition;

    if (rule->opacityActive != -1)
    {
        opacityStr = ruleStart + "opacity " + ToString(rule->opacityActive) + " " + ToString(rule->opacityInactive) +
                     "," + ruleCondition;
    }

    if (!rule->posX.empty() || !rule->posY.empty())
    {
        posStr = ruleStart + "move " + rule->posX + " " + rule->posY + "," + ruleCondition;
    }

    if (!rule->sizeX.empty() || !rule->sizeY.empty())
    {
        sizeStr = ruleStart + "size " + rule->sizeX + " " + rule->sizeY + "," + ruleCondition;
    }
}

std::string Saver::ToString(float value)
{
    std::stringstream s;
    s.imbue(std::locale("C"));
    s << value;
    return s.str();
}
