/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Thu May 26 22:29:54 2022 Francois Michaut
** Last update Sat Jul 26 21:26:53 2025 Léo Lhuile
**
** main.cpp : Main file of the application
*/

#include "Application.hpp"

int main() {
    FileShare::GUI::Application app;

    try {
        app.init();
    } catch (const tgui::Exception &e) {
        std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
        return 1;
    }

    app.loop();
    return 0;
}
