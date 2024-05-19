/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** DeviceList.cpp : Implementation of DeviceList class
*/

#include "DeviceList.hpp"
#include <TGUI/TGUI.hpp>

namespace FileShare::GUI {
    DeviceList::DeviceList()
        : tgui::ScrollablePanel("DeviceList")
    {
        this->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);

        this->createCurrentDeviceSection();
        this->createSection("LAN", { "Sister", "Father" });
        this->createSection("Own devices", { "My device 1", "My device 2" });
        this->createSection("Contacts", { "Alexandre", "Bart", "Caroline", "Didier" });
    }

    DeviceList::~DeviceList() {}

    DeviceList::Ptr DeviceList::create()
    {
        return std::make_shared<DeviceList>();
    }

    DeviceList::Ptr DeviceList::copy(DeviceList::ConstPtr widget)
    {
        return widget
            ? std::static_pointer_cast<DeviceList>(widget->clone())
            : nullptr;
    }

    tgui::Widget::Ptr DeviceList::clone() const
    {
        return std::make_shared<DeviceList>(*this);
    }

    void DeviceList::setCurrentDevice(std::string device)
    {
        tgui::Label::Ptr label = this->get<tgui::Label>("CurrentDevice::label");
        label->setText(device);
    }

    void DeviceList::setCurrentDeviceConnected(bool connected)
    {
        this->currentDeviceConnected = connected;

        tgui::Picture::Ptr picture = this->get<tgui::Picture>("CurrentDevice::picture");
        picture->getRenderer()->setTexture(this->currentDeviceConnected
            ? "assets/images/button-green.png"
            : "assets/images/button-red.png"
        );
    }

    void DeviceList::createCurrentDeviceSection()
    {
        tgui::Panel::Ptr panel = tgui::Panel::create();
        panel->setHeight(64);
        panel->setAutoLayout(tgui::AutoLayout::Top);
        panel->getRenderer()->setPadding({ 8, 8 });

        tgui::Picture::Ptr pictureOption = tgui::Picture::create("assets/images/button-red.png", true);
        pictureOption->setWidgetName("CurrentDevice::picture");
        pictureOption->setPosition({ 0, 0 });
        pictureOption->setSize({ 8, 8 });
        panel->add(pictureOption);

        tgui::Label::Ptr title = tgui::Label::create("Current device:");
        title->setPosition({ 16, 0 });
        title->setSize({ "100%", "50%" });
        title->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
        title->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        panel->add(title);

        tgui::Label::Ptr label = tgui::Label::create();
        label->setPosition({ 0, 24 });
        label->setSize({ "100%", "50%" });
        label->setWidgetName("CurrentDevice::label");
        label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
        panel->add(label);

        tgui::ClickableWidget::Ptr buttonOption = tgui::ClickableWidget::create();
        buttonOption->setPosition({ 0, 0 });
        buttonOption->setSize({ "100%", "100%" });
        buttonOption->onClick([=]() {
            this->setCurrentDeviceConnected(!this->currentDeviceConnected);
        });
        panel->add(buttonOption);

        this->add(panel);
    }

    void DeviceList::createSection(std::string name, std::vector<std::string> options)
    {
        int optionHeight = 32;
        int optionPadding = 8;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        panel->setHeight((options.size() + 1) * optionHeight);
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
                this->setCurrentDevice(options[i]);
            });
            groupOption->add(buttonOption);
        }

        this->add(panel);
    }
}
