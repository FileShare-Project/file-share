/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun May 26 15:18:37 2024 Léo Lhuile
** Last update Sun May 26 15:18:37 2024 Léo Lhuile
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
