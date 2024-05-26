/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** Settings/View.cpp : Implementation of Settings/View class
*/

#include "Settings/View.hpp"
#include "Components/Button.hpp"
#include "Components/Foldout.hpp"

namespace FileShare::GUI::Settings {
    View::View(const char* typeName, bool initRenderer)
        : tgui::ScrollablePanel(typeName, initRenderer)
    {
        this->menu = Components::ListMenu::create();
        this->menu->setAutoLayout(tgui::AutoLayout::Top);
        this->menu->setAutoSeparatorsBeforeTitles();
        this->menu->setAutoHeight(true);
        this->add(this->menu);

        this->getRenderer()->setPadding({6, 6});
        this->createSettings();
    }

    View::~View() {}

    tgui::Signal &View::getSignal(tgui::String signalName)
    {
        std::vector<tgui::Signal*> signals = {};

        for (auto signal : signals) {
            if (signal->getName() == signalName) {
                return *signal;
            }
        }

        return tgui::Widget::getSignal(signalName);
    }

    void View::createSettings()
    {
        this->menu->addTitle("Application settings");

        this->menu->addTitle("Devices settings");
        this->menu->addItem("assets/images/settings_black.svg", "General");
        this->menu->addSubItem("Device name");
        this->menu->addSubItem("Download folder");
        this->menu->addSubItem("Allow connections");
        this->menu->addItem("assets/images/virtual_folder_black.svg", "Virtual folder");
        this->menu->addItem("assets/images/advanced_black.svg", "Advanced");
        this->menu->addSubItem("Virtual root name");
        this->menu->addSubItem("Private key directory");
        this->menu->addSubItem("Private key name");
        this->menu->addSubItem("Transport mode");

        this->menu->addTitle("Account");

        auto logoutButton = Components::Button::create();
        logoutButton->setText("Logout");
        logoutButton->setType(Components::Button::Type::Danger);
        logoutButton->onPress([=]() {
            this->getSignal("logout").emit(this); // TODO: Implement signal
        });
        this->menu->add(logoutButton);
    }
}
