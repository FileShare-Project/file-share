/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2024 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2024 Léo Lhuile
**
** Settings/Controller.hpp : Class to manage the settings view and data
*/

#pragma once

#include "View.hpp"

namespace FileShare::GUI::Settings {
    class Controller {
        public:
            Controller();
            ~Controller();

            View::Ptr getView() const { return this->view; }

        private:
            Model model;
            View::Ptr view;
    };
}
