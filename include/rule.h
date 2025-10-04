#pragma once

#include "regextype.h"
#include "string"
#include "vector"

struct Rule
{
    std::string windowName;
    RegexType win = RegexType::nouse;

    std::string windowClass;
    RegexType cls = RegexType::nouse;

    bool floating = false;
    float opacityActive = -1;
    float opacityInactive = -1;

    std::string posX;
    std::string posY;

    std::string sizeX;
    std::string sizeY;

    std::vector<int> lineNum;
};
