#include "include/saver.h"

#include "include/rule.h"
#include <format>
#include <fstream>
#include <string>
#include <vector>

int Saver::SaveStruct(Rule *rule, std::string path) {
  if (rule->effects.size() == 0 && rule->props.size() == 0)
    return 2;
  setlocale(LC_ALL, "C");

  if (!rule->lineNum.empty()) {
    std::vector<std::string> content;
    if (!DeleteStrings(rule->lineNum, content, path))
      return false;
    std::vector<std::string> lines;
    GetStrings(rule, lines);
    int lineCount = rule->lineNum.front();
    rule->lineNum.clear();

    strToVector(lines, content, lineCount, rule);

    std::ofstream outfile(path);
    for (auto str : content) {
      outfile << str << std::endl;
    }
    outfile.close();
  } else {
    int lineCount = 0;
    std::ifstream lineCounter(path);
    if (!lineCounter.good())
      return 1;
    std::string uselessStr;
    while (std::getline(lineCounter, uselessStr))
      lineCount++;
    lineCounter.close();

    std::ofstream file(path, std::ios::app);
    std::vector<std::string> strings;
    std::vector<std::string> lines;
    GetStrings(rule, lines);

    std::vector<int> lineNum;

    appendToFile(lines, file, lineNum, lineCount);

    rule->lineNum = lineNum;

    file.close();
  }
  return 0;
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

  if (ruleLineNum.size() == 1)
    content.erase(content.begin() + ruleLineNum[0]);
  else if (ruleLineNum.size() == 2)
    content.erase(content.begin() + ruleLineNum[0],
                  content.begin() + ruleLineNum[1] + 1);

  return true;
}

void Saver::GetStrings(Rule *rule, std::vector<std::string> &lines) {
  if (rule->name.empty()) {
    std::string ruleProps, ruleEffects;
    std::string ruleStart = "windowrule = ";

    for (auto i : rule->props) {
      ruleProps += std::format("match:{} {},", i.first, i.second);
    }
    for (auto i : rule->effects) {
      ruleEffects += std::format("{} {},", i.first, i.second);
    }
    std::string s = ruleStart + ruleProps + ruleEffects;
    s.pop_back(); // это что?
    lines.push_back(s);
  } else {
    std::string ruleProps, ruleEffects;
    lines.push_back("windowrule {");
    lines.push_back(std::format("\tname = {}", rule->name));
    for (auto i : rule->props) {
      lines.push_back(std::format("\tmatch:{} = {}", i.first, i.second));
    }
    for (auto i : rule->effects) {
      lines.push_back(std::format("\t{} = {}", i.first, i.second));
    }
    lines.push_back("}");
  }
}

void Saver::strToVector(std::vector<std::string> &strings,
                        std::vector<std::string> &content, int &lineCount,
                        Rule *rule) {
  if (lineCount > content.size())
    lineCount = content.size();
  for (int i = 0; i < strings.size(); i++) {
    content.insert(content.begin() + lineCount, strings[i]);
    if (i == 0)
      rule->lineNum.push_back(lineCount);
    lineCount++;
  }
  if (strings.size() > 1) {
    rule->lineNum.push_back(lineCount - 1);
  }
}

void Saver::appendToFile(std::vector<std::string> &lines, std::ofstream &file,
                         std::vector<int> &lineNum, int &lineCount) {
  for (int i = 0; i < lines.size(); i++) {
    file << lines[i] << std::endl;
    if (i == 0)
      lineNum.push_back(lineCount);
    lineCount++;
  }
  if (lines.size() > 1) {
    lineNum.push_back(lineCount);
  }
}
