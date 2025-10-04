#pragma once

#include "string"
#include "vector"
#include <vector>
class Rule;

class Saver
{
  public:
    bool SaveStruct(Rule* rule, std::string path);
    bool DeleteRule(std::vector<int>& ruleLineNum, std::string path);

  private:
    bool DeleteStrings(std::vector<int>& ruleLineNum, std::vector<std::string>& content, std::string path);
    void GetStrings(Rule* rule, std::string& floatStr, std::string& opacityStr, std::string& posStr,
                    std::string& sizeStr);
    std::string ToString(float value);
};
