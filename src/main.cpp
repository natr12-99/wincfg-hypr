#include "gtkmm/application.h"
#include "include/window.h"

int main(int argc, char** argv)
{
    auto app = Gtk::Application::create("org.natr12.wincfg-hypr");
    return app->make_window_and_run<MainWindow>(argc, argv);
}
