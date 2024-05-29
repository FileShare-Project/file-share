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
    View::View(const char* typeName, bool initRenderer)
        : Components::ListMenu(typeName, initRenderer)
    {
        this->setAutoLayout(tgui::AutoLayout::Top);
        this->setAutoHeight(true);
        this->getRenderer()->setPadding({6, 6});

        this->createCurrentDeviceSection();
    }

    View::~View() {}

    tgui::Signal &View::getSignal(tgui::String signalName)
    {
        std::vector<tgui::Signal*> signals = { &this->onSelectDevice, &this->onToggleDevice };

        for (auto signal : signals) {
            if (signal->getName() == signalName) {
                return *signal;
            }
        }

        return tgui::Widget::getSignal(signalName);
    }

    void View::setCurrentDevice(std::string &device)
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

    void View::createSection(const std::string &title, const std::vector<std::string> &options)
    {
        this->addTitle(title);
        for (ushort i = 0; i < options.size(); i += 1) {
            auto option = options[i];
            auto item = this->addItem("assets/images/button-red.png", option);

            item->getSignal("Clicked").connect([this, option]() {
                this->onSelectDevice.emit(this, option);
            });
        }
    }
}
