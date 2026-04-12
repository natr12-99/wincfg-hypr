#include "include/loader.h"
#include "include/rule.h"
#include <cstddef>
#include <fstream>

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
    int type = isWindowRule(input); //
    if (type == 0) {
      fileLine++;
      continue;
    } else if (type == 1) {
      std::size_t pos = 0;
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
        dat = dat.substr(dat.find('=') + 1);
        stringstream ss(dat);
        string prop, args;
        
        ss >> prop;

        bool isMatch = false;
        if (prop.starts_with("match:")) {
          isMatch = true;
          prop = prop.substr(6);
        }
        getline(ss, args);

        if (isMatch)
          rule.props[prop] = Trim(args);
        else
          rule.effects[prop] = Trim(args);
      }
      vector<int> vec{fileLine};
      rule.lineNum = vec;
      rules.push_back(rule);
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
        auto pos = input.find("=");
        if (pos != string::npos) {

          string first = input.substr(0, pos);
          string last = input.substr(pos + 1);
          first = Trim(first);
          last = Trim(last);
          if (first == "name") {
            rule.name = Trim(last);

          } else if (first.starts_with("match:")) {
            rule.props[first.substr(6)] = Trim(last);
          } else
            rule.effects[first] = Trim(last);
        }
        fileLine++;
      }
      rules.push_back(rule);
    }
  }

  file.close();
  return true;
}

std::string Loader::Trim(std::string &input) {
  auto first = input.find_first_not_of(" \t");
  auto last = input.find_last_not_of(" \t");
  return input.substr(first, last - first + 1);
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
  } else
    return 0;
}
