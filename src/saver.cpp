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

        std::string winTypeStr, opacityStr, posStr, sizeStr, pinStr;
        GetStrings(rule, winTypeStr, opacityStr, posStr, sizeStr, pinStr);
        int lineCount = rule->lineNum.back();
        rule->lineNum.clear();

        if (winTypeStr.length() > 0)
        {
            content.insert(content.begin() + lineCount, winTypeStr);
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
        
        if (pinStr.length() > 0)
        {
            content.insert(content.begin() + lineCount, pinStr);
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

        std::string winTypeStr, opacityStr, posStr, sizeStr, pinStr;
        GetStrings(rule, winTypeStr, opacityStr, posStr, sizeStr, pinStr);

        std::vector<int> tvec;

        if (winTypeStr.length() > 0)
        {
            file << winTypeStr << std::endl;
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
        
        if (pinStr.length() > 0)
        {
            file << pinStr << std::endl;
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

void Saver::GetStrings(Rule* rule, std::string& winTypeStr, std::string& opacityStr, std::string& posStr,
                       std::string& sizeStr, std:: string& pinStr)
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
        winTypeStr = ruleStart + "float," + ruleCondition;
        break;
    case WindowType::fullscreen:
        winTypeStr = ruleStart + "fullscreen," + ruleCondition;
        break;
    case WindowType::tile:
        winTypeStr = ruleStart + "tile," + ruleCondition;
        break;
    case WindowType::maximize:
        winTypeStr = ruleStart + "maximize," + ruleCondition; // в этой строке не уверен посмотри пж
        break;
    };

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
    
    if (rule->isPinned)
    pinStr = ruleStart + "pin," + ruleCondition; 
}

std::string Saver::ToString(float value)
{
    std::stringstream s;
    s.imbue(std::locale("C"));
    s << value;
    return s.str();
}
