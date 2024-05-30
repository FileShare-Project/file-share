/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** DeviceList/View.cpp : Implementation of DeviceList/View class
*/

#include "DeviceList/View.hpp"

namespace FileShare::GUI::DeviceList {
    View::View()
        : tgui::ScrollablePanel("DeviceList")
    {
        this->setVerticalScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);

        this->createCurrentDeviceSection();
    }

    View::~View() {}

    tgui::Signal& View::getSignal(tgui::String signalName)
    {
        std::vector<tgui::Signal*> signals = { &onSelectDevice, &onToggleDevice };

        for (auto signal : signals) {
            if (signal->getName() == signalName) {
                return *signal;
            }
        }

        return tgui::Widget::getSignal(signalName);
    }

    void View::setCurrentDevice(std::string& device)
    {
        tgui::Label::Ptr label = this->get<tgui::Label>("CurrentDevice::label");
        label->setText(device);
    }

    void View::setCurrentDeviceConnected(bool connected)
    {
        tgui::Picture::Ptr picture = this->get<tgui::Picture>("CurrentDevice::picture");
        picture->getRenderer()->setTexture(connected
            ? "assets/images/button-green.png"
            : "assets/images/button-red.png"
        );
    }

    void View::createCurrentDeviceSection()
    {
        tgui::Panel::Ptr panel = tgui::Panel::create();
        panel->setHeight(64);
        panel->setAutoLayout(tgui::AutoLayout::Top);
        panel->getRenderer()->setPadding({ 8, 8 });
        panel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
        panel->onClick([this]() {
            this->onToggleDevice.emit(this);
        });

        tgui::Picture::Ptr picture = tgui::Picture::create("assets/images/button-red.png", true);
        picture->setWidgetName("CurrentDevice::picture");
        picture->setPosition({ 0, 6 });
        picture->setSize({ 12, 12 });
        panel->add(picture);

        tgui::Label::Ptr title = tgui::Label::create("Current device:");
        title->setPosition({ 20, 0 });
        title->setSize({ "100%", "50%" });
        title->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
        title->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        title->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        panel->add(title);

        tgui::Label::Ptr label = tgui::Label::create();
        label->setPosition({ 0, "50%" });
        label->setSize({ "100%", "50%" });
        label->setWidgetName("CurrentDevice::label");
        label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        panel->add(label);

        this->add(panel);
    }

    void View::createSection(const std::string& title, const std::vector<std::string>& options)
    {
        tgui::Panel::Ptr spacer = tgui::Panel::create();
        spacer->setHeight(8);
        spacer->setAutoLayout(tgui::AutoLayout::Top);
        spacer->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
        this->add(spacer);

        tgui::VerticalLayout::Ptr layout = tgui::VerticalLayout::create();
        layout->setHeight(options.size() * 24 + 32);
        layout->setAutoLayout(tgui::AutoLayout::Top);

        tgui::Label::Ptr labelTitle = tgui::Label::create(title);
        labelTitle->setHeight(32);
        labelTitle->setAutoLayout(tgui::AutoLayout::Top);
        labelTitle->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        labelTitle->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        labelTitle->getRenderer()->setTextStyle(tgui::TextStyle::Bold);
        labelTitle->getRenderer()->setPadding({ 8, 0 });
        layout->add(labelTitle);

        for (ushort i = 0; i < options.size(); i += 1) {
            tgui::Group::Ptr group = tgui::Group::create();
            group->setHeight(24);
            group->setAutoLayout(tgui::AutoLayout::Top);
            group->getRenderer()->setPadding({ 8, 0 });
            layout->add(group);

            tgui::Picture::Ptr picture = tgui::Picture::create("assets/images/button-red.png", true);
            picture->setPosition({ 0, 6 });
            picture->setSize({ 12, 12 });
            group->add(picture);

            tgui::Label::Ptr label = tgui::Label::create(options[i]);
            label->setPosition({ 0, 0 });
            label->setSize({ "100%", "100%" });
            label->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
            label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
            label->getRenderer()->setPadding({ 20, 0 });
            group->add(label);

            tgui::ClickableWidget::Ptr button = tgui::ClickableWidget::create();
            button->setPosition({ 0, 0 });
            button->setSize({ "100%", "100%" });
            button->onClick([=]() {
                this->onSelectDevice.emit(this, options[i]);
            });
            group->add(button);
        }

        this->add(layout);
    }
}
