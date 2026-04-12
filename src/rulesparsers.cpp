#include "include/rulesparsers.h"
#include "gtkmm/dropdown.h"
#include "include/regextype.h"
#include <locale>
#include <sstream>

RegexType GetRType(std::string &input) {
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

void ParseRegExProps(std::string input, Gtk::Entry *entry,
                     Gtk::DropDown *dropdown) {
  RegexType type = GetRType(input);
  dropdown->set_selected(static_cast<int>(type));
  entry->set_text(input);
}

void ParseTwoFields(std::string input, Gtk::Entry *e1, Gtk::Entry *e2) {
  std::istringstream ss(input);
  std::string a, b;
  ss >> a >> b;
  e1->set_text(a);
  e2->set_text(b);
}

void ParseOpacity(std::string input, Gtk::SpinButton *active,
                  Gtk::SpinButton *inactive, Gtk::SpinButton *fullscreen) {
  float a = -1, i = -1, f = -1; // добавить сюда установку слайдеров
  std::stringstream ss(input);
  ss.imbue(std::locale::classic());
  ss >> a >> i >> f;
  if (i == -1 && f == -1) {
    active->set_value(a * 100);
    inactive->set_value(a * 100);
    fullscreen->set_value(a * 100);
  } else {
    active->set_value(a * 100);
    inactive->set_value(i * 100);
    if (f != -1)
      fullscreen->set_value(f * 100);
  }
}

void ParseDropDown(std::string input, Gtk::DropDown *dd) {
  if (input == "1" || input == "on" || input == "true") {
    dd->set_selected(1);
  } else {
    dd->set_selected(2);
  }
}
