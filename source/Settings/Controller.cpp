/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** Settings/Controller.cpp : Implementation of Settings/Controller class
*/

#include "Settings/Controller.hpp"

namespace FileShare::GUI::Settings {
    Controller::Controller() {
        this->view = View::create();
    }

    Controller::~Controller() {}
}
