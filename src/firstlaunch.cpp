#include "include/firstlaunch.h"

#include "gtkmm/alertdialog.h"
#include "gtkmm/filedialog.h"
#include <fstream>
#include <iostream>

void FirstLaunch(MainWindow* window, Glib::RefPtr<Gio::Settings> settings, std::string& configPath)
{
    using namespace Gtk;
    struct stat buffer;
    if ((stat(configPath.c_str(), &buffer) == 0))
    {
        auto dialog = AlertDialog::create("короче");
        dialog->set_detail(configPath + " has been found.");
        dialog->set_detail(
            "Rules are loaded only from it. If your rules are stored in a different file, specify that file.");
        dialog->set_buttons({"Select window rules file", "Ok"});
        dialog->choose(*window, [dialog, window, &configPath, settings](Glib::RefPtr<Gio::AsyncResult>& result) {
            int response = dialog->choose_finish(result);

            if (response == 0)
            {
                auto fileDialog = Gtk::FileDialog::create();
                fileDialog->set_title("Select window rules file");
                fileDialog->open(*window, [fileDialog, &configPath, settings](Glib::RefPtr<Gio::AsyncResult>& result) {
                    auto file = fileDialog->open_finish(result);
                    // std::cout << file->get_path();
                    configPath = file->get_path();
                    settings->set_string("config-path", configPath);
                });
            }
            else
            {
                settings->set_string("config-path", configPath);
            }
        });
    }
    else
    {
        auto dialog = AlertDialog::create(configPath + " not found.");
        dialog->set_detail(
            "This program load rules only from it. If your rules are stored in a different file, specify that file.");
        dialog->set_buttons({"Select window rules file", "Create windowrules.conf"});
        dialog->choose(*window, [dialog, window, &configPath, settings](Glib::RefPtr<Gio::AsyncResult>& result) {
            int response = dialog->choose_finish(result);
            if (response == 0)
            {
                auto fileDialog = Gtk::FileDialog::create();
                fileDialog->set_title("Select window rules file");
                fileDialog->open(*window, [fileDialog, &configPath, settings](Glib::RefPtr<Gio::AsyncResult>& result) {
                    auto file = fileDialog->open_finish(result);
                    // std::cout << file->get_path();
                    configPath = file->get_path();
                    settings->set_string("config-path", configPath);
                });
            }
            else
            {
                std::ofstream nfile(configPath);
                nfile.close();
                std::string hyprPath = configPath.substr(0, configPath.length() - 16);
                hyprPath += "/hyprland.conf";
                std::ofstream file(hyprPath, std::ios::app);
                file << std::endl << "source = ~/.config/hypr/windowrules.conf";
                std::cout << hyprPath << "путь до этого" << std::endl;
                settings->set_string("config-path", configPath);
            }
        });
    }
    settings->set_boolean("first-launch", false);
}
