/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun May 26 15:18:37 2024 Léo Lhuile
** Last update Thu Jul 24 15:44:13 2025 Léo Lhuile
**
** Settings/View.cpp : Implementation of Settings/View class
*/

#include "Settings/View.hpp"
#include "Components/Button.hpp"
#include "Components/InputFileDialog.hpp"
#include "ThemeManager.hpp"

namespace FileShare::GUI::Settings {
    View::View(const char* typeName, bool initRenderer)
        : Components::ListMenu(typeName, initRenderer)
    {
        this->setActiveMode(Components::ListMenu::ActiveMode::AlwaysOne);
        this->setAutoLayout(tgui::AutoLayout::Top);
        this->setAutoSeparatorsBeforeTitles();
        this->setAutoHeight(true);
        this->getRenderer()->setPadding({ 6, 6 });
        this->onSelectionChanged.connect(&View::handleMenuSelectionChanged, this);

        this->createSettings();
    }

    View::~View() {}

    tgui::Signal &View::getSignal(tgui::String signalName)
    {
        std::vector<tgui::Signal*> signals = { &this->onMenuChange };

        for (auto signal : signals) {
            if (signal->getName() == signalName) {
                return *signal;
            }
        }

        return tgui::Widget::getSignal(signalName);
    }

    void View::createSettings()
    {
        auto iconThemeSuffix = ThemeManager::getInstance().getIconThemeSuffix();

        this->menuItems["Application settings"] = this->createApplicationSettings();
        this->addItem("assets/images/settings" + iconThemeSuffix + ".svg", "Application settings");

        this->addItem("assets/images/settings_device" + iconThemeSuffix + ".svg", "Devices settings");
        this->menuItems["General"] = this->createDevicesGeneralSettings();
        this->addSubItem("General");
        this->menuItems["Virtual folder"] = this->createDevicesVirtualFolderSettings();
        this->addSubItem("Virtual folder");
        this->menuItems["Advanced"] = this->createDevicesAdvancedSettings();
        this->addSubItem("Advanced");

        this->menuItems["Account"] = this->createAccountSettings();
        this->addItem("assets/images/account" + iconThemeSuffix + ".svg", "Account");

        auto logoutButton = Components::Button::create();
        logoutButton->setText("Logout");
        logoutButton->setType(Components::Button::Type::Danger);
        logoutButton->onPress([this]() {
            this->getSignal("logout").emit(this); // TODO: Implement signal. For now, it's not a bug; it's a feature
            });
        this->add(logoutButton);
    }

    tgui::Widget::Ptr View::createApplicationSettings()
    {
        return this->createSection("Application settings", {});
    }

    tgui::Widget::Ptr View::createDevicesGeneralSettings()
    {
        auto deviceNameInput = tgui::EditBox::create();
        deviceNameInput->setWidth("50%");

        auto downloadFolderInput = Components::InputFileDialog::create();
        downloadFolderInput->setWidth("50%");
        downloadFolderInput->setMode(Components::InputFileDialog::FileMode::Directory);

        auto allowConnectionsInput = tgui::CheckBox::create();
        allowConnectionsInput->setText("Allow connections");
        allowConnectionsInput->getRenderer()->setTextDistanceRatio(0.5f);

        return this->createSection("Devices settings - General", {
            this->createSectionInput(deviceNameInput, "Device name"),
            this->createSectionInput(downloadFolderInput, "Download folder"),
            this->createSectionInput(allowConnectionsInput)
        });
    }

    tgui::Widget::Ptr View::createDevicesVirtualFolderSettings()
    {
        return this->createSection("Devices settings - Virtual folder", {});
    }

    tgui::Widget::Ptr View::createDevicesAdvancedSettings()
    {
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

        return this->createSection("Devices settings - Advanced", {
            this->createSectionInput(virtualRootNameInput, "Virtual root name"),
            this->createSectionInput(privateKeyDirectoryInput, "Private key directory"),
            this->createSectionInput(privateKeyNameInput, "Private key name"),
            this->createSectionInput(transportModeInput, "Transport mode")
        });
    }

    tgui::Widget::Ptr View::createAccountSettings()
    {
        return this->createSection("Account settings", {});
    }

    tgui::Widget::Ptr View::createSection(const tgui::String &title, std::vector<tgui::Widget::Ptr> contents)
    {
        Components::List::Ptr list = Components::List::create();
        list->setAutoLayout(tgui::AutoLayout::Top);
        list->setAutoHeight(true);
        list->setSpaceBetweenItems(6);
        list->getRenderer()->setPadding({ 6, 6 });

        tgui::Label::Ptr label = tgui::Label::create(title);
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

    void View::handleMenuSelectionChanged(const std::vector<tgui::String> &selection)
    {
        this->currentMenuContent = nullptr;

        for (const auto &menuItem : this->menuItems) {
            const tgui::String &menuKey = menuItem.first;
            if (std::find(selection.begin(), selection.end(), menuKey) != selection.end()) {
                this->currentMenuContent = menuItem.second;
                break;
            }
        }

        if (!this->currentMenuContent) {
            throw std::runtime_error("A menu item was selected but not found");
        }

        this->onMenuChange.emit(this, this->currentMenuContent);
    }
}
