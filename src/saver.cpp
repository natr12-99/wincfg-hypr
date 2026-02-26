#include "include/saver.h"

#include "include/rule.h"
#include <algorithm>
#include <format>
#include <fstream>
#include <string>
#include <vector>

bool Saver::SaveStruct(Rule *rule, std::string path) {
  setlocale(LC_ALL, "C");

  if (!rule->lineNum.empty()) {
    std::vector<std::string> content;
    if (!DeleteStrings(rule->lineNum, content, path))
      return false;

    std::string line = GetStrings(rule);
    int lineCount = rule->lineNum.back();
    rule->lineNum.clear();

    strToVector(line, content, lineCount, rule);

    std::ofstream outfile(path);
    for (auto str : content) {
      outfile << str << std::endl;
    }
    outfile.close();
  } else {
    int lineCount = 0;
    std::ifstream lineCounter(path);
    if (!lineCounter.good())
      return false;
    std::string uselessStr;
    while (std::getline(lineCounter, uselessStr))
      lineCount++;
    lineCounter.close();

    std::ofstream file(path, std::ios::app);
    std::vector<std::string> strings;
    std::string line = GetStrings(rule);

    std::vector<int> lineNum;

    appendToFile(line, file, lineNum, lineCount);

    rule->lineNum = lineNum;

    file.close();
  }
  return true;
}

bool Saver::DeleteRule(std::vector<int> &ruleLineNum, std::string path) {
  std::vector<std::string> content;
  if (!DeleteStrings(ruleLineNum, content, path))
    return false;
  std::ofstream outfile(path);
  for (auto str : content) {
    outfile << str << std::endl;
  }
  outfile.close();
  return true;
}

bool Saver::DeleteStrings(std::vector<int> &ruleLineNum,
                          std::vector<std::string> &content, std::string path) {
  std::ifstream infile(path);
  if (!infile.good())
    return false;
  std::string tmp;
  while (std::getline(infile, tmp))
    content.push_back(tmp);
  infile.close();

  std::sort(ruleLineNum.rbegin(), ruleLineNum.rend());
  for (int index : ruleLineNum) {
    if (index >= 0 && index < content.size()) {
      content.erase(content.begin() + index);
    }
  }
  return true;
}

std::string Saver::GetStrings(Rule *rule) {
  std::string ruleProps, ruleEffects;
  std::string ruleStart = "windowrule = ";

  for (auto i : rule->props) {
    ruleProps += std::format("match:{} {},", i.first, i.second);
  }
  for (auto i : rule->effects) {
    ruleEffects += std::format("{} {},", i.first, i.second);
  }
  std::string s = ruleStart + ruleProps + ruleEffects;
  s.pop_back();
  return s;
}

void Saver::strToVector(std::string &str, std::vector<std::string> &content,
                        int &lineCount, Rule *rule) {
  if (!str.empty()) {
    content.insert(content.begin() + lineCount, str);
    rule->lineNum.push_back(lineCount);
    lineCount++;
  }
}

void Saver::appendToFile(std::string &str, std::ofstream &file,
                         std::vector<int> &lineNum, int &lineCount) {
  if (!str.empty()) {
    file << str << std::endl;
    lineNum.push_back(lineCount);
    lineCount++;
  }
}
