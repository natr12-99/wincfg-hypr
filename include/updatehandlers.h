#pragma once

#include "gtkmm/checkbutton.h"
#include "gtkmm/dropdown.h"
#include "gtkmm/entry.h"
#include "gtkmm/scale.h"
#include "gtkmm/spinbutton.h"
#include "include/windowtype.h"
#include <string>

void HandleTwoFieldsUpdate(std::string keyword, Gtk::Entry *firstEntry,
                           Gtk::Entry *secondEntry);

void HandleCheckButtonUpdate(std::string keyword, Gtk::CheckButton *cb);

void HandleOpacityUpdate(Gtk::SpinButton *active, Gtk::SpinButton *inactive,
                         Gtk::SpinButton *fullscreen, Gtk::Scale *scale,
                         Gtk::SpinButton *current);

void HandleRegExProps(std::string keyword, Gtk::Entry *entry,
                      Gtk::DropDown *dropdown);

void HandleWindowTypeUpdae(WindowType type);

void HandleDropDown(std::string keyword, Gtk::DropDown *dropdown);
