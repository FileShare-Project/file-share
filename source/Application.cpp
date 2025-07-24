/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Thu Dec 28 21:54:14 2023 Léo Lhuile
** Last update Thu Jul 24 15:44:13 2025 Léo Lhuile
**
** Application.cpp : Implementation of Application class
*/

#include "Application.hpp"
#include "Debug.hpp"
#include "ThemeManager.hpp"

namespace FileShare::GUI {
    Application::Application()
        : window(sf::VideoMode(1200, 800), "FileShare")
        , gui(window)
    {
    }

    Application::~Application() {}

    void Application::init()
    {
        auto &themeManager = ThemeManager::getInstance();

        tgui::ScrollablePanel::Ptr content = tgui::ScrollablePanel::create();
        content->setAutoLayout(tgui::AutoLayout::Fill);
        if (themeManager.getCurrentTheme() == "dark") {
            content->getRenderer()->setBackgroundColor(tgui::Color(35, 35, 35));
        }
        content->getRenderer()->setPadding(tgui::Padding(20, 20, 20, 20));
        this->gui.add(content);

        this->deviceListController = std::make_unique<DeviceList::Controller>();
        this->settingsController = std::make_unique<Settings::Controller>();

        tgui::ScrollablePanel::Ptr sider = tgui::ScrollablePanel::create();
        sider->setWidth("25%");
        sider->setAutoLayout(tgui::AutoLayout::Leftmost);
        if (themeManager.getCurrentTheme() == "dark") {
            sider->getRenderer()->setBackgroundColor(tgui::Color(45, 45, 45));
        }
        sider->getRenderer()->setPadding(tgui::Padding(15, 15, 15, 15));
        this->gui.add(sider);

        auto deviceListView = this->deviceListController->getView();
        deviceListView->setAutoLayout(tgui::AutoLayout::Top);
        sider->add(deviceListView);

        auto settingsView = this->settingsController->getView();
        settingsView->setAutoLayout(tgui::AutoLayout::Top);
        settingsView->onMenuChange.connect([=](tgui::Widget::Ptr widget) {
            content->removeAllWidgets();
            content->add(widget);
        });

        Components::Button::Ptr button = Components::Button::create();
        button->setType(Components::Button::Type::Primary);
        button->setText("Settings");
        button->setAutoLayout(tgui::AutoLayout::Bottom);
        button->setSize(Components::Button::Size::Large);
        button->onClick([=]() {
            auto showSettings = settingsView->getParent() == nullptr;

            content->removeAllWidgets();
            if (showSettings) {
                sider->remove(deviceListView);
                sider->add(settingsView);
                content->add(settingsView->getCurrentMenuContent());
            } else {
                sider->remove(settingsView);
                sider->add(deviceListView);
            }
            button->setText(showSettings ? "Back" : "Settings");
        });
        sider->add(button);
    }

    void Application::loop()
    {
        // Debug::debug(this->gui.getContainer());
        gui.mainLoop();
    }

}
