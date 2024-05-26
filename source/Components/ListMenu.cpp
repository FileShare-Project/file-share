/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** ListMenu.cpp : Implementation of ListMenu class
*/

#include "Components/ListMenu.hpp"

namespace FileShare::GUI::Components {
    ListMenu::ListMenu(const char* typeName, bool initRenderer)
        : List(typeName, initRenderer)
    {
    }

    ListMenu::~ListMenu() {}

    tgui::Widget::Ptr ListMenu::addItem(const tgui::String &icon, const tgui::String &title, const ItemOptions &options, const tgui::String &widgetName)
    {
        this->currentFoldout = Components::Foldout::create();
        this->currentFoldout->setText(title);
        this->currentFoldout->setImage(icon);
        this->add(this->currentFoldout, widgetName);
        return this->currentFoldout;
    }

    tgui::Widget::Ptr ListMenu::addSubItem(const tgui::String &title, const SubItemOptions &options, const tgui::String &widgetName)
    {
        if (!this->currentFoldout) {
            return nullptr;
        }

        auto item = Components::Button::create();
        auto itemRenderer = item->getRenderer();
        item->setOffset(18);
        item->setAlignment(Components::Button::Alignment::Left);
        item->setType(Components::Button::Type::Soft);
        item->setGhost(true);
        item->setText(title);
        item->setTextSize(options.textSize);
        itemRenderer->setTextColor(options.textColor);
        this->currentFoldout->addToContent(item, widgetName);

        return item;
    }

    tgui::Widget::Ptr ListMenu::addTitle(const tgui::String &title, const TitleOptions &options, const tgui::String &widgetName)
    {
        this->currentFoldout = nullptr;

        auto label = tgui::Label::create();
        auto labelRenderer = label->getRenderer();
        label->setHeight(24);
        label->setText(title);
        label->setTextSize(options.textSize);
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        labelRenderer->setTextColor(options.textColor);
        this->add(label, widgetName);

        return label;
    }
}
