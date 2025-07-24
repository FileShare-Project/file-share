/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Thu Jul 24 15:44:13 2025 Léo Lhuile
** Last update Thu Jul 24 15:44:13 2025 Léo Lhuile
**
** ThemeManager.hpp : Theme management system
*/

#pragma once

#include <string>
#include <TGUI/TGUI.hpp>

namespace FileShare::GUI {
    class ThemeManager {
        public:
            static ThemeManager &getInstance();

            void setTheme(const std::string &theme);
            std::string getCurrentTheme() const { return this->currentTheme; }

            std::string getIconThemeSuffix() const { return this->currentTheme == "light" ? "_black" : "_white"; }

        private:
            ThemeManager();
            ~ThemeManager() = default;
            ThemeManager(const ThemeManager&) = delete;
            ThemeManager &operator=(const ThemeManager&) = delete;

            std::string currentTheme = "dark";
        };
}
