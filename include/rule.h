#pragma once

#include "include/windowtype.h"
#include "regextype.h"
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

    bool isPinned;

    bool noInitialFocus;
    bool stayFocused;
    bool noMaxSize;

    std::vector<int> lineNum;
};
