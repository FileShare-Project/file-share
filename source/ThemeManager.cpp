/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Thu Jul 24 15:44:13 2025 Léo Lhuile
** Last update Thu Jul 24 15:44:13 2025 Léo Lhuile
**
** ThemeManager.cpp : Implementation of ThemeManager class
*/

#include "ThemeManager.hpp"

namespace FileShare::GUI {
    ThemeManager::ThemeManager()
    {
        this->setTheme(this->currentTheme);
    }

    ThemeManager &ThemeManager::getInstance()
    {
        static ThemeManager instance;
        return instance;
    }

    void ThemeManager::setTheme(const std::string &theme)
    {
        this->currentTheme = theme;
        tgui::Theme::setDefault("assets/themes/" + theme + ".txt");
    }
}