#include "include/updatehandlers.h"
#include "gtkmm/entry.h"
#include "include/regextype.h"
#include "include/ruleconfig.h"
#include "include/windowtype.h"
#include <format>
#include <string>

void HandleTwoFieldsUpdate(std::string keyword, Gtk::Entry *firstEntry,
                           Gtk::Entry *secondEntry) {
  RuleConfig::SetEffectsString(keyword, firstEntry->get_text() + " " +
                                            secondEntry->get_text());
}

void HandleCheckButtonUpdate(std::string keyword, Gtk::CheckButton *cb) {
  if (cb->get_active())
    RuleConfig::SetEffectsString(keyword, "1");
  else
    RuleConfig::RemoveEffect(keyword);
}

void HandleRegExProps(std::string keyword, Gtk::Entry *entry,
                      Gtk::DropDown *dropdown) {
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

void HandleWindowTypeUpdae(WindowType type) {
  switch (type) {
  case WindowType::floating:
    RuleConfig::SetEffectsString("float", "1");
    RuleConfig::RemoveEffect("fullscreen");
    RuleConfig::RemoveEffect("tile");
    RuleConfig::RemoveEffect("maximize");
    break;
  case WindowType::fullscreen:
    RuleConfig::SetEffectsString("fullscreen", "1");
    RuleConfig::RemoveEffect("float");
    RuleConfig::RemoveEffect("tile");
    RuleConfig::RemoveEffect("maximize");
    break;
  case WindowType::tile:
    RuleConfig::SetEffectsString("tile", "1");
    RuleConfig::RemoveEffect("float");
    RuleConfig::RemoveEffect("fullscreen");
    RuleConfig::RemoveEffect("maximize");
    break;
  case WindowType::maximize:
    RuleConfig::SetEffectsString("maximize", "1");
    RuleConfig::RemoveEffect("float");
    RuleConfig::RemoveEffect("tile");
    RuleConfig::RemoveEffect("fullscreen");
    break;
  case WindowType::none:
    RuleConfig::RemoveEffect("maximize");
    RuleConfig::RemoveEffect("float");
    RuleConfig::RemoveEffect("tile");
    RuleConfig::RemoveEffect("fullscreen");
  }
}

void HandleOpacityUpdate(Gtk::SpinButton *activeSB, Gtk::SpinButton *inactiveSB,
                         Gtk::SpinButton *fullscreenSB, Gtk::Scale *scale,
                         Gtk::SpinButton *current) {
  float active = activeSB->get_value() / 100;
  float inactive = inactiveSB->get_value() / 100;
  float fullscreen = fullscreenSB->get_value() / 100;
  if (active == inactive && inactive == fullscreen)
    RuleConfig::SetEffectsString("opacity", std::to_string(active));
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
