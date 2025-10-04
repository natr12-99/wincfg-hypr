#include "include/loader.h"

#include <fstream>
#include <regex>

bool Loader::LoadOnlyNames(std::vector<std::string>& winNames, std::vector<RegexType>& RNames,
                           std::vector<std::string>& winClasses, std::vector<RegexType>& RClasses,
                           std::vector<std::vector<int>>& lineNum, std::string path)
{
    using namespace std;
    ifstream file(path);
    if (!file.good())
        return false;

    string input;
    int fileLine = 0;
    while (getline(file, input))
    {
        regex pattern(R"(\s*([^,]+)\s*,\s*([^:]+)\s*:\s*([^,]+)(?:\s*,\s*([^:]+)\s*:\s*([^,]+))?)");
        smatch matches;
        if (regex_search(input, matches, pattern))
        {
            if (matches[1].str()[0] == '#')
            {
                fileLine++;
                continue;
            }

            if (matches[1].str().find("windowrule") == string::npos)
            {
                fileLine++;
                continue;
            }

            string Wtitle = "";
            string Wclass = "";
            RegexType RName = RegexType::nouse;
            RegexType RClass = RegexType::nouse;

            for (int i = 1; i < matches.size(); i++)
            {
                if (matches[i].str().compare("class") == 0)
                {
                    Wclass = matches[i + 1];
                    RClass = GetRType(Wclass);
                }
                else if (matches[i].str().compare("title") == 0)
                {
                    Wtitle = matches[i + 1];
                    RName = GetRType(Wtitle);
                }
            }
            int index = 0;
            if (!FindMatches(winNames, winClasses, RNames, RClasses, Wtitle, Wclass, RName, RClass, index))
            {
                winNames.push_back(Wtitle);
                winClasses.push_back(Wclass);

                RClasses.push_back(RClass);
                RNames.push_back(RName);
                // ноый
                vector<int> vec{fileLine};
                lineNum.push_back(vec);
            }
            else
            {

                lineNum.at(index).push_back(fileLine);
            }
        }
        fileLine++;
    }
    file.close();
    return true;
}

bool Loader::LoadFull(std::vector<int>& ruleLineNum, bool& isFloating, int& opacityActive, int& opacityInactive,
                      std::string& posX, std::string& posY, std::string& sizeX, std::string& sizeY, std::string path)
{
    using namespace std;
    setlocale(LC_ALL, "C");

    ifstream file(path);
    if (!file.good())
        return false;
    string input;
    int lineNum = 0;
    int vecIndex = 0;

    while (getline(file, input) && vecIndex < ruleLineNum.size())
    {
        if (lineNum == ruleLineNum.at(vecIndex))
        {
            int pos = input.find(',');
            string str = input.substr(0, pos);

            regex pattern(R"(\s*([^=]+)\s*=\s*(\w+)(?:\s+([\d.]+))?(?:\s+([\d.]+))?\s*)");
            smatch matches;
            if (regex_search(str, matches, pattern))
            {
                if (matches[2].compare("float") == 0)
                    isFloating = true;
                else if (matches[2].compare("opacity") == 0)
                {
                    opacityActive = stof(matches[3]) * 100;
                    if (matches[4].matched)
                        opacityInactive = stof(matches[4]) * 100;
                }
                else if (matches[2].compare("move") == 0)
                {
                    posX = matches[3];
                    if (matches[4].matched)
                        posY = matches[4];
                }
                else if (matches[2].compare("size") == 0)
                {
                    sizeX = matches[3];
                    if (matches[4].matched)
                        sizeY = matches[4];
                }
            }

            vecIndex++;
        }
        lineNum++;
    }
    return true;
}

bool Loader::FindMatches(std::vector<std::string>& vec, std::vector<std::string>& vec2, std::vector<RegexType>& type1,
                         std::vector<RegexType>& type2, std::string str, std::string str2, RegexType tp1, RegexType tp2,
                         int& index)
{
    for (std::string st : vec)
    {
        if ((st.compare(str) == 0) && (vec2.at(index).compare(str2) == 0) && type1.at(index) == tp1 &&
            type2.at(index) == tp2)
            return true;
        index++;
    }
    return false;
}

RegexType Loader::GetRType(std::string& input)
{
    if (input.compare(0, 1, "^") == 0 && input.compare(input.length() - 1, 1, "$") == 0)
    {
        input = input.substr(1, input.length() - 2);
        return RegexType::fullMatch;
    }
    else if (input.compare(0, 2, ".*") == 0 && input.compare(input.length() - 2, 2, ".*") == 0)
    {
        input = input.substr(2, input.length() - 4);
        return RegexType::contain;
    }
    else
    {
        return RegexType::fullMatch;
    }
}
