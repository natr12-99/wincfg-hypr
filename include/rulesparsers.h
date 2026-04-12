#pragma once

#include "gtkmm/dropdown.h"
#include "gtkmm/entry.h"
#include "gtkmm/scale.h"
#include "gtkmm/spinbutton.h"
#include "include/regextype.h"
#include <string>

RegexType GetRType(std::string &input);

void ParseRegExProps(std::string input, Gtk::Entry *entry,
                     Gtk::DropDown *dropdown);

void ParseTwoFields(std::string input, Gtk::Entry *e1, Gtk::Entry *e2);
void ParseOpacity(std::string input, Gtk::SpinButton *active,
                  Gtk::Scale *activeS, Gtk::SpinButton *inactive,
                  Gtk::Scale *inactiveS, Gtk::SpinButton *fullscreen,
                  Gtk::Scale *fullscreenS);
void ParseDropDown(std::string input, Gtk::DropDown *dd);
