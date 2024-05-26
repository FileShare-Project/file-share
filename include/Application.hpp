/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2022 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2022 Léo Lhuile
**
** Application.hpp : Class to manage the application.
*/

#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "DeviceList/Controller.hpp"
#include "Settings/Controller.hpp"

namespace FileShare::GUI {
    #define ANIMATION_DURATION 250

    class Application {
        public:
            Application();
            ~Application();

            void init();
            void loop();

        private:
            tgui::Button::Ptr buildSiderButton(tgui::Widget::Ptr sider);
            tgui::Panel::Ptr buildContent();

            sf::RenderWindow window;
            tgui::Gui gui;
            std::unique_ptr<DeviceList::Controller> deviceListController;
            std::unique_ptr<Settings::Controller> settingsController;
    };
}