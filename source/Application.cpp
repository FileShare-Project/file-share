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
#include <TGUI/TGUI.hpp>

namespace FileShare::GUI {
    Application::Application() :
        window(sf::VideoMode(800, 600), "FileShare"),
        gui(window)
    {
        tgui::Panel::Ptr sider = this->buildSider();
        this->gui.add(sider);
        this->gui.add(this->buildSiderButton(sider));

        // tgui::Panel::Ptr content = this->buildContent();
        // content->setPosition({tgui::bindRight(sider), "0"});
        // this->gui.add(content);
    }

    Application::~Application()
    {}

    void Application::loop()
    {
        gui.mainLoop();
    }

    tgui::Panel::Ptr Application::buildSider()
    {
        tgui::ScrollablePanel::Ptr panel = tgui::ScrollablePanel::create();
        panel->setSize({ "20%", "95%" });
        panel->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);

        panel->add(this->buildSiderSection("LAN", { "Sister", "Father" }));
        panel->add(this->buildSiderSection("Own devices", { "My device 1", "My device 2" }));
        panel->add(this->buildSiderSection("Contacts", { "Alexandre", "Bart", "Caroline", "Didier" }));
        return panel;
    }

    tgui::Panel::Ptr Application::buildSiderSection(std::string name, std::vector<std::string> options)
    {
        int optionHeight = 32;
        int optionPadding = 8;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        panel->setSize({ "100%", (options.size() + 1) * optionHeight });
        panel->setAutoLayout(tgui::AutoLayout::Top);

        tgui::Label::Ptr labelName = tgui::Label::create(name);
        labelName->setSize({ "100% - " + tgui::String::fromNumber(2 * optionPadding), optionHeight - 2 * optionPadding });
        labelName->setPosition({ optionPadding, optionPadding });
        labelName->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
        labelName->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        panel->add(labelName);

        for (ushort i = 0; i < options.size(); i += 1) {
            tgui::Group::Ptr groupOption = tgui::Group::create();
            groupOption->setPosition({ 0, (i + 1) * optionHeight });
            groupOption->setSize({ "100%", optionHeight });
            panel->add(groupOption);

            tgui::Picture::Ptr pictureOption = tgui::Picture::create("assets/images/button-red.png", true);
            pictureOption->setPosition({ optionPadding, optionPadding * 1.5f });
            pictureOption->setSize({ 8, 8 });
            groupOption->add(pictureOption);

            tgui::Label::Ptr labelOption = tgui::Label::create(options[i]);
            labelOption->setPosition({ 24, optionPadding });
            labelOption->setSize({ "100% - " + tgui::String::fromNumber(optionPadding + 24), optionHeight - 2 * optionPadding });
            labelOption->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
            groupOption->add(labelOption);

            tgui::ClickableWidget::Ptr buttonOption = tgui::ClickableWidget::create();
            buttonOption->setPosition({ 0, 0 });
            buttonOption->setSize({ "100%", "100%" });
            buttonOption->onClick([=]() {
                std::cout << "Option " << options[i] << " pressed" << std::endl;
            });
            groupOption->add(buttonOption);
        }

        return panel;
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
