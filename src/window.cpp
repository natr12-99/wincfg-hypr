#include "include/window.h"

#include "glib.h"
#include "gtkmm/alertdialog.h"
#include "gtkmm/button.h"
#include "gtkmm/enums.h"
#include "gtkmm/filedialog.h"
#include "gtkmm/label.h"
#include "gtkmm/listboxrow.h"
#include "gtkmm/object.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/separator.h"
#include "gtkmm/stringlist.h"
#include "include/firstlaunch.h"
#include "include/func.h"
#include "include/json.hpp"
#include "include/loader.h"
#include "include/regextype.h"
#include "include/saver.h"
#include "include/windowtype.h"
#include "sigc++/adaptors/bind.h"
#include "sigc++/functors/mem_fun.h"
#include <string>

using namespace Gtk;

MainWindow::MainWindow()
{
    set_title("Hyprland rules editor");
    set_default_size(500, 520);
    set_resizable(false);
    InitRuleEditor();

    refreshListButton.set_icon_name("view-refresh-symbolic");
    refreshListButton.set_tooltip_text("Refresh");
    refreshListButton.set_margin(5);
    windowSelectBox.set_orientation(Orientation::VERTICAL);
    refreshListButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::RefreshWindowsList));
    goToRuleList.signal_clicked().connect([this]() { set_child(ruleSelectBox); });
    goToRuleList.set_icon_name("go-previous-symbolic");
    goToRuleList.set_tooltip_text("Back to rules list");
    goToRuleList.set_margin(5);

    newEmtryRuleButton.set_label("New empty rule");
    newEmtryRuleButton.set_margin(5);
    newEmtryRuleButton.signal_clicked().connect([this]() {
        OpenRuleEditor("", "", &windowSelectBox);
        ResetRuleEditor();
    });
    listClients.set_margin(5);

    Box windowListTopBox;
    windowListTopBox.set_hexpand(true);
    Label windowSelectL;
    windowSelectL.set_hexpand(true);
    windowSelectL.set_markup("<b>New rule</b>");
    windowListTopBox.append(windowSelectL);
    windowListTopBox.append(refreshListButton);
    windowListTopBox.append(goToRuleList);

    windowSelectBox.append(windowListTopBox);
    windowSelectBox.append(newEmtryRuleButton);
    ScrolledWindow swClients;
    swClients.set_vexpand(true);
    swClients.set_child(listClients);
    windowSelectBox.append(swClients);
    //
    refreshRulesListButton.set_icon_name("view-refresh-symbolic");
    refreshRulesListButton.set_tooltip_text("Refresh");
    refreshRulesListButton.set_margin(5);
    refreshRulesListButton.set_vexpand(false);
    ruleSelectBox.set_orientation(Orientation::VERTICAL);
    refreshRulesListButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::RefreshRulesList));
    selectConfigButton.set_icon_name("document-edit-symbolic");
    selectConfigButton.set_tooltip_text("Select config file");
    selectConfigButton.set_margin(5);
    selectConfigButton.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::SelectConfigPath));
    goToWindowsList.signal_clicked().connect([this]() {
        RefreshWindowsList();
        set_child(windowSelectBox);
    });
    goToWindowsList.set_icon_name("document-new-symbolic");
    goToWindowsList.set_tooltip_text("New rule");
    goToWindowsList.set_margin(5);
    listRules.set_margin(5);

    Box rulesListTopBox;
    rulesListTopBox.append(selectConfigButton);
    rulesListTopBox.set_hexpand(true);
    Label ruleSelectL;
    ruleSelectL.set_hexpand(true);
    ruleSelectL.set_markup("<b>Select rule</b>");
    rulesListTopBox.append(ruleSelectL);
    rulesListTopBox.append(refreshRulesListButton);
    rulesListTopBox.append(goToWindowsList);

    ruleSelectBox.append(rulesListTopBox);
    ScrolledWindow swRules;
    swRules.set_child(listRules);
    swRules.set_vexpand(true);
    ruleSelectBox.append(swRules);

    set_child(ruleSelectBox);

    configPath = g_get_user_config_dir();
    configPath += "/hypr/windowrules.conf";

    settings = Gio::Settings::create("org.natr12.wincfg-hypr");
    if (settings->get_boolean("first-launch"))
    {
        FirstLaunch(this, settings, configPath);
    }
    else
    {
        configPath = settings->get_string("config-path");
        RefreshRulesList();
    }
}

void MainWindow::RefreshWindowsList()
{
    while (auto child = listClients.get_first_child())
    {
        listClients.remove(*child);
    }
    std::string clientsStr;
    clientsStr = exec("hyprctl clients -j");

    nlohmann::json clients;
    try
    {
        clients = nlohmann::json::parse(clientsStr);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        auto dialog = AlertDialog::create("Eror getting window list: " + clientsStr);
        dialog->show();
        return;
    }

    for (auto item : clients)
    {
        Button* but = make_managed<Button>();
        ListBoxRow row;
        std::string s_class = "class: ";
        s_class += item["class"];
        std::string s_title = "title: ";
        s_title += item["title"];

        Box box;
        box.set_orientation(Orientation::VERTICAL);
        Label wClass(s_class);
        Label wTitle(s_title);

        box.append(wTitle);
        box.append(wClass);
        but->set_child(box);

        but->signal_clicked().connect([this, item]() {
            OpenRuleEditor(item["title"], item["class"], &windowSelectBox);
            ResetRuleEditor();
        });

        row.set_child(*but);
        listClients.append(row);
    }
}

void MainWindow::RefreshRulesList()
{
    while (auto child = listRules.get_first_child())
    {
        listRules.remove(*child);
    }

    Loader loader;
    std::vector<std::string> winNames;
    std::vector<RegexType> RNames;
    std::vector<std::string> winClasses;
    std::vector<RegexType> RClasses;
    std::vector<std::vector<int>> lineNum;
    if (!loader.LoadOnlyNames(winNames, RNames, winClasses, RClasses, lineNum, configPath))
    {
        FileErrorAlert();
        return;
    }

    for (int i = 0; i < winNames.size(); i++)
    {
        ListBoxRow row;
        Button* but = make_managed<Button>();
        Box box;
        box.set_orientation(Orientation::VERTICAL);
        std::string s_class = winClasses.at(i);
        if (s_class.empty())
            s_class = "class: empty";
        else
            s_class = "class: " + s_class;
        std::string s_title = winNames.at(i);
        if (s_title.empty())
            s_title = "title: empty";
        else
            s_title = "title: " + s_title;
        std::vector<int> ruleLineNum = lineNum.at(i);
        Label wClass(s_class);
        Label wTitle(s_title);

        box.append(wTitle);
        box.append(wClass);
        but->set_hexpand(true);
        but->set_child(box);
        but->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::LoadRule), winNames.at(i),
                                                 RNames.at(i), winClasses.at(i), RClasses.at(i), ruleLineNum));

        Button* delBut = make_managed<Button>();
        delBut->set_icon_name("edit-delete-symbolic");
        delBut->set_tooltip_text("Delete rule");
        delBut->signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::DeleteRule), ruleLineNum));
        Box box2;
        box2.set_hexpand(true);
        box2.append(*but);
        box2.append(*delBut);

        row.set_child(box2);
        listRules.append(row);
    }
}

void MainWindow::SetRuleStrings(Entry* t, Entry* c)
{
    config.ChangeRuleStr(c->get_text(), t->get_text());
}
void MainWindow::SetRegex()
{
    config.ChangeClsRegEx(static_cast<RegexType>(dropdownC.get_selected()));
    config.ChangeWinRegEx(static_cast<RegexType>(dropdownT.get_selected()));
}
void MainWindow::SetOpacity(SpinButton* active, SpinButton* inactive, Scale* scale, bool activeToScale)
{
    if (modifyOpacity.get_active())
        config.ChangeOpacity(active->get_value_as_int(), inactive->get_value_as_int());

    scale->freeze_notify();
    if (activeToScale)
        scale->set_value(active->get_value_as_int());
    else
        scale->set_value(inactive->get_value_as_int());
    scale->thaw_notify();
}

void MainWindow::SetSize()
{
    config.ChangeSize(sizeXEntry.get_text(), sizeYEntry.get_text());
}

void MainWindow::SetPos()
{
    config.ChangePos(posXEntry.get_text(), posYEntry.get_text());
}

void MainWindow::SetModifyOpacity()
{
    if (!modifyOpacity.get_active())
    {
        opacityBox.set_sensitive(false);
        config.ChangeOpacity(-1, -1);
    }
    else
    {
        opacityBox.set_sensitive(true);
        config.ChangeOpacity(activeOpacity.get_value_as_int(), inactiveOpacity.get_value_as_int());
    }
}

void MainWindow::InitRuleEditor()
{
    mainEditRuleBox.set_orientation(Orientation::VERTICAL);
    Label titleLabel("Window title");
    titleLabel.set_margin_bottom(2);
    Box editRuleBox;
    ScrolledWindow editRuleWindow;
    editRuleWindow.set_vexpand(true);
    editRuleWindow.set_child(editRuleBox);
    mainEditRuleBox.append(editRuleWindow);
    editRuleBox.set_margin(5);
    editRuleBox.append(titleLabel);
    editRuleBox.set_orientation(Orientation::VERTICAL);

    editRuleBox.append(titleEntry);
    std::string tooltipS =
        "<b>Unimportant:</b> this parameter will not be used in rule\n<b>Exact match:</b> this parameter must be the "
        "same as the window parameter\n<b>Contain:</b> window parameter must contain this parameter. &lt;any "
        "text&gt;your rule"
        "&lt;any "
        "text&gt;\n<b>Contain left:</b> &lt;any text&gt;your rule\n<b>Contain right:</b> your rule&lt;any text&gt;";

    auto stringList = StringList::create({"Unimportant", "Exact match", "Contain", "Contain Left", "Contain Right"});
    dropdownT.set_model(stringList);
    dropdownT.property_selected().signal_changed().connect(sigc::mem_fun(*this, &MainWindow::SetRegex));
    dropdownT.set_tooltip_markup(tooltipS);
    dropdownT.set_margin_top(2);
    editRuleBox.append(dropdownT);

    Separator separator;
    editRuleBox.append(separator);

    Label classLabel("Window class");
    classLabel.set_margin_bottom(2);
    editRuleBox.append(classLabel);

    editRuleBox.append(classEntry);

    dropdownC.set_model(stringList);
    dropdownC.property_selected().signal_changed().connect(sigc::mem_fun(*this, &MainWindow::SetRegex));
    dropdownC.set_tooltip_markup(tooltipS);
    dropdownC.set_margin_top(2);
    editRuleBox.append(dropdownC);

    titleEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &MainWindow::SetRuleStrings), &titleEntry, &classEntry));
    classEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &MainWindow::SetRuleStrings), &titleEntry, &classEntry));

    classEntry.set_tooltip_text("Windows with class matching RegEx below");
    titleEntry.set_tooltip_text("Windows with title matching RegEx below");
    Separator separator2;
    editRuleBox.append(separator2); // раздел2

    editRuleBox.append(modifyOpacity);
    modifyOpacity.set_label("Modify opacity");
    modifyOpacity.set_active(false);
    modifyOpacity.signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::SetModifyOpacity));
    opacityBox.set_orientation(Orientation::VERTICAL);
    opacityBox.set_sensitive(false);
    // прозрачность
    Box activeOpBox;
    activeOpBox.set_margin(4);

    Label actL("active opacity%   ");
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
    opacityBox.append(activeOpBox);

    // прозрачность
    Box inactiveOpBox;
    inactiveOpBox.set_margin(4);

    Label inacL("inactive opacity%");
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
    opacityBox.append(inactiveOpBox);

    editRuleBox.append(opacityBox);

    activeOpacity.signal_value_changed().connect((sigc::bind(sigc::mem_fun(*this, &MainWindow::SetOpacity),
                                                             &activeOpacity, &inactiveOpacity, &activeOpScale, true)));
    activeOpScale.signal_value_changed().connect([this]() { activeOpacity.set_value(activeOpScale.get_value()); });

    inactiveOpacity.signal_value_changed().connect((sigc::bind(
        sigc::mem_fun(*this, &MainWindow::SetOpacity), &activeOpacity, &inactiveOpacity, &inactiveOpScale, false)));
    inactiveOpScale.signal_value_changed().connect(
        [this]() { inactiveOpacity.set_value(inactiveOpScale.get_value()); });

    // тип
    Separator winTypeSeparator;
    editRuleBox.append(winTypeSeparator);
    Label winTypeLabel("Window Type");
    editRuleBox.append(winTypeLabel);
    Box winTypeBox;
    floating.set_label("Floating");
    floating.set_tooltip_text("Floats a window");
    floating.signal_toggled().connect([this]() { config.ChangeWindowType(WindowType::floating); });
    winTypeBox.append(floating);
    tile.set_label("Tile");
    tile.set_tooltip_text("Tiles a window");
    tile.signal_toggled().connect([this]() { config.ChangeWindowType(WindowType::tile); });
    tile.set_group(floating);
    winTypeBox.append(tile);
    fullscreen.set_label("Fullscreen");
    fullscreen.set_tooltip_text("Fullscreens a window");
    fullscreen.signal_toggled().connect([this]() { config.ChangeWindowType(WindowType::fullscreen); });
    winTypeBox.append(fullscreen);
    fullscreen.set_group(floating);
    maximize.set_label("Maximize");
    maximize.set_tooltip_text("Maximizes a window");
    maximize.signal_toggled().connect([this]() { config.ChangeWindowType(WindowType::maximize); });
    maximize.set_group(floating);
    winTypeBox.append(maximize);
    noType.set_label("Default");
    noType.signal_toggled().connect([this]() { config.ChangeWindowType(WindowType::none); });
    noType.set_group(floating);
    winTypeBox.append(noType);
    editRuleBox.append(winTypeBox);

    Separator sizeSeparator;
    editRuleBox.append(sizeSeparator);
    // размер
    Label sizeLabel("Size");
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
    sizeXEntry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::SetSize));
    sizeYEntry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::SetSize));
    sizeXEntry.set_tooltip_text("Resizes a floating window. Can be int or %, e.g. 1280 or 50%");
    sizeYEntry.set_tooltip_text("Resizes a floating window. Can be int or %, e.g. 1280 or 50%");
    // положение
    Label posLabel("Position");
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
    posXEntry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::SetPos));
    posYEntry.signal_changed().connect(sigc::mem_fun(*this, &MainWindow::SetPos));
    posXEntry.set_tooltip_text("Moves a floating window. Can be int or %, e.g. 1280 or 50%");
    posYEntry.set_tooltip_text("Moves a floating window. Can be int or %, e.g. 1280 or 50%");
    // закреп
    pinned.set_label("Pin");
    pinned.set_tooltip_text("Pins the window (i.e. show it on all workspaces). Note: floating only.");
    pinned.signal_toggled().connect([this]() { config.ChangePinned(pinned.get_active()); });
    editRuleBox.append(pinned);
    // все остальное

    Box bottomBox;
    Button* exitB = make_managed<Button>("Canel");
    exitB->signal_clicked().connect([this]() {
        set_child(*prevBox);
        RefreshRulesList();
        RefreshWindowsList();
    });
    exitB->set_margin(5);
    exitB->set_hexpand(false);

    Button* saveB = make_managed<Button>("Save");
    saveB->signal_clicked().connect([this]() {
        if (!config.Save(configPath))
            FileErrorAlert();
    });
    saveB->set_margin(5);
    Box t;
    t.set_hexpand(true);
    bottomBox.append(t);
    bottomBox.append(*exitB);
    bottomBox.append(*saveB);
    mainEditRuleBox.append(bottomBox);
}

void MainWindow::OpenRuleEditor(std::string wTitle, std::string wClass, Box* _prevBox)
{
    config.InitRule(wTitle, wClass);

    prevBox = _prevBox;
    titleEntry.set_text(wTitle);
    classEntry.set_text(wClass);
    set_child(mainEditRuleBox);
}

void MainWindow::ResetRuleEditor()
{
    dropdownT.set_selected(0);
    dropdownC.set_selected(0);
    noType.set_active(true);
    modifyOpacity.set_active(false);
    activeOpacity.set_value(100);
    inactiveOpacity.set_value(100);
    posXEntry.set_text("");
    posYEntry.set_text("");
    sizeXEntry.set_text("");
    sizeYEntry.set_text("");
    pinned.set_active(false);
}

void MainWindow::LoadRule(std::string wTitle, RegexType rTitle, std::string wClass, RegexType rClass,
                          std::vector<int>& ruleLineNum)
{
    Loader loader;
    int opacityActive = -1;
    int opacityInactive = -1;
    std::string posX, posY, sizeX, sizeY;
    WindowType winType = WindowType::none;
    bool isPinned = false;
    if (!loader.LoadFull(ruleLineNum, winType, opacityActive, opacityInactive, posX, posY, sizeX, sizeY, isPinned,
                         configPath))
    {
        FileErrorAlert();
        return;
    }
    OpenRuleEditor(wTitle, wClass, &ruleSelectBox);

    activeOpacity.set_value(opacityActive);
    inactiveOpacity.set_value(opacityInactive);
    if (opacityActive != -1)
    {
        modifyOpacity.set_active(true);
        config.ChangeOpacity(activeOpacity.get_value_as_int(), inactiveOpacity.get_value_as_int());
    }
    else
    {
        modifyOpacity.set_active(false);
    }

    switch (winType)
    {
    case WindowType::floating:
        floating.set_active(true);
        break;
    case WindowType::fullscreen:
        fullscreen.set_active(true);
        break;
    case WindowType::tile:
        tile.set_active(true);
        break;
    case WindowType::maximize:
        maximize.set_active(true);
        break;
    case WindowType::none:
        noType.set_active(true);
        break;
    };
    config.ChangeWindowType(winType);
    config.SetLines(ruleLineNum);

    dropdownT.set_selected(static_cast<int>(rTitle));
    config.ChangeWinRegEx(rTitle);
    dropdownC.set_selected(static_cast<int>(rClass));
    config.ChangeClsRegEx(rClass);
    posXEntry.set_text(posX);
    posYEntry.set_text(posY);
    sizeXEntry.set_text(sizeX);
    sizeYEntry.set_text(sizeY);

    pinned.set_active(isPinned);
    config.ChangePinned(isPinned);
}

void MainWindow::DeleteRule(std::vector<int>& ruleLineNum)
{
    Saver saver;
    if (!saver.DeleteRule(ruleLineNum, configPath))
    {
        FileErrorAlert();
        return;
    }
    RefreshRulesList();
}
void MainWindow::FileErrorAlert()
{
    auto dialog = AlertDialog::create("Error");
    dialog->set_detail("File " + configPath + " not found");
    dialog->show();
}

void MainWindow::SelectConfigPath()
{
    auto fileDialog = Gtk::FileDialog::create();
    fileDialog->set_title("Select window rules file");
    fileDialog->open(*this, [fileDialog, this](Glib::RefPtr<Gio::AsyncResult>& result) {
        auto file = fileDialog->open_finish(result);
        configPath = file->get_path();
        settings->set_string("config-path", configPath);
    });
}
