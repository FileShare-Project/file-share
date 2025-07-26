/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun May 26 15:18:37 2024 Léo Lhuile
** Last update Sat Jul 26 21:26:53 2025 Léo Lhuile
**
** List.cpp : Implementation of List class
*/

#include <algorithm>
#include "Components/List.hpp"

namespace FileShare::GUI::Components {
    List::List(const char *typeName, bool initRenderer)
        : tgui::Panel(typeName, initRenderer) {
    }

    List::~List() {}

    void List::setSpaceBetweenItems(unsigned int space) {
        this->space = space;
        this->updateItemsPosition();
    }

    void List::setAutoHeight(bool enable) {
        this->autoHeight = enable;
        this->updateHeight();
    }

    void List::add(const tgui::Widget::Ptr &item, const tgui::String &widgetName) {
        tgui::Panel::add(item, widgetName);

        auto widgetIndex = this->getWidgets().size() - 1;

        item->setAutoLayout(tgui::AutoLayout::Manual);
        item->setAutoLayoutUpdateEnabled(false);
        item->setWidth("100%");
        this->onSizeChangeSignals.push_back(
            item->onSizeChange.connect([this, widgetIndex]() {
            this->updateItemsPosition(widgetIndex);
            this->updateHeight();
        })
        );
        this->updateItemsPosition(widgetIndex);
        this->updateHeight();
    }

    bool List::remove(const Widget::Ptr &widget) {
        auto widgets = this->getWidgets();
        auto widgetIndex = std::find(widgets.begin(), widgets.end(), widget) - widgets.begin();

        auto success = tgui::Panel::remove(widget);

        if (success) {
            widget->onSizeChange.disconnect(this->onSizeChangeSignals[widgetIndex]);
            this->onSizeChangeSignals.erase(this->onSizeChangeSignals.begin() + widgetIndex);
            this->updateItemsPosition(widgetIndex - 1);
            this->updateHeight();
        }

        return success;
    }

    void List::addSpacer(unsigned int height) {
        auto spacer = tgui::Panel::create();
        spacer->setHeight(height);
        spacer->getRenderer()->setBackgroundColor(tgui::Color::Transparent);

        this->add(spacer);
    }

    void List::addSeparator(const tgui::Layout2d &size, tgui::Color color) {
        auto separator = tgui::SeparatorLine::create();

        this->add(separator);

        separator->setSize(size);
        separator->setPosition("parent.width / 2 - width / 2", separator->getPosition().y);
        separator->getRenderer()->setColor(color);
    }

    void List::updateItemsPosition(unsigned int from) {
        auto items = this->getWidgets();
        if (from >= items.size()) {
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

    void List::updateHeight() {
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

        auto borders = this->getRenderer()->getBorders();
        height += borders.getTop() + borders.getBottom();

        auto padding = this->getRenderer()->getPadding();
        height += padding.getTop() + padding.getBottom();

        this->setHeight(height);
    }
}
