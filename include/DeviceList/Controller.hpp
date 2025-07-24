/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2024 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2024 Léo Lhuile
**
** DeviceList/Controller.hpp : Class to manage the device list view and data
*/

#pragma once

#include "View.hpp"

namespace FileShare::GUI::DeviceList {
    class Controller {
        public:
            Controller();
            ~Controller();

            View::Ptr getView() const { return this->view; }

            void handleSelectDevice(const std::string &device);
            void handleToggleDevice();

        private:
            Model model;
            View::Ptr view;
    };
}
