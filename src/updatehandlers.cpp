#include "include/updatehandlers.h"
#include "gtkmm/entry.h"
#include "include/regextype.h"
#include "include/ruleconfig.h"
#include "include/window.h"
#include "include/windowtype.h"
#include <format>
#include <string>

void HandleNameUpdate(Gtk::Entry *nameEntry) {
  if (!MainWindow::blockUpdateHandle)
    RuleConfig::SetName(nameEntry->get_text());
}

void HandleTwoFieldsUpdate(std::string keyword, Gtk::Entry *firstEntry,
                           Gtk::Entry *secondEntry) {
  if (MainWindow::blockUpdateHandle)
    return;
  if (firstEntry->get_text().empty() && secondEntry->get_text().empty())
    RuleConfig::RemoveEffect(keyword);
  else
    RuleConfig::SetEffectsString(keyword, firstEntry->get_text() + " " +
                                              secondEntry->get_text());
}

void HandleCheckButtonUpdate(std::string keyword, Gtk::CheckButton *cb) {
  if (MainWindow::blockUpdateHandle)
    return;
  if (cb->get_active()) {
    if (keyword != "decorate")
      RuleConfig::SetEffectsString(keyword, "1");
    else
      RuleConfig::SetEffectsString(keyword, "0");
  } else
    RuleConfig::RemoveEffect(keyword);
}

void HandleRegExProps(std::string keyword, Gtk::Entry *entry,
                      Gtk::DropDown *dropdown) {
  if (MainWindow::blockUpdateHandle)
    return;
  std::string data = entry->get_text();
  RegexType type = static_cast<RegexType>(dropdown->get_selected());
  if (type != RegexType::nouse) {
    std::string prop;
    switch (type) {
    case RegexType::fullMatch:
      prop = "^" + data + "$";
      break;
    case RegexType::contain:
      prop = ".*" + data + ".*";
      break;
    case RegexType::containLeft:
      prop = ".*" + data;
      break;
    case RegexType::containRight:
      prop = "" + data + ".*";
      break;
    }
    RuleConfig::SetPropsString(keyword, prop);
  } else {
    RuleConfig::RemoveProp(keyword);
  }
}

void HandleWindowTypeUpdate(WindowType type) {
  if (MainWindow::blockUpdateHandle)
    return;
  switch (type) {
  case WindowType::floating:
    RuleConfig::SetEffectsString("float", "1");
    RuleConfig::RemoveEffect("fullscreen");
    RuleConfig::RemoveEffect("tile");
    RuleConfig::RemoveEffect("maximize");
    RuleConfig::RemoveEffect("pseudo");
    break;
  case WindowType::fullscreen:
    RuleConfig::SetEffectsString("fullscreen", "1");
    RuleConfig::RemoveEffect("float");
    RuleConfig::RemoveEffect("tile");
    RuleConfig::RemoveEffect("maximize");
    RuleConfig::RemoveEffect("pseudo");
    break;
  case WindowType::tile:
    RuleConfig::SetEffectsString("tile", "1");
    RuleConfig::RemoveEffect("float");
    RuleConfig::RemoveEffect("fullscreen");
    RuleConfig::RemoveEffect("maximize");
    RuleConfig::RemoveEffect("pseudo");
    break;
  case WindowType::pseudotile:
    RuleConfig::SetEffectsString("pseudo", "1");
    RuleConfig::RemoveEffect("float");
    RuleConfig::RemoveEffect("fullscreen");
    RuleConfig::RemoveEffect("maximize");
    break;
  case WindowType::maximize:
    RuleConfig::SetEffectsString("maximize", "1");
    RuleConfig::RemoveEffect("float");
    RuleConfig::RemoveEffect("tile");
    RuleConfig::RemoveEffect("fullscreen");
    RuleConfig::RemoveEffect("pseudo");
    break;
  case WindowType::none:
    RuleConfig::RemoveEffect("maximize");
    RuleConfig::RemoveEffect("float");
    RuleConfig::RemoveEffect("tile");
    RuleConfig::RemoveEffect("fullscreen");
    RuleConfig::RemoveEffect("pseudo");
  }
}

void HandleOpacityUpdate(Gtk::SpinButton *activeSB, Gtk::SpinButton *inactiveSB,
                         Gtk::SpinButton *fullscreenSB, Gtk::Scale *scale,
                         Gtk::SpinButton *current) {
  if (MainWindow::blockUpdateHandle)
    return;
  float active = activeSB->get_value() / 100;
  float inactive = inactiveSB->get_value() / 100;
  float fullscreen = fullscreenSB->get_value() / 100;
  if (active == inactive && inactive == fullscreen)
    RuleConfig::SetEffectsString("opacity", std::format("{}", active));
  else if (fullscreen == 1)
    RuleConfig::SetEffectsString("opacity",
                                 std::format("{} {}", active, inactive));
  else
    RuleConfig::SetEffectsString(
        "opacity", std::format("{} {} {}", active, inactive, fullscreen));

  scale->freeze_notify();
  scale->set_value(current->get_value_as_int());
  scale->thaw_notify();
}

void HandleDropDown(std::string keyword, Gtk::DropDown *dropdown) {
  if (MainWindow::blockUpdateHandle)
    return;
  auto selected = dropdown->get_selected();
  if (selected == 0)
    RuleConfig::RemoveProp(keyword);
  else if (selected == 1)
    RuleConfig::SetPropsString(keyword, "1");
  else if (selected == 2)
    RuleConfig::SetPropsString(keyword, "0");
}
