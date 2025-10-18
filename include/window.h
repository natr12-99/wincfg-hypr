#pragma once
#include "giomm/settings.h"
#include "gtkmm/box.h"
#include "gtkmm/button.h"
#include "gtkmm/checkbutton.h"
#include "gtkmm/entry.h"
#include "gtkmm/listbox.h"
#include "gtkmm/scale.h"
#include "gtkmm/spinbutton.h"
#include "gtkmm/window.h"
#include "include/regextype.h"
#include "gtkmm/dropdown.h"
#include "gtkmm/searchentry2.h"
#include "include/ruleconfig.h"
#include <string>
#include <vector>
struct Rule; 
class MainWindow : public Gtk::Window
{
  public:
    MainWindow();

  private:
    bool FilterRulesList(Gtk::ListBoxRow* row);
    void SelectConfigPath();
    void RefreshWindowsList();
    void RefreshRulesList();
    void SetRuleStrings(Gtk::Entry* t, Gtk::Entry* c);
    void SetOpacity(Gtk::SpinButton* active, Gtk::SpinButton* inactive, Gtk::Scale* scale, bool activeToScale);
    void SetSize();
    void SetPos();
    void InitRuleEditor();
    void OpenRuleEditor(std::string wTitle, std::string wClass, Gtk::Box* _prevBox, Rule* rule);
    void ResetRuleEditor();
    void LoadRule(std::string wTitle, RegexType rTitle, std::string wClass, RegexType rClass,
                  std::vector<int>& ruleLineNum);
    void DeleteRule(std::vector<int>& ruleLineNum);
    void FileErrorAlert();
    void SetModifyOpacity();
    void SetRegex();
    Gtk::Box windowSelectBox;
    Gtk::ListBox listClients;
    Gtk::Button refreshListButton;
    Gtk::Button newEmtryRuleButton;
    Gtk::Button goToRuleList;

    Gtk::Box ruleSelectBox;
    Gtk::ListBox listRules;
    Gtk::Button refreshRulesListButton;
    Gtk::Button selectConfigButton;
    Gtk::Button goToWindowsList;

    Gtk::Box* prevBox;
    Gtk::Box mainEditRuleBox;
    Gtk::Box opacityBox;
    Gtk::Entry titleEntry, classEntry, posXEntry, posYEntry, sizeXEntry, sizeYEntry;
    Gtk::CheckButton floating, fullscreen, tile, maximize, noType, modifyOpacity, pinned, noInitialFocusCB,
        stayFocusedCB, noMaxSizeCB;
    Gtk::SpinButton activeOpacity, inactiveOpacity;
    Gtk::Scale activeOpScale, inactiveOpScale;
    Gtk::Button exitFromEditing;
    Gtk::DropDown dropdownC, dropdownT;
    Gtk::SearchEntry2 rulesListSearch;

    RuleConfig config;
    std::string configPath;

    Glib::RefPtr<Gio::Settings> settings;
};
