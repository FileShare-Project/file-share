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

namespace FileShare::GUI::Settings {
    View::View(const char* typeName, bool initRenderer)
        : Components::ListMenu(typeName, initRenderer)
    {
        this->setDefaultItemOptions({ .foldable = false });
        this->setAutoLayout(tgui::AutoLayout::Top);
        this->setAutoSeparatorsBeforeTitles();
        this->setAutoHeight(true);
        this->getRenderer()->setPadding({6, 6});

        this->createSettings();
    }

    View::~View() {}

    tgui::Signal &View::getSignal(tgui::String signalName)
    {
        std::vector<tgui::Signal*> signals = { &this->onSettingsClicked };

        for (auto signal : signals) {
            if (signal->getName() == signalName) {
                return *signal;
            }
        }

        return tgui::Widget::getSignal(signalName);
    }

    void View::createSettings()
    {
        tgui::Label::Ptr label = tgui::Label::create("Application settings");
        this->addItem("assets/images/settings_black.svg", "Application settings")->getSignal("Clicked").connect([=]() {
            this->onSettingsClicked.emit(this, label);
        });

        tgui::Label::Ptr label2 = tgui::Label::create("Devices settings");
        this->addItem("assets/images/settings_device_black.svg", "Devices settings")->getSignal("Clicked").connect([=]() {
            this->onSettingsClicked.emit(this, label2);
        });

        tgui::Label::Ptr label3 = tgui::Label::create("Account");
        this->addItem("assets/images/account_black.svg", "Account")->getSignal("Clicked").connect([=]() {
            this->onSettingsClicked.emit(this, label3);
        });

        /*
        this->addItem("assets/images/settings_black.svg", "General");
        this->addSubItem("Device name");
        this->addSubItem("Download folder");
        this->addSubItem("Allow connections");
        this->addItem("assets/images/virtual_folder_black.svg", "Virtual folder");
        this->addItem("assets/images/advanced_black.svg", "Advanced");
        this->addSubItem("Virtual root name");
        this->addSubItem("Private key directory");
        this->addSubItem("Private key name");
        this->addSubItem("Transport mode");
        */

        auto logoutButton = Components::Button::create();
        logoutButton->setText("Logout");
        logoutButton->setType(Components::Button::Type::Danger);
        logoutButton->onPress([=]() {
            this->getSignal("logout").emit(this); // TODO: Implement signal. For now, it's not a bug; it's a feature
        });
        this->add(logoutButton);
    }
}
