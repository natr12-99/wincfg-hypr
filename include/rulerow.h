#pragma once

#include <gtkmm/listboxrow.h>
#include <nlohmann/json.hpp>
#include <vector>

class RuleRow : public Gtk::ListBoxRow {
public:
  std::vector<int> ruleLineNum;
  nlohmann::basic_json<> clients;
};
