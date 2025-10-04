#include "include/window.h"

#include "glib.h"
#include "gtkmm/alertdialog.h"
#include "gtkmm/button.h"
#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/listboxrow.h"
#include "gtkmm/object.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/separator.h"
#include "include/func.h"

#include "gtkmm/filedialog.h"
#include "include/firstlaunch.h"
#include "include/loader.h"
#include "include/regextype.h"
#include "include/saver.h"
#include "sigc++/adaptors/bind.h"
#include "sigc++/functors/mem_fun.h"

using namespace Gtk;

MainWindow::MainWindow()
{
    set_title("Hyprland rules editor");
    set_default_size(500, 500);
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

void MainWindow::SetOpacity(SpinButton* active, SpinButton* inactive, Scale* scale, bool activeToScale)
{
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

void MainWindow::ModifyOpacity()
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
    Label tittleLabel("Window tittle", 55);
    editRuleBox.append(tittleLabel);
    editRuleBox.set_orientation(Orientation::VERTICAL);

    editRuleBox.append(tittleEntry);

    matchT.set_label("full match");
    containT.set_label("contains");
    notUseT.set_label("do not use this parameter");

    containT.set_group(matchT);
    notUseT.set_group(matchT);

    matchT.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(config, &RuleConfig::ChangeWinRegEx), RegexType::fullMatch));
    containT.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(config, &RuleConfig::ChangeWinRegEx), RegexType::contain));
    notUseT.signal_toggled().connect(sigc::bind(sigc::mem_fun(config, &RuleConfig::ChangeWinRegEx), RegexType::nouse));

    Box box;
    box.append(matchT);
    box.append(containT);
    box.append(notUseT);
    editRuleBox.append(box);

    Separator separator;
    editRuleBox.append(separator);

    Label classLabel("Window class");
    editRuleBox.append(classLabel);

    editRuleBox.append(classEntry);

    tittleEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &MainWindow::SetRuleStrings), &tittleEntry, &classEntry));
    classEntry.signal_changed().connect(
        sigc::bind(sigc::mem_fun(*this, &MainWindow::SetRuleStrings), &tittleEntry, &classEntry));

    matchC.set_label("full match");
    containC.set_label("contains");
    notUseC.set_label("do not use this parameter");
    containC.set_group(matchC);
    notUseC.set_group(matchC);

    matchC.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(config, &RuleConfig::ChangeClsRegEx), RegexType::fullMatch));
    containC.signal_toggled().connect(
        sigc::bind(sigc::mem_fun(config, &RuleConfig::ChangeClsRegEx), RegexType::contain));
    notUseC.signal_toggled().connect(sigc::bind(sigc::mem_fun(config, &RuleConfig::ChangeClsRegEx), RegexType::nouse));

    Box box2;
    box2.append(matchC);
    box2.append(containC);
    box2.append(notUseC);
    editRuleBox.append(box2);

    Separator separator2;
    editRuleBox.append(separator2); // раздел2

    editRuleBox.append(modifyOpacity);
    modifyOpacity.set_label("Modify opacity");
    modifyOpacity.set_active(false);
    modifyOpacity.signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::ModifyOpacity));
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

    // плаванье
    floating.set_label("floating");
    floating.signal_toggled().connect([this]() { config.ChangeFloating(floating.get_active()); });
    editRuleBox.append(floating);

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

    // все остальное
    Box spacer(Orientation::VERTICAL);
    spacer.set_hexpand(true);
    spacer.set_vexpand(true);
    editRuleBox.append(spacer);

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
    editRuleBox.append(bottomBox);
}

void MainWindow::OpenRuleEditor(std::string wTitle, std::string wClass, Box* _prevBox)
{
    config.InitRule(wTitle, wClass);

    prevBox = _prevBox;
    tittleEntry.set_text(wTitle);
    classEntry.set_text(wClass);
    set_child(editRuleBox);
}

void MainWindow::ResetRuleEditor()
{
    notUseC.set_active(true);
    notUseT.set_active(true);
    floating.set_active(false);
    activeOpacity.set_value(100);
    inactiveOpacity.set_value(100);
    modifyOpacity.set_active(false);
    posXEntry.set_text("");
    posYEntry.set_text("");
    sizeXEntry.set_text("");
    sizeYEntry.set_text("");
}

void MainWindow::LoadRule(std::string wTitle, RegexType rTitle, std::string wClass, RegexType rClass,
                          std::vector<int>& ruleLineNum)
{
    Loader loader;
    bool isFloating = false;
    int opacityActive = -1;
    int opacityInactive = -1;
    std::string posX, posY, sizeX, sizeY;
    if (!loader.LoadFull(ruleLineNum, isFloating, opacityActive, opacityInactive, posX, posY, sizeX, sizeY, configPath))
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

    floating.set_active(isFloating);
    config.ChangeFloating(isFloating);

    config.SetLines(ruleLineNum);

    if (rTitle == RegexType::fullMatch)
    {
        matchT.set_active(true);
        config.ChangeWinRegEx(RegexType::fullMatch);
    }
    else if (rTitle == RegexType::contain)
    {
        containT.set_active(true);
        config.ChangeWinRegEx(RegexType::contain);
    }
    else
        notUseT.set_active(true);

    if (rClass == RegexType::fullMatch)
    {
        matchC.set_active(true);
        config.ChangeClsRegEx(RegexType::fullMatch);
    }
    else if (rClass == RegexType::contain)
    {
        containC.set_active(true);
        config.ChangeClsRegEx(RegexType::contain);
    }
    else
        notUseC.set_active(true);

    posXEntry.set_text(posX);
    posYEntry.set_text(posY);
    sizeXEntry.set_text(sizeX);
    sizeYEntry.set_text(sizeY);
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
