#pragma once

#include <gtkmm/listboxrow.h>
#include <nlohmann/json.hpp>
#include <include/rule.h>
#include <vector>

class RuleRow : public Gtk::ListBoxRow {
public:
  Rule rule;
  nlohmann::basic_json<> clients;
};
