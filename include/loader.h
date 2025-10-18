#pragma once

#include "include/regextype.h"
#include "include/windowtype.h"
#include "string"
#include "vector"
class Loader
{
  public:
    bool LoadOnlyNames(std::vector<std::string>& winNames, std::vector<RegexType>& RNames,
                       std::vector<std::string>& winClasses, std::vector<RegexType>& RClasses,
                       std::vector<std::vector<int>>& lineNum, std::string path);

    bool LoadFull(std::vector<int>& ruleLineNum, WindowType& winType, int& opacityActive, int& opacityInactive,
                  std::string& posX, std::string& posY, std::string& sizeX, std::string& sizeY, bool& isPinned,
                  bool& noMaxSize, bool& stayFocused, bool& noInitialFocus, std::string path);

  private:
    bool FindMatches(std::vector<std::string>& vec, std::vector<std::string>& vec2, std::vector<RegexType>& type1,
                     std::vector<RegexType>& type2, std::string str, std::string str2, RegexType tp1, RegexType tp2,
                     int& index);

    RegexType GetRType(std::string& input);
};
