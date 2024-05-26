/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2022 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2022 Léo Lhuile
**
** ListMenu.hpp : Class to manage a menu rendered as a list.
*/

#pragma once

#include <TGUI/TGUI.hpp>
#include "List.hpp"
#include "Components/Foldout.hpp"

namespace FileShare::GUI::Components {
    struct StyleOptions {
        tgui::Color textColor = tgui::Color::Black;
        unsigned int textSize = 12;
    };

    struct IconOptions {
        tgui::Layout2d iconSize = {18, 18};
    };

    struct GroupOptions {
        tgui::Padding padding = {10, 5, 10, 5};
    };

    struct ItemOptions : StyleOptions, IconOptions, GroupOptions {};
    struct SubItemOptions : StyleOptions { SubItemOptions() { textSize = 9; } };
    struct TitleOptions : StyleOptions { TitleOptions() { textSize = 9; } };

    class ListMenu : public List {
        public:
            ListMenu(const char* typeName = "Components::ListMenu", bool initRenderer = true);
            ~ListMenu();

			typedef std::shared_ptr<ListMenu> Ptr;
			typedef std::shared_ptr<const ListMenu> ConstPtr;

			static ListMenu::Ptr create() { return std::make_shared<ListMenu>(); }
			static ListMenu::Ptr copy(ListMenu::ConstPtr widget) { return widget ? std::static_pointer_cast<ListMenu>(widget->clone()) : nullptr; }

            void setAutoSeparatorsBeforeTitles(bool enable = true) { this->autoSeparators = enable; }
            void setDefaultItemOptions(const ItemOptions &options) { this->defaultItemOptions = options; }
            void setDefaultSubItemOptions(const SubItemOptions &options) { this->defaultSubItemOptions = options; }
            void setDefaultTitleOptions(const TitleOptions &options) { this->defaultTitleOptions = options; }

            tgui::Widget::Ptr addItem(const tgui::String &icon, const tgui::String &title, const tgui::String &widgetName = "") { return this->addItem(icon, title, this->defaultItemOptions, widgetName); }
            tgui::Widget::Ptr addItem(const tgui::String &icon, const tgui::String &title, const ItemOptions &options, const tgui::String &widgetName = "");

            tgui::Widget::Ptr addSubItem(const tgui::String &title, const tgui::String &widgetName = "") { return this->addSubItem(title, this->defaultSubItemOptions, widgetName); }
            tgui::Widget::Ptr addSubItem(const tgui::String &title, const SubItemOptions &options, const tgui::String &widgetName = "");

            tgui::Widget::Ptr addTitle(const tgui::String &title, const tgui::String &widgetName = "") { return this->addTitle(title, this->defaultTitleOptions, widgetName); }
            tgui::Widget::Ptr addTitle(const tgui::String &title, const TitleOptions &options, const tgui::String &widgetName = "");

		protected:
			tgui::Widget::Ptr clone() const override { return std::make_shared<ListMenu>(*this); }

        private:
            bool autoSeparators = false;
            ItemOptions defaultItemOptions;
            SubItemOptions defaultSubItemOptions;
            TitleOptions defaultTitleOptions;
            Components::Foldout::Ptr currentFoldout;
    };
}
