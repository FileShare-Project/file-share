/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2024 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2024 Léo Lhuile
**
** ListMenu.hpp : Class to manage a menu rendered as a list.
*/

#pragma once

#include <TGUI/TGUI.hpp>
#include "List.hpp"
#include "Components/Foldout.hpp"

namespace FileShare::GUI::Components {
    class ListMenu : public List {
        public:
            enum ActiveMode {
                None,
                Single,
                AlwaysOne,
                AtLeastOne,
                Multiple,
            };

            ListMenu(const char* typeName = "Components::ListMenu", bool initRenderer = true);
            ~ListMenu();

            typedef std::shared_ptr<ListMenu> Ptr;
            typedef std::shared_ptr<const ListMenu> ConstPtr;

            static ListMenu::Ptr create() { return std::make_shared<ListMenu>(); }
            static ListMenu::Ptr copy(ListMenu::ConstPtr widget) { return widget ? std::static_pointer_cast<ListMenu>(widget->clone()) : nullptr; }

            tgui::Signal &getSignal(tgui::String signalName) override;

            tgui::SignalString onMenuClicked = { "MenuClicked" };
            tgui::SignalString onMenuActive = { "MenuActived" };
            tgui::SignalString onMenuInactive = { "MenuInactived" };
            tgui::SignalTyped<const std::vector<tgui::String>&> onSelectionChanged = { "SelectionChanged" };

            void setAutoSeparatorsBeforeTitles(bool enable = true) { this->autoSeparators = enable; }

            void setActiveMode(ActiveMode mode) { this->activeMode = mode; }
            ActiveMode getActiveMode() const { return this->activeMode; }

            tgui::Widget::Ptr addItem(const tgui::String &icon, const tgui::String &title, const tgui::String &widgetName = "");
            tgui::Widget::Ptr addSubItem(const tgui::String &title, const tgui::String &widgetName = "");
            tgui::Widget::Ptr addTitle(const tgui::String &title, const tgui::String &widgetName = "");

        protected:
            tgui::Widget::Ptr clone() const override { return std::make_shared<ListMenu>(*this); }

        private:
            void handleItemClicked(Components::Foldout::Ptr item);
            void handleSubItemClicked(Components::Button::Ptr subItem);
            void emitSelectionChanged();

            bool autoSeparators = false;
            ActiveMode activeMode = ActiveMode::Single;

            Components::Foldout::Ptr currentFoldout;
            std::vector<Components::Foldout::Ptr> activeItems;
            std::vector<Components::Button::Ptr> activeSubItems;
    };
}
