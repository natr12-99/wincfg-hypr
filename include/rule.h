#pragma once

#include "regextype.h"
#include "include/windowtype.h"
#include "string"
#include "vector"

struct Rule
{
    std::string windowName;
    RegexType win = RegexType::nouse;

    std::string windowClass;
    RegexType cls = RegexType::nouse;

    WindowType winType = WindowType::none; 
    float opacityActive = -1;
    float opacityInactive = -1;

    std::string posX;
    std::string posY;

    std::string sizeX;
    std::string sizeY;

    std::vector<int> lineNum;
};
