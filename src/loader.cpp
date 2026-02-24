#include "include/loader.h"
#include "include/regextype.h"
#include "include/rule.h"
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

bool Loader::LoadOnlyProps(std::vector<std::string> &winProps,
                           std::vector<std::string> &rulesStrings,
                           std::vector<std::vector<int>> &lineNum,
                           std::string path) {
  using namespace std;
  ifstream file(path);
  if (!file.good())
    return false;

  string input;
  int fileLine = 0;
  while (getline(file, input)) {
    if (!isWindowRule(input)) {
      fileLine++;
      continue;
    }

    string matches;
    std::regex matchRegex(R"(match:([^,]+))");

    for (std::sregex_iterator it(input.begin(), input.end(), matchRegex), end;
         it != end; ++it) {
      matches += (*it)[1].str();
    }
    winProps.push_back(matches);
    auto t = input.substr(input.find('=') + 1);

    rulesStrings.push_back(t.substr(t.find_first_not_of(' ')));
    vector<int> vec{fileLine};
    lineNum.push_back(vec);
    fileLine++;
  }

  file.close();
  return true;
}

void Loader::LoadFull(std::string &input, Rule *rule) {
  using namespace std;

  int pos = input.find(',');
  string str = input.substr(0, pos);

  bool isMatch = false;
  if (input.find("match:", 0) == 0) {
    isMatch = true;
    input = input.substr(6);
  }
  std::stringstream ss(input);
  std::string prop, args;
  ss >> prop;
  std::getline(ss, args);
  if (isMatch)
    rule->props[prop] = args;
  else
    rule->effects[prop] = args;
}

RegexType Loader::GetRType(std::string &input) {
  if (input.starts_with("^") && input.ends_with("$")) {
    input = input.substr(1, input.length() - 2);
    return RegexType::fullMatch;
  } else if (input.starts_with(".*")) {
    input = input.substr(2);

    if (input.ends_with(".*")) {
      input = input.substr(0, input.length() - 2);
      return RegexType::contain;
    } else
      return RegexType::containLeft;
  } else if (input.ends_with(".*")) {
    input = input.substr(0, input.length() - 2);
    return RegexType::containRight;
  } else {
    return RegexType::fullMatch;
  }
}

bool Loader::isWindowRule(const std::string &line) {
  auto firstNonSpace = line.find_first_not_of(" \t");
  if (firstNonSpace == std::string::npos)
    return false;

  if (line[firstNonSpace] == '#')
    return false;

  return line.find("windowrule =", firstNonSpace) != std::string::npos;
}
