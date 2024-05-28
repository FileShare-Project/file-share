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
#include "Components/InputFileDialog.hpp"

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
        auto applicationSettings = this->createApplicationSettings();
        this->addItem("assets/images/settings_black.svg", "Application settings")->getSignal("Clicked").connect([=]() {
            this->onSettingsClicked.emit(this, applicationSettings);
        });

        auto devicesSettings = this->createDevicesSettings();
        this->addItem("assets/images/settings_device_black.svg", "Devices settings")->getSignal("Clicked").connect([=]() {
            this->onSettingsClicked.emit(this, devicesSettings);
        });

        auto accountSettings = this->createAccountSettings();
        this->addItem("assets/images/account_black.svg", "Account")->getSignal("Clicked").connect([=]() {
            this->onSettingsClicked.emit(this, accountSettings);
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

    tgui::Widget::Ptr View::createApplicationSettings()
    {
        Components::List::Ptr list = Components::List::create();
        list->setAutoLayout(tgui::AutoLayout::Top);
        list->setAutoHeight(true);
        list->setSpaceBetweenItems(24);
        list->getRenderer()->setPadding({6, 6});

        tgui::Label::Ptr label = tgui::Label::create("Application settings");
        label->getRenderer()->setTextColor(tgui::Color::Black);
        label->getRenderer()->setTextSize(20);
        list->add(label);

        return list;
    }

    tgui::Widget::Ptr View::createDevicesSettings()
    {
        Components::List::Ptr list = Components::List::create();
        list->setAutoLayout(tgui::AutoLayout::Top);
        list->setAutoHeight(true);
        list->setSpaceBetweenItems(24);
        list->getRenderer()->setPadding({6, 6});

        // GENERAL
        auto deviceNameInput = tgui::EditBox::create();
        deviceNameInput->setWidth("50%");

        auto downloadFolderInput = Components::InputFileDialog::create();
        downloadFolderInput->setWidth("50%");
        downloadFolderInput->setMode(Components::InputFileDialog::FileMode::Directory);

        auto allowConnectionsInput = tgui::CheckBox::create();
        allowConnectionsInput->setText("Allow connections");        
        allowConnectionsInput->getRenderer()->setTextDistanceRatio(0.5f);

        list->add(this->createSection("General", {
            this->createSectionInput(deviceNameInput, "Device name"),
            this->createSectionInput(downloadFolderInput, "Download folder"),
            this->createSectionInput(allowConnectionsInput)
        }));

        // Virtual folder
        list->add(this->createSection("Virtual folder", {}));

        // Advanced
        auto virtualRootNameInput = tgui::EditBox::create();
        virtualRootNameInput->setWidth("50%");

        auto privateKeyDirectoryInput = Components::InputFileDialog::create();
        privateKeyDirectoryInput->setWidth("50%");
        privateKeyDirectoryInput->setMode(Components::InputFileDialog::FileMode::Directory);

        auto privateKeyNameInput = tgui::EditBox::create();
        privateKeyNameInput->setWidth("50%");

        auto transportModeInput = tgui::ComboBox::create();
        transportModeInput->addItem("TCP");
        transportModeInput->addItem("UDP");
        transportModeInput->addItem("Automatic");
        transportModeInput->setSelectedItem("Automatic");

        list->add(this->createSection("Advanced", {
            this->createSectionInput(virtualRootNameInput, "Virtual root name"),
            this->createSectionInput(privateKeyDirectoryInput, "Private key directory"),
            this->createSectionInput(privateKeyNameInput, "Private key name"),
            this->createSectionInput(transportModeInput, "Transport mode")
        }));

        return list;
    }

    tgui::Widget::Ptr View::createAccountSettings()
    {
        Components::List::Ptr list = Components::List::create();
        list->setAutoLayout(tgui::AutoLayout::Top);
        list->setAutoHeight(true);
        list->setSpaceBetweenItems(24);

        tgui::Label::Ptr label = tgui::Label::create("Account settings");
        label->getRenderer()->setTextColor(tgui::Color::Black);
        label->getRenderer()->setTextSize(20);
        list->add(label);

        return list;
    }

    tgui::Widget::Ptr View::createSection(const tgui::String &title, std::vector<tgui::Widget::Ptr> contents)
    {
        Components::List::Ptr list = Components::List::create();
        list->setAutoHeight(true);
        list->setSpaceBetweenItems(6);
        list->getRenderer()->setPadding({6, 6});

        tgui::Label::Ptr label = tgui::Label::create(title);
        label->getRenderer()->setTextColor(tgui::Color::Black);
        label->getRenderer()->setTextSize(18);
        label->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
        list->add(label);

        Components::List::Ptr listContent = Components::List::create();
        listContent->setAutoHeight(true);
        listContent->setSpaceBetweenItems(12);
        for (auto content : contents) {
            listContent->add(content);
        }
        list->add(listContent);

        return list;
    }

    tgui::Widget::Ptr View::createSectionInput(const tgui::Widget::Ptr &input, const tgui::String &label)
    {
        tgui::Group::Ptr group = tgui::Group::create();

        if (!label.empty()) {
            tgui::Label::Ptr labelWidget = tgui::Label::create(label);
            labelWidget->getRenderer()->setTextColor(tgui::Color::Black);
            labelWidget->getRenderer()->setTextSize(14);
            group->add(labelWidget);

            input->setPosition(0, labelWidget->getSize().y);
            group->setHeight(labelWidget->getSize().y + input->getSize().y);
        } else {
            group->setHeight(input->getSize().y);
        }

        group->add(input);
        return group;
    }
}
