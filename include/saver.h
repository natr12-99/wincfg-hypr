#pragma once
#include "fstream"
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
    void GetStrings(Rule* rule, std::vector<std::string>& strings);
    std::string ToString(float value);
    void strToVector(std::string& str, std::vector<std::string>& content, int& lineCount, Rule* rule);
    void appendToFile(std::string& str, std::ofstream& file, std::vector<int> lineNum, int& lineCount);
};
