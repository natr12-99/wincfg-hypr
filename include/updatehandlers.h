#include "gtkmm/dropdown.h"
#include "gtkmm/entry.h"
#include "gtkmm/scale.h"
#include "gtkmm/spinbutton.h"
#include "include/ruleconfig.h"
#include "include/windowtype.h"
#include <string>

RuleConfig config;

void HadleTwoFieldsUpdate(std::string keyword, std::string firstStr,
                          std::string secondStr);

void HandleCheckButtonUpdate(std::string keyword, bool isEnabled);

void HandleOpacityUpdate(Gtk::SpinButton *active, Gtk::SpinButton *inactive,
                         Gtk::SpinButton *fullscreen, Gtk::Scale *scale,
                         Gtk::SpinButton *current);

void HandleRegExProps(std::string keyword, Gtk::Entry *entry,
                      Gtk::DropDown *dropdown);

void HandleWindowTypeUpdae(WindowType type);
