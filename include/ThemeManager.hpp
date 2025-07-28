/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Thu Jul 24 15:44:13 2025 Léo Lhuile
** Last update Mon Jul 28 10:03:12 2025 Léo Lhuile
**
** ThemeManager.hpp : Theme management system
*/

#pragma once

#include <string>
#include <TGUI/TGUI.hpp>

#include <CppSockets/OSDetection.hpp>
#ifdef OS_WINDOWS
    #include <windows.h>
    #include <winreg.h>
#elif defined(OS_APPLE)
    #include <CoreFoundation/CoreFoundation.h>
#elif defined(OS_LINUX)
    #include <cstdlib>
    #include <fstream>
    #include <algorithm>
#endif

namespace FileShare::GUI {
    class ThemeManager {
        public:
            static ThemeManager &getInstance();

            static void setTheme(const std::string &theme);
            static std::string getCurrentTheme() { return getInstance().currentTheme; }
            static std::string getThemedAssetPath(const std::string &filename) { return "assets/images/" + getInstance().currentTheme + "/" + filename; }

        private:
            ThemeManager() = default;
            ~ThemeManager() = default;
            ThemeManager(const ThemeManager&) = delete;
            ThemeManager &operator=(const ThemeManager&) = delete;

            static std::string detectSystemTheme();
            void initializeTheme();

            std::string currentTheme = "system";
        };
}
