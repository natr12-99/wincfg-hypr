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

        std::string winTypeStr, opacityStr, posStr, sizeStr, pinStr, noMaxSizeStr, stayFocusedStr, noInitialFocusStr;
        GetStrings(rule, winTypeStr, opacityStr, posStr, sizeStr, pinStr, noMaxSizeStr, stayFocusedStr,
                   noInitialFocusStr);
        int lineCount = rule->lineNum.back();
        rule->lineNum.clear();
        strToVector(winTypeStr, content, lineCount, rule);

        strToVector(opacityStr, content, lineCount, rule);

        strToVector(posStr, content, lineCount, rule);

        strToVector(sizeStr, content, lineCount, rule);

        strToVector(pinStr, content, lineCount, rule);

        strToVector(noMaxSizeStr, content, lineCount, rule);

        strToVector(stayFocusedStr, content, lineCount, rule);
        strToVector(noInitialFocusStr, content, lineCount, rule);

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

        std::string winTypeStr, opacityStr, posStr, sizeStr, pinStr, noMaxSizeStr, stayFocusedStr, noInitialFocusStr;
        GetStrings(rule, winTypeStr, opacityStr, posStr, sizeStr, pinStr, noMaxSizeStr, stayFocusedStr,
                   noInitialFocusStr);

        std::vector<int> lineNum;

        appendToFile(winTypeStr, file, lineNum, lineCount);

        appendToFile(opacityStr, file, lineNum, lineCount);

        appendToFile(posStr, file, lineNum, lineCount);

        appendToFile(sizeStr, file, lineNum, lineCount);

        appendToFile(pinStr, file, lineNum, lineCount);

        appendToFile(noMaxSizeStr, file, lineNum, lineCount);

        appendToFile(stayFocusedStr, file, lineNum, lineCount);
        appendToFile(noInitialFocusStr, file, lineNum, lineCount);

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

void Saver::GetStrings(Rule* rule, std::string& winTypeStr, std::string& opacityStr, std::string& posStr,
                       std::string& sizeStr, std::string& pinStr, std::string& noMaxSizeStr,
                       std::string& stayFocusedStr, std::string& noInitialFocusStr)
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
        winTypeStr = ruleStart + "maximize," + ruleCondition;
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

    if (rule->noMaxSize)
        noMaxSizeStr = ruleStart + "nomaxsize," + ruleCondition;

    if (rule->stayFocused)
        stayFocusedStr = ruleStart + "stayfocused," + ruleCondition;

    if (rule->noInitialFocus)
        noInitialFocusStr = ruleStart + "noinitialfocus," + ruleCondition;
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
