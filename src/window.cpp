#include "include/window.h"

#include "glibmm/main.h"
#include "gtkmm/alertdialog.h"
#include "gtkmm/button.h"
#include "gtkmm/checkbutton.h"
#include "gtkmm/dropdown.h"
#include "gtkmm/entry.h"
#include "gtkmm/enums.h"
#include "gtkmm/filedialog.h"
#include "gtkmm/label.h"
#include "gtkmm/listboxrow.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/separator.h"
#include "gtkmm/stringlist.h"
#include "include/firstlaunch.h"
#include "include/func.h"
#include "include/loader.h"
#include "include/rule.h"
#include "include/ruleconfig.h"
#include "include/rulerow.h"
#include "include/rulesparsers.h"
#include "include/saver.h"
#include "include/simplerule.h"
#include "include/updatehandlers.h"
#include "include/windowtype.h"
#include "sigc++/functors/mem_fun.h"
#include <format>
#include <nlohmann/json.hpp>
#include <string>

using namespace Gtk;

MainWindow::MainWindow() {
  set_title("Hyprland rules editor");
  InitRuleEditor();
  set_default_size(600, 600);
  refreshListButton.set_icon_name("view-refresh-symbolic");
  refreshListButton.set_tooltip_text("Refresh");
  refreshListButton.set_margin(5);
  windowSelectBox.set_orientation(Orientation::VERTICAL);
  refreshListButton.signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::RefreshWindowsList));
  goToRuleList.signal_clicked().connect(
      [this]() { layout.set_start_child(ruleSelectBox); });
  goToRuleList.set_icon_name("go-previous-symbolic");
  goToRuleList.set_tooltip_text("Back to rules list");
  goToRuleList.set_margin(5);

  Box windowListTopBox;
  windowListTopBox.set_hexpand(true);
  Label windowSelectL;
  windowSelectL.set_hexpand(true);
  windowSelectL.set_markup("<b>Window list</b>");
  windowListTopBox.append(windowSelectL);
  windowListTopBox.append(refreshListButton);
  windowListTopBox.append(goToRuleList);

  windowSelectBox.append(windowListTopBox);

  ScrolledWindow swClients;
  swClients.set_vexpand(true);
  swClients.set_child(listClients);
  listClients.signal_row_selected().connect(
      sigc::mem_fun(*this, &MainWindow::OnWindowSelected));
  windowSelectBox.append(swClients);
  // вот здесь это начинается
  refreshRulesListButton.set_icon_name("view-refresh-symbolic");
  refreshRulesListButton.set_tooltip_text("Refresh");
  refreshRulesListButton.set_margin(5);
  refreshRulesListButton.set_vexpand(false);
  ruleSelectBox.set_orientation(Orientation::VERTICAL);
  refreshRulesListButton.signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::RefreshRulesList));
  selectConfigButton.set_icon_name("document-edit-symbolic");
  selectConfigButton.set_tooltip_text("Select config file");
  selectConfigButton.set_margin(5);
  selectConfigButton.signal_clicked().connect(
      sigc::mem_fun(*this, &MainWindow::SelectConfigPath));
  goToWindowsList.signal_clicked().connect([this]() {
    RefreshWindowsList();
    layout.set_start_child(windowSelectBox);
  });
  goToWindowsList.set_icon_name("document-new-symbolic");
  goToWindowsList.set_tooltip_text("Show windows list (hyprctl clients)");
  goToWindowsList.set_margin(5);
  listRules.set_margin(5);

  deleteRule.set_icon_name("user-trash-symbolic");
  deleteRule.set_tooltip_text("Delete selected rule");
  deleteRule.set_margin(5);
  deleteRule.signal_clicked().connect([this]() {
    DeleteRule(listRules.get_selected_row());
    RefreshWindowsList();
  });
  Box rulesListTopBox;
  rulesListTopBox.append(selectConfigButton);
  rulesListTopBox.set_hexpand(true);
  Label ruleSelectL;
  ruleSelectL.set_hexpand(true);
  ruleSelectL.set_markup("<b>Select rule</b>");
  rulesListTopBox.append(ruleSelectL);
  rulesListTopBox.append(deleteRule);
  rulesListTopBox.append(refreshRulesListButton);
  rulesListTopBox.append(goToWindowsList);
  ruleSelectBox.append(rulesListTopBox);

  rulesListSearch.set_margin(5);
  rulesListSearch.set_placeholder_text("Search rule");
  rulesListSearch.signal_search_changed().connect(
      [this]() { listRules.invalidate_filter(); });
  listRules.set_filter_func(sigc::mem_fun(*this, &MainWindow::FilterRulesList));
  ruleSelectBox.append(rulesListSearch);
  ScrolledWindow swRules;
  swRules.set_child(listRules);
  swRules.set_vexpand(true);
  ruleSelectBox.append(swRules);
  listRules.signal_row_selected().connect(
      sigc::mem_fun(*this, &MainWindow::OnRuleSelected));

  set_child(layout);
  layout.set_start_child(ruleSelectBox);
  layout.set_end_child(mainEditRuleBox);
  layout.set_position(301);

  configPath = g_get_user_config_dir();
  configPath += "/hypr/windowrules.conf";

  settings = Gio::Settings::create("org.natr12.wincfg-hypr");
  if (settings->get_boolean("first-launch")) {
    FirstLaunch(this, settings, configPath);
  } else {
    configPath = settings->get_string("config-path");
    RefreshRulesList();
  }
  RuleConfig::InitRule(new Rule());
}

bool MainWindow::FilterRulesList(Gtk::ListBoxRow *row) {
  std::string searchT = rulesListSearch.get_text();
  if (searchT.empty())
    return true;

  auto child = dynamic_cast<Gtk::Box *>(row->get_child());
  auto child2 = dynamic_cast<Gtk::Button *>(child->get_first_child());

  std::string textL = Glib::ustring(child2->get_label()).lowercase();
  std::string searchL = Glib::ustring(searchT).lowercase();
  return textL.find(searchL) != std::string::npos;
}

void MainWindow::RefreshWindowsList() {
  while (auto child = listClients.get_first_child()) {
    listClients.remove(*child);
  }
  std::string clientsStr;
  clientsStr = exec("hyprctl clients -j");
  nlohmann::json clients;
  try {
    clients = nlohmann::json::parse(clientsStr);
  } catch (const nlohmann::json::parse_error &e) {
    auto dialog = AlertDialog::create(
        "Error while execution 'hyprctl clients -j':\n" + clientsStr);
    dialog->show();
    return;
  }

  for (auto item : clients) {
    auto row = make_managed<RuleRow>();
    Label label(std::format("class: {}\ntitle {}",
                            item["class"].get<std::string>(),
                            item["title"].get<std::string>()));
    row->set_child(label);
    label.set_justify(Gtk::Justification::CENTER);
    row->clients = item;

    listClients.append(*row);
  }
}

void MainWindow::OnWindowSelected(Gtk::ListBoxRow *row) {
  if (row) {
    RuleConfig::InitRule(new Rule);
    auto ruleRow = dynamic_cast<RuleRow *>(row);
    ResetRuleEditor();
    ParseHyprClients(ruleRow->clients);
  }
}

void MainWindow::ParseHyprClients(nlohmann::basic_json<> clients) {
  regexEntrys["title"]->set_text(clients["title"].get<std::string>());
  regexEntrys["class"]->set_text(clients["class"].get<std::string>());
  regexEntrys["initial_class"]->set_text(
      clients["initialClass"].get<std::string>());
  regexEntrys["initial_title"]->set_text(
      clients["initialTitle"].get<std::string>());

  if (clients["floating"].get<bool>()) {
    sizeXEntry.set_text(std::to_string(clients["size"][0].get<int>()));
    sizeYEntry.set_text(std::to_string(clients["size"][1].get<int>()));
    posXEntry.set_text(std::to_string(clients["at"][0].get<int>()));
    posYEntry.set_text(std::to_string(clients["at"][1].get<int>()));
    checkButtons["float"]->set_active(true);
  }
}
void MainWindow::RefreshRulesList() {
  while (auto child = listRules.get_first_child()) {
    listRules.remove(*child);
  }

  Loader loader;
  std::vector<std::string> winProps;
  std::vector<std::string> rulesStrings;
  std::vector<std::vector<int>> lineNum;
  if (!loader.LoadOnlyProps(winProps, rulesStrings, lineNum, configPath)) {
    FileErrorAlert();
    return;
  }

  for (uint i = 0; i < winProps.size(); i++) {
    RuleRow *row = make_managed<RuleRow>();
    std::vector<int> ruleLineNum = lineNum.at(i);
    Label label(winProps.at(i), Align::START);
    row->set_child(label);
    row->ruleLineNum = ruleLineNum;
    row->set_name(rulesStrings.at(i));

    listRules.append(*row);
  }
}

void MainWindow::OnRuleSelected(Gtk::ListBoxRow *row) {
  if (row) {
    auto ruleRow = dynamic_cast<RuleRow *>(row);
    ResetRuleEditor();
    LoadRule(ruleRow->get_name(), ruleRow->ruleLineNum);
  }
}

void MainWindow::InitRuleEditor() {
  mainEditRuleBox.set_orientation(Orientation::VERTICAL);
  Label titleLabel;
  titleLabel.set_markup(
      "<b>Props</b>\n(used to determine if a window should get the rule)");
  titleLabel.set_justify(Gtk::Justification::CENTER);
  titleLabel.set_margin_bottom(2);
  Box editRuleBox;
  editRuleBox.append(titleLabel);
  ScrolledWindow editRuleWindow;
  editRuleWindow.set_vexpand(true);
  editRuleWindow.set_child(editRuleBox);
  mainEditRuleBox.append(editRuleWindow);
  editRuleBox.set_margin(5);
  editRuleBox.set_orientation(Orientation::VERTICAL);

  std::string tooltipS =
      "<b>Unimportant:</b> this parameter will not be used in rule\n<b>Exact "
      "match:</b> this parameter must be the "
      "same as the window parameter\n<b>Contain:</b> window parameter must "
      "contain this parameter. &lt;any "
      "text&gt;your rule"
      "&lt;any "
      "text&gt;\n<b>Contain left:</b> &lt;any text&gt;your rule\n<b>Contain "
      "right:</b> your rule&lt;any text&gt;";

  auto stringList = StringList::create({"Unimportant", "Exact match", "Contain",
                                        "Contain Left", "Contain Right"});
  for (auto i : regexFields) {
    Label lb(i.name, Align::START);
    lb.set_margin(2);
    Entry *entry = make_managed<Entry>();
    entry->set_margin(2);
    entry->set_tooltip_text(i.tooltipText);
    DropDown *dropDown = make_managed<DropDown>();
    dropDown->set_margin(2);
    dropDown->set_model(stringList);
    dropDown->set_tooltip_markup(tooltipS);
    dropDown->property_selected().signal_changed().connect(
        [dropDown, entry, i]() {
          HandleRegExProps(i.keyword, entry, dropDown);
        });
    entry->signal_changed().connect([dropDown, entry, i]() {
      HandleRegExProps(i.keyword, entry, dropDown);
    });
    editRuleBox.append(lb);
    editRuleBox.append(*entry);
    editRuleBox.append(*dropDown);
    regexDropDowns[i.keyword] = dropDown;
    regexEntrys[i.keyword] = entry;
  }

  // простые пропы
  Box *simplePropBox = nullptr;
  auto stringListSProps =
      StringList::create({"Unimportant", "Match", "Doesn't match"});
  for (auto i : simpleProps) {
    Gtk::DropDown *dd = make_managed<Gtk::DropDown>();
    Gtk::Label label(i.name);
    label.set_margin(2);
    dd->set_margin(2);
    dd->set_model(stringListSProps);
    dd->set_tooltip_text(i.tooltipText);
    dd->property_selected().signal_changed().connect(
        [i, dd]() { HandleDropDown(i.keyword, dd); });

    if (simplePropBox == nullptr) {
      simplePropBox = make_managed<Box>();
      simplePropBox->append(label);
      simplePropBox->append(*dd);
    } else {
      simplePropBox->append(label);
      simplePropBox->append(*dd);
      editRuleBox.append(*simplePropBox);
      simplePropBox = nullptr;
    }
    dropDowns[i.keyword] = dd;
  }
  // раздел2
  Separator separator;
  editRuleBox.append(separator);
  Label effectsLabel;
  effectsLabel.set_markup("<b>Effects</b>");
  effectsLabel.set_margin(2);
  editRuleBox.append(effectsLabel);
  // прозрачность
  Label opacityL;
  opacityL.set_markup("<b>Opacity</b>");
  opacityL.set_halign(Align::START);
  editRuleBox.append(opacityL);
  Box activeOpBox;
  activeOpBox.set_margin(4);

  Label actL("Active opacity%");
  activeOpBox.append(actL);
  activeOpScale.set_hexpand(true);
  activeOpScale.set_range(0, 100);
  activeOpScale.set_increments(1, 10);
  activeOpScale.set_value(100);
  activeOpBox.append(activeOpScale);
  activeOpacity.set_range(0, 100);
  activeOpacity.set_increments(1, 10);
  activeOpacity.set_value(100);
  activeOpBox.append(activeOpacity);

  // прозрачность
  Box inactiveOpBox;
  inactiveOpBox.set_margin(4);
  Label inacL("Inactive opacity%");
  inactiveOpBox.append(inacL);
  inactiveOpScale.set_hexpand(true);
  inactiveOpScale.set_range(0, 100);
  inactiveOpScale.set_increments(1, 10);
  inactiveOpScale.set_value(100);
  inactiveOpBox.append(inactiveOpScale);
  inactiveOpacity.set_range(0, 100);
  inactiveOpacity.set_increments(1, 10);
  inactiveOpacity.set_value(100);
  inactiveOpBox.append(inactiveOpacity);
  inactiveOpBox.set_margin(4);

  Box fullscreenOpBox;
  fullscreenOpBox.set_margin(4);
  Label fullL("Fullscreen opacity%");
  fullscreenOpBox.append(fullL);
  fullscreenOpScale.set_hexpand(true);
  fullscreenOpScale.set_range(0, 100);
  fullscreenOpScale.set_increments(1, 10);
  fullscreenOpScale.set_value(100);
  fullscreenOpBox.append(fullscreenOpScale);
  fullscreenOpacity.set_range(0, 100);
  fullscreenOpacity.set_increments(1, 10);
  fullscreenOpacity.set_value(100);
  fullscreenOpBox.append(fullscreenOpacity);

  editRuleBox.append(activeOpBox);
  editRuleBox.append(inactiveOpBox);
  editRuleBox.append(fullscreenOpBox);

  activeOpacity.signal_value_changed().connect([this]() {
    HandleOpacityUpdate(&activeOpacity, &inactiveOpacity, &fullscreenOpacity,
                        &activeOpScale, &activeOpacity);
  });
  activeOpScale.signal_value_changed().connect(
      [this]() { activeOpacity.set_value(activeOpScale.get_value()); });

  inactiveOpacity.signal_value_changed().connect([this]() {
    HandleOpacityUpdate(&activeOpacity, &inactiveOpacity, &fullscreenOpacity,
                        &inactiveOpScale, &inactiveOpacity);
  });
  inactiveOpScale.signal_value_changed().connect(
      [this]() { inactiveOpacity.set_value(inactiveOpScale.get_value()); });

  fullscreenOpacity.signal_value_changed().connect([this]() {
    HandleOpacityUpdate(&activeOpacity, &inactiveOpacity, &fullscreenOpacity,
                        &fullscreenOpScale, &fullscreenOpacity);
  });
  fullscreenOpScale.signal_value_changed().connect(
      [this]() { fullscreenOpacity.set_value(fullscreenOpScale.get_value()); });

  // тип

  Label winTypeLabel;
  editRuleBox.append(winTypeLabel);
  winTypeLabel.set_markup("<b>Window type</b>");
  winTypeLabel.set_halign(Align::START);

  Box winTypeBox;
  floating.set_label("Floating");
  floating.set_tooltip_text("Floats a window");
  floating.signal_toggled().connect(
      []() { HandleWindowTypeUpdae(WindowType::floating); });
  winTypeBox.append(floating);
  tile.set_label("Tile");
  tile.set_tooltip_text("Tiles a window");
  tile.signal_toggled().connect(
      []() { HandleWindowTypeUpdae(WindowType::tile); });
  tile.set_group(floating);
  winTypeBox.append(tile);
  pseudotile.set_label("Pseudotile");
  pseudotile.set_tooltip_text("Pseudotiles a window");
  pseudotile.signal_toggled().connect(
      []() { HandleWindowTypeUpdae(WindowType::pseudotile); });
  pseudotile.set_group(floating);
  winTypeBox.append(pseudotile);
  fullscreen.set_label("Fullscreen");
  fullscreen.set_tooltip_text("Fullscreens a window");
  fullscreen.signal_toggled().connect(
      []() { HandleWindowTypeUpdae(WindowType::fullscreen); });
  winTypeBox.append(fullscreen);
  fullscreen.set_group(floating);
  maximize.set_label("Maximize");
  maximize.set_tooltip_text("Maximizes a window");
  maximize.signal_toggled().connect(
      []() { HandleWindowTypeUpdae(WindowType::maximize); });
  maximize.set_group(floating);
  winTypeBox.append(maximize);
  noType.set_label("Default");
  noType.set_active(true);
  noType.signal_toggled().connect(
      []() { HandleWindowTypeUpdae(WindowType::none); });
  noType.set_group(floating);
  winTypeBox.append(noType);
  editRuleBox.append(winTypeBox);

  // размер
  Label sizeLabel;
  sizeLabel.set_halign(Align::START);
  sizeLabel.set_markup("<b>Size</b>");
  sizeLabel.set_margin(2);
  editRuleBox.append(sizeLabel);
  Box sizeBox;
  Label sXLabel("X:");
  Label sYLabel("Y:");
  sizeBox.append(sXLabel);
  sizeBox.append(sizeXEntry);
  sizeBox.append(sYLabel);
  sizeBox.append(sizeYEntry);
  editRuleBox.append(sizeBox);
  sizeXEntry.signal_changed().connect(
      [this]() { HandleTwoFieldsUpdate("size", &sizeXEntry, &sizeYEntry); });
  sizeYEntry.signal_changed().connect(
      [this]() { HandleTwoFieldsUpdate("size", &sizeXEntry, &sizeYEntry); });
  sizeXEntry.set_tooltip_text(
      "Resizes a floating window. Can be int or %, e.g. 1280 or 50%");
  sizeYEntry.set_tooltip_text(
      "Resizes a floating window. Can be int or %, e.g. 1280 or 50%");
  // положение
  Label posLabel;
  posLabel.set_halign(Align::START);
  posLabel.set_markup("<b>Position</b>");
  posLabel.set_margin(2);
  editRuleBox.append(posLabel);

  Box posBox;
  Label pXLabel("X:");
  Label pYLabel("Y:");
  posBox.append(pXLabel);
  posBox.append(posXEntry);
  posBox.append(pYLabel);
  posBox.append(posYEntry);
  editRuleBox.append(posBox);
  posXEntry.signal_changed().connect(
      [this]() { HandleTwoFieldsUpdate("move", &posXEntry, &posYEntry); });
  posYEntry.signal_changed().connect(
      [this]() { HandleTwoFieldsUpdate("move", &posXEntry, &posYEntry); });
  posXEntry.set_tooltip_text(
      "Moves a floating window. Can be int or %, e.g. 1280 or 50%");
  posYEntry.set_tooltip_text(
      "Moves a floating window. Can be int or %, e.g. 1280 or 50%");
  // простые
  Label simplerLabel;
  simplerLabel.set_halign(Align::START);
  simplerLabel.set_markup("<b>Simple props</b>");
  simplerLabel.set_margin(2);
  editRuleBox.append(simplerLabel);
  Box *simpleRulBox = nullptr;
  for (auto i : simpleRules) {
    Gtk::CheckButton *cb = make_managed<Gtk::CheckButton>();
    cb->set_label(i.name);
    cb->set_tooltip_text(i.tooltipText);
    cb->signal_toggled().connect(
        [i, cb]() { HandleCheckButtonUpdate(i.keyword, cb); });
    if (simpleRulBox == nullptr) {
      simpleRulBox = make_managed<Box>();
      simpleRulBox->append(*cb);
    } else {
      simpleRulBox->append(*cb);
      editRuleBox.append(*simpleRulBox);
      simpleRulBox = nullptr;
    }
    checkButtons[i.keyword] = cb;
  }
  checkButtons["float"] = &floating;
  checkButtons["fullscreen"] = &fullscreen;
  checkButtons["tile"] = &tile;
  checkButtons["pseudo"] = &pseudotile;
  checkButtons["maximize"] = &maximize;

  Box bottomBox;
  Button *clearB = make_managed<Button>("Clear");
  clearB->signal_clicked().connect([this]() {
    ResetRuleEditor();
    RuleConfig::InitRule(new Rule());
  });
  clearB->set_margin(5);
  clearB->set_hexpand(false);

  Button *saveB = make_managed<Button>("Save");
  saveB->signal_clicked().connect([this]() {
    int result = RuleConfig::Save(configPath);
    if (result == 1)
      FileErrorAlert();
    else if (result == 2) {
      savingStateNotify.set_text("Rule is empty. Nothing to save");
      savingStateNotify.show();
    } else {
      savingStateNotify.set_text("Saved to " + configPath);
      savingStateNotify.show();
    }
    Glib::signal_timeout().connect(
        [&]() {
          savingStateNotify.hide();
          return false;
        },
        1777);
    RefreshRulesList();
    RefreshWindowsList();
  });
  saveB->set_margin(5);
  Box t;
  t.append(savingStateNotify);
  t.set_hexpand(true);
  bottomBox.append(t);
  bottomBox.append(*clearB);
  bottomBox.append(*saveB);
  mainEditRuleBox.append(bottomBox);
}

void MainWindow::ResetRuleEditor() {
  noType.set_active(true);
  for (auto i : dropDowns)
    i.second->set_selected(0);
  for (auto i : regexDropDowns)
    i.second->set_selected(0);
  for (auto i : regexEntrys)
    i.second->set_text("");
  for (auto i : checkButtons)
    i.second->set_active(false);

  activeOpacity.set_value(100);
  inactiveOpacity.set_value(100);
  fullscreenOpacity.set_value(100);
  posXEntry.set_text("");
  posYEntry.set_text("");
  sizeXEntry.set_text("");
  sizeYEntry.set_text("");
}

void MainWindow::LoadRule(std::string ruleString,
                          std::vector<int> &ruleLineNum) {
  Loader loader;
  Rule *rule = new Rule();

  loader.LoadFull(ruleString, rule);

  RuleConfig::InitRule(rule);
  RuleConfig::SetLines(ruleLineNum);
  for (auto i : rule->props) {
    std::string prop = i.first;
    if (dropDowns.contains(prop))
      ParseDropDown(i.second, dropDowns[prop]);
    else if (regexEntrys.contains(prop))
      ParseRegExProps(i.second, regexEntrys[prop], regexDropDowns[prop]);
  }
  for (auto i : rule->effects) {
    std::string prop = i.first;
    if (checkButtons.contains(prop))
      checkButtons[prop]->set_active(true);
    else if (prop == "size")
      ParseTwoFields(i.second, &sizeXEntry, &sizeYEntry);
    else if (prop == "pos")
      ParseTwoFields(i.second, &posXEntry, &posYEntry);
    else if (prop == "opacity")
      ParseOpacity(i.second, &activeOpacity, &inactiveOpacity,
                   &fullscreenOpacity);
  }
}

void MainWindow::DeleteRule(Gtk::ListBoxRow *row) {
  if (row) {
    Saver saver;
    auto ruleRow = dynamic_cast<RuleRow *>(row);
    if (!saver.DeleteRule(ruleRow->ruleLineNum, configPath)) {
      FileErrorAlert();
      return;
    }
    RefreshRulesList();
  }
}
void MainWindow::FileErrorAlert() {
  auto dialog = AlertDialog::create("Error");
  dialog->set_detail("File " + configPath + " not found");
  dialog->show();
}

void MainWindow::SelectConfigPath() {
  auto fileDialog = Gtk::FileDialog::create();
  fileDialog->set_title("Select window rules file");
  fileDialog->open(*this,
                   [fileDialog, this](Glib::RefPtr<Gio::AsyncResult> &result) {
                     auto file = fileDialog->open_finish(result);
                     configPath = file->get_path();
                     settings->set_string("config-path", configPath);
                   });
}
