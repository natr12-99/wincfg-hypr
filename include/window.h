#pragma once
#include "giomm/settings.h"
#include "gtkmm/box.h"
#include "gtkmm/button.h"
#include "gtkmm/checkbutton.h"
#include "gtkmm/dropdown.h"
#include "gtkmm/entry.h"
#include "gtkmm/label.h"
#include "gtkmm/listbox.h"
#include "gtkmm/listboxrow.h"
#include "gtkmm/paned.h"
#include "gtkmm/scale.h"
#include "gtkmm/searchentry2.h"
#include "gtkmm/spinbutton.h"
#include "gtkmm/window.h"

#include <nlohmann/json_fwd.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class MainWindow : public Gtk::Window {
public:
  MainWindow();

private:
  bool FilterRulesList(Gtk::ListBoxRow *row);
  void SelectConfigPath();
  void RefreshWindowsList();
  void OnWindowSelected(Gtk::ListBoxRow *row);
  void RefreshRulesList();
  void OnRuleSelected(Gtk::ListBoxRow *row);

  void InitRuleEditor();
  void ResetRuleEditor();
  void LoadRule(std::string ruleString, std::vector<int> &ruleLineNum);
  void DeleteRule(Gtk::ListBoxRow *row);
  void FileErrorAlert();
  void ParseHyprClients(nlohmann::basic_json<> clients);

  Gtk::Paned layout;

  Gtk::Box windowSelectBox;
  Gtk::ListBox listClients;
  Gtk::Button refreshListButton;

  Gtk::Button goToRuleList;

  Gtk::Box ruleSelectBox;
  Gtk::ListBox listRules;
  Gtk::Button refreshRulesListButton;
  Gtk::Button selectConfigButton;
  Gtk::Button goToWindowsList;
  Gtk::Button deleteRule;
  Gtk::SearchEntry2 rulesListSearch;

  Gtk::Box mainEditRuleBox;
  Gtk::Entry posXEntry, posYEntry, sizeXEntry, sizeYEntry;
  Gtk::CheckButton floating, fullscreen, tile, pseudotile, maximize, noType;
  Gtk::SpinButton activeOpacity, inactiveOpacity, fullscreenOpacity;
  Gtk::Scale activeOpScale, inactiveOpScale, fullscreenOpScale;
  Gtk::Label savingStateNotify;

  std::unordered_map<std::string, Gtk::CheckButton *> checkButtons;
  std::unordered_map<std::string, Gtk::DropDown *> dropDowns;
  std::unordered_map<std::string, Gtk::DropDown *> regexDropDowns;
  std::unordered_map<std::string, Gtk::Entry *> regexEntrys;
  std::string configPath;

  Glib::RefPtr<Gio::Settings> settings;
};
