#include "include/updatehandlers.h"
#include "include/regextype.h"
#include "include/windowtype.h"
#include <format>
#include <string>

void HadleTwoFieldsUpdate(std::string keyword, std::string firstStr,
                          std::string secondStr) {
  config.SetEffectsString(keyword, firstStr + " " + secondStr);
}

void HandleCheckButtonUpdate(std::string keyword, bool isEnabled) {
  if (isEnabled)
    config.SetEffectsString(keyword, "");
  else
    config.RemoveEffect(keyword);
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
    config.SetPropsString(keyword, prop);
  } else {
    config.RemoveProp(keyword);
  }
}

void HandleWindowTypeUpdae(WindowType type) {
  switch (type) {
  case WindowType::floating:
    config.SetEffectsString("float", "");
    config.RemoveEffect("fullscreen");
    config.RemoveEffect("tile");
    config.RemoveEffect("maximize");
    break;
  case WindowType::fullscreen:
    config.SetEffectsString("fullscreen", "");
    config.RemoveEffect("float");
    config.RemoveEffect("tile");
    config.RemoveEffect("maximize");
    break;
  case WindowType::tile:
    config.SetEffectsString("tile", "");
    config.RemoveEffect("float");
    config.RemoveEffect("fullscreen");
    config.RemoveEffect("maximize");
    break;
  case WindowType::maximize:
    config.SetEffectsString("maximize", "");
    config.RemoveEffect("float");
    config.RemoveEffect("tile");
    config.RemoveEffect("fullscreen");
    break;
  case WindowType::none:
    config.RemoveEffect("maximize");
    config.RemoveEffect("float");
    config.RemoveEffect("tile");
    config.RemoveEffect("fullscreen");
  }
}

void HandleOpacityUpdate(Gtk::SpinButton *activeSB, Gtk::SpinButton *inactiveSB,
                         Gtk::SpinButton *fullscreenSB, Gtk::Scale *scale,
                         Gtk::SpinButton *current) {
  float active = activeSB->get_value() / 100;
  float inactive = inactiveSB->get_value() / 100;
  float fullscreen = fullscreenSB->get_value() / 100;
  if (active == inactive && inactive == fullscreen)
    config.SetEffectsString("opacity", std::to_string(active));
  else if (fullscreen == 1)
    config.SetEffectsString("opacity", std::format("{} {}", active, inactive));
  else
    config.SetEffectsString(
        "opacity", std::format("{} {} {}", active, inactive, fullscreen));

  scale->freeze_notify();
  scale->set_value(current->get_value_as_int());
  scale->thaw_notify();
}
