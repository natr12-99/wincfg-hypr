#include "include/loader.h"
#include "include/regextype.h"
#include "include/rule.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

bool Loader::Load(std::string path, std::vector<Rule> &rules) {
  using namespace std;
  ifstream file(path);
  if (!file.good())
    return false;

  string input;
  int fileLine = 0;
  while (getline(file, input)) {
    int type = isWindowRule(input);
    if (type == 0) {
      fileLine++;
      continue;
    } else if (type == 1) {

      int pos = 0;
      string dat;
      Rule rule;
      while (input.length() > 0) {
        if ((pos = input.find(',')) != string::npos) {
          dat = input.substr(0, pos);
          input = input.substr(pos + 1);
        } else {
          dat = input;
          input.erase();
        }
        stringstream ss(dat);
        string prop, args;
        ss >> prop;

        bool isMatch = false;
        if (prop.find("match:", 0) == 0) {
          isMatch = true;
          prop = prop.substr(6);
        }
        getline(ss, args);
        args.erase(0, 1);
        if (isMatch)
          rule.props[prop] = args;
        else
          rule.effects[prop] = args;
      }
      rules.push_back(rule);
      vector<int> vec{fileLine};
      rule.lineNum = vec;
      fileLine++;
    } else if (type == 2) {
      vector<int> lineNum;
      Rule rule;
      lineNum.push_back(fileLine);

      fileLine++;
      while (getline(file, input)) {
        if (input.find('}') == 0) {
          lineNum.push_back(fileLine);
          rule.lineNum = lineNum;
          fileLine++;
          break;
        }
        stringstream ss(input);   // отределай еще кнопки очищения где аргумент
                                  // массив из полей которые очистить
        std::string s, sec, last; // переименуй это
        ss >> s >> sec;
        getline(ss, last);
        if (sec == "=")
          cout << "хорошо,у нас =\n";
        if (s == "name") {
          rule.name = last;

        } else if (s.find("match:", 0) == 0) {
          rule.props[s.substr(6)] = last;
        } else {
          rule.props[s] = last;
        }
        fileLine++;
      }
      rules.push_back(rule);
    }
  }

  file.close();
  return true;
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

int Loader::isWindowRule(const std::string &line) {
  auto firstNonSpace = line.find_first_not_of(" \t");
  if (firstNonSpace == std::string::npos)
    return 0;

  if (line[firstNonSpace] == '#')
    return 0;

  if (line.find("windowrule", firstNonSpace) != std::string::npos) {
    if (line.find("=", firstNonSpace + 10) != std::string::npos)
      return 1;
    else if (line.find("{", firstNonSpace + 10) != std::string::npos)
      return 2;
    else
      return 0;
  }
}
