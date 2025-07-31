/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2024 Léo Lhuile
** Last update Mon Jul 28 10:03:12 2025 Léo Lhuile
**
** List.hpp : Class to manage a list and render it horizontally.
*/

#pragma once

#include <TGUI/TGUI.hpp>

namespace FileShare::GUI::Components {
    class List : public tgui::Panel {
        public:
            List(const char *typeName = "Components::List", bool initRenderer = true);
            ~List() = default;

            typedef std::shared_ptr<List> Ptr;
            typedef std::shared_ptr<const List> ConstPtr;

            static List::Ptr create() { return std::make_shared<List>(); }
            static List::Ptr copy(List::ConstPtr widget) { return widget ? std::static_pointer_cast<List>(widget->clone()) : nullptr; }

            void setSpaceBetweenItems(unsigned int space);
            void setAutoHeight(bool enable = true);

            void add(const tgui::Widget::Ptr &item, const tgui::String &widgetName = "") override;
            bool remove(const Widget::Ptr &widget) override;

            void addSpacer(unsigned int height);
            void addSeparator(tgui::Color color = tgui::Color(0, 0, 0, 50)) { this->addSeparator({"90%", 1}, color); }
            void addSeparator(tgui::Layout width, tgui::Layout height, tgui::Color color = tgui::Color(0, 0, 0, 50)) { this->addSeparator({std::move(width), std::move(height)}, color); }
            void addSeparator(const tgui::Layout2d &size, tgui::Color color = tgui::Color(0, 0, 0, 50));

        protected:
            tgui::Widget::Ptr clone() const override { return std::make_shared<List>(*this); }

            void updateItemsPosition(tgui::Widget::Ptr from) { this->updateItemsPosition(std::find(this->getWidgets().begin(), this->getWidgets().end(), from) - this->getWidgets().begin()); }
            void updateItemsPosition(unsigned int from = 0);
            void updateHeight();

        private:
            std::vector<unsigned int> onSizeChangeSignals = {};
            unsigned int space = 0;
            bool autoHeight = false;
    };
}
