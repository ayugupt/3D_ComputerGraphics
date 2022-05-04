#include<iostream>
#include<gtkmm/application.h>
#include "src/MainWindow.h"
#include<iostream>

int main(int argc, char *argv[]){
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    app->set_accel_for_action("menu.open", "<Primary>o");

    MainWindow mainWindow{"../cube.obj"};

    return app->run(mainWindow);
}