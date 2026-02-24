#pragma once

#include <string>

struct SimlleRule {
  std::string name;
  std::string keyword;
  std::string tooltipText;
};

SimlleRule simpleRules[] = {
    {.name = "No inital focus", .keyword = "nofocus", .tooltipText = "всегда"},
    {.name = "Всегда топ",
     .keyword = "alwaystop",
     .tooltipText = "всегда сверху"}};
