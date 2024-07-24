/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Thu Dec 28 21:54:14 2023 Léo Lhuile
** Last update Thu May 30 08:59:00 2024 Léo Lhuile
**
** Application.cpp : Implementation of Application class
*/

#include "Application.hpp"
#include "Debug.hpp"

namespace FileShare::GUI {
    Application::Application()
        : window(sf::VideoMode(800, 600), "FileShare")
        , gui(window)
    {
    }

    Application::~Application() {}

    void Application::init()
    {
        // tgui::Theme::setDefault("assets/themes/Black.txt");

        tgui::ScrollablePanel::Ptr content = tgui::ScrollablePanel::create();
        content->setAutoLayout(tgui::AutoLayout::Fill);
        this->gui.add(content);

        this->deviceListController = std::make_unique<DeviceList::Controller>();
        this->settingsController = std::make_unique<Settings::Controller>();

        tgui::ScrollablePanel::Ptr sider = tgui::ScrollablePanel::create();
        sider->setWidth("20%");
        sider->setAutoLayout(tgui::AutoLayout::Leftmost);
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
        button->setType(Components::Button::Type::Secondary);
        button->setText("Settings");
        button->setAutoLayout(tgui::AutoLayout::Bottom);
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
