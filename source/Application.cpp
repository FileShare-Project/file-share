/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** Application.cpp : Implementation of Application class
*/

#include "Application.hpp"

namespace FileShare::GUI {
    Application::Application()
        : window(sf::VideoMode(800, 600), "FileShare")
        , gui(window)
    {
    }

    Application::~Application() {}

    void Application::init()
    {
        this->deviceListController = std::make_unique<DeviceList::Controller>();
        DeviceList::View::Ptr deviceListView = this->deviceListController->getView();
        deviceListView->setWidth("20%");
        deviceListView->setAutoLayout(tgui::AutoLayout::Leftmost);
        deviceListView->setAutoLayoutUpdateEnabled(true);
        this->gui.add(deviceListView);
        this->gui.add(this->buildSiderButton(deviceListView));

        tgui::Panel::Ptr content = this->buildContent();
        content->setAutoLayout(tgui::AutoLayout::Fill);
        this->gui.add(content);
    }

    void Application::loop()
    {
        gui.mainLoop();
    }

    tgui::Button::Ptr Application::buildSiderButton(tgui::Panel::Ptr sider)
    {
        tgui::Button::Ptr button = tgui::Button::create("X");
        button->setSize({ "20%", "5%" });
        button->setPosition({ "0", "95%" });
        button->onPress([=]() {
            if (sider->isVisible()) {
                sider->moveWithAnimation({ "-width", "0" }, sf::milliseconds(ANIMATION_DURATION));
                button->resizeWithAnimation({ "64", button->getSize().y }, sf::milliseconds(ANIMATION_DURATION));

                std::shared_ptr<unsigned int> signalIdPtr = std::make_shared<unsigned int>(0);
                *signalIdPtr = sider->onAnimationFinish([=]() {
                    sider->setVisible(false);
                    sider->onAnimationFinish.disconnect(*signalIdPtr);
                });
            } else {
                sider->setVisible(true);
                sider->moveWithAnimation({ "0", "0" }, sf::milliseconds(ANIMATION_DURATION));
                button->resizeWithAnimation({ "20%", button->getSize().y }, sf::milliseconds(ANIMATION_DURATION));
            }
        });

        return button;
    }

    tgui::Panel::Ptr Application::buildContent()
    {
        tgui::ScrollablePanel::Ptr panel = tgui::ScrollablePanel::create();
        panel->getRenderer()->setBackgroundColor(sf::Color::Blue);

        tgui::VerticalLayout::Ptr layout = tgui::VerticalLayout::create();
        layout->add(tgui::Label::create("Content"));
        panel->add(layout);

        return panel;
    }
}
