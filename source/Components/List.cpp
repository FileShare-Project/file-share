/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** List.cpp : Implementation of List class
*/

#include "Components/List.hpp"

namespace FileShare::GUI::Components {
    List::List(const char* typeName, bool initRenderer)
        : tgui::Panel(typeName, initRenderer)
    {
    }

    List::~List() {}

    void List::setSpaceBetweenItems(unsigned int space)
    {
        this->space = space;
        this->updateItemsPosition();
    }

    void List::setAutoHeight(bool enable)
    {
        this->autoHeight = enable;
        this->updateHeight();
    }

    void List::add(const tgui::Widget::Ptr &item, const tgui::String &widgetName)
    {
        tgui::Panel::add(item, widgetName);
        auto widgetIndex = this->getWidgets().size() - 1;

        item->setWidth("100%");
        item->onSizeChange.connect([this, widgetIndex]() {
            this->updateItemsPosition(widgetIndex);
            this->updateHeight();
        });
        this->updateItemsPosition(widgetIndex);
        this->updateHeight();
    }

    bool List::remove(const Widget::Ptr &widget)
    {
        auto success = tgui::Panel::remove(widget);

        if (success) {
            widget->onSizeChange.disconnectAll(); // TODO: Find another way, some other widgets might be disconnected
            this->updateItemsPosition();
        }

        return success;
    }

    void List::addSpacer(unsigned int height)
    {
        auto spacer = tgui::Panel::create();
        spacer->setHeight(height);
        spacer->getRenderer()->setBackgroundColor(tgui::Color::Transparent);

        this->add(spacer);
    }

    void List::addSeparator(const tgui::Layout2d &size, tgui::Color color)
    {
        auto separator = tgui::SeparatorLine::create();

        this->add(separator);

        separator->setSize(size);
        separator->setPosition("parent.width / 2 - width / 2", separator->getPosition().y);
        separator->getRenderer()->setColor(color);
    }

    void List::updateItemsPosition(unsigned int from)
    {
        auto items = this->getWidgets();
        if (items.size() == 0) {
            return;
        }

        auto y = from > 0
            ? items[from - 1]->getPosition().y + items[from - 1]->getFullSize().y + this->space
            : 0;

        for (unsigned int i = from; i < items.size(); i++) {
            items[i]->setPosition(items[i]->getPosition().x, y);
            y += items[i]->getFullSize().y + this->space;
        }
    }

    void List::updateHeight()
    {
        if (!this->autoHeight) {
            return;
        }

        auto items = this->getWidgets();
        if (items.size() == 0) {
            this->setSize(this->getSize().x, 0);
            return;
        }

        auto lastItem = items[items.size() - 1];
        auto height = lastItem->getPosition().y + lastItem->getFullSize().y;
        this->setSize(this->getSize().x, height);
    }
}