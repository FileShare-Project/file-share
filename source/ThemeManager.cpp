/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Thu Jul 24 15:44:13 2025 Léo Lhuile
** Last update Sat Jul 26 21:26:53 2025 Léo Lhuile
**
** ThemeManager.cpp : Implementation of ThemeManager class
*/

#include "ThemeManager.hpp"

namespace FileShare::GUI {
    ThemeManager::ThemeManager() {
        this->initializeTheme();
    }

    ThemeManager &ThemeManager::getInstance() {
        static ThemeManager instance;
        return instance;
    }

    void ThemeManager::setTheme(const std::string &theme) {
        auto &instance = getInstance();
        std::string actualTheme = theme;

        if (theme == "system") {
            actualTheme = detectSystemTheme();
        }

        instance.currentTheme = actualTheme;
        tgui::Theme::setDefault("assets/themes/" + actualTheme + ".txt");
    }

    std::string ThemeManager::detectSystemTheme() {
#ifdef _WIN32
        // Windows: Check registry for dark mode
        HKEY hKey;
        DWORD dwType = REG_DWORD;
        DWORD dwSize = sizeof(DWORD);
        DWORD dwValue = 0;

        if (RegOpenKeyExA(HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            if (RegQueryValueExA(hKey, "AppsUseLightTheme", nullptr, &dwType,
                (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS) {
                RegCloseKey(hKey);
                return (dwValue == 0) ? "dark" : "light";
            }
            RegCloseKey(hKey);
        }
        return "dark";

#elif __APPLE__
        // macOS: Check system appearance
        CFStringRef appearanceName = nullptr;
        CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);

        // Try to get the interface style from user defaults
        CFPropertyListRef propertyList = CFPreferencesCopyAppValue(
            CFSTR("AppleInterfaceStyle"),
            kCFPreferencesCurrentApplication
        );

        if (propertyList && CFGetTypeID(propertyList) == CFStringGetTypeID()) {
            appearanceName = (CFStringRef)propertyList;
            if (CFStringCompare(appearanceName, CFSTR("Dark"), 0) == kCFCompareEqualTo) {
                CFRelease(propertyList);
                return "dark";
            }
        }

        if (propertyList) {
            CFRelease(propertyList);
        }
        return "light";

#elif __linux__
        // Try GNOME gsettings
        const char *gsettings_cmd = "gsettings get org.gnome.desktop.interface gtk-theme 2>/dev/null";
        FILE *pipe = popen(gsettings_cmd, "r");
        if (pipe) {
            char buffer[256];
            if (fgets(buffer, sizeof(buffer), pipe)) {
                std::string theme(buffer);
                pclose(pipe);

                std::transform(theme.begin(), theme.end(), theme.begin(), ::tolower);
                if (theme.find("dark") != std::string::npos) {
                    return "dark";
                }
            } else {
                pclose(pipe);
            }
        }

        // Try KDE plasma settings
        const char *kde_cmd = "kreadconfig5 --group General --key ColorScheme 2>/dev/null";
        pipe = popen(kde_cmd, "r");
        if (pipe) {
            char buffer[256];
            if (fgets(buffer, sizeof(buffer), pipe)) {
                std::string scheme(buffer);
                pclose(pipe);
                std::transform(scheme.begin(), scheme.end(), scheme.begin(), ::tolower);
                if (scheme.find("dark") != std::string::npos || scheme.find("breeze dark") != std::string::npos) {
                    return "dark";
                }
            } else {
                pclose(pipe);
            }
        }

        // Check environment variables
        const char *gtk_theme = std::getenv("GTK_THEME");
        if (gtk_theme) {
            std::string theme(gtk_theme);
            std::transform(theme.begin(), theme.end(), theme.begin(), ::tolower);
            if (theme.find("dark") != std::string::npos) {
                return "dark";
            }
        }

        return "light";

#else
        // Unsupported platform - fallback to dark
        return "dark";
#endif
    }

    // TODO: remove after we store the value inside SQLite
    void ThemeManager::initializeTheme() {
        std::string actualTheme = this->currentTheme;

        if (this->currentTheme == "system") {
            actualTheme = detectSystemTheme();
        }

        this->currentTheme = actualTheme;
        tgui::Theme::setDefault("assets/themes/" + actualTheme + ".txt");
    }
}
