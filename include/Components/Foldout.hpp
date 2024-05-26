/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2022 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2022 Léo Lhuile
**
** Foldout.hpp : Class to manage a foldout with icon and extra informations.
*/

#pragma once

#include <TGUI/TGUI.hpp>
#include "Components/Button.hpp"
#include "Components/List.hpp"

namespace FileShare::GUI::Components {
    class Foldout : public tgui::Group {
        public:
            Foldout(const char* typeName = "Components::Foldout", bool initRenderer = true);
            ~Foldout();

            typedef std::shared_ptr<Foldout> Ptr;
			typedef std::shared_ptr<const Foldout> ConstPtr;

			static Foldout::Ptr create() { return std::make_shared<Foldout>(); }
			static Foldout::Ptr copy(Foldout::ConstPtr widget) { return widget ? std::static_pointer_cast<Foldout>(widget->clone()) : nullptr; }

            void addToContent(const tgui::Widget::Ptr &item, const tgui::String &widgetName = "");
            bool removeFromContent(const Widget::Ptr &widget);

            void setText(const tgui::String &text) { this->button->setText(text); }
            const tgui::String &getText() const { return this->button->getText(); }

            void setImage(const tgui::String &image) { this->button->setImage(image); }
            const tgui::Texture &getImage() const { return this->button->getImage(); }

            void open() { this->isOpened = true; this->toggleContent(false); }
            void close() { this->isOpened = false; this->toggleContent(false); }

		protected:
			tgui::Widget::Ptr clone() const override { return std::make_shared<Foldout>(*this); }

        private:
            void build();
            void buildHeader();

            void toggleOpenClose() { this->isOpened = !this->isOpened; this->toggleContent(); }
            void toggleContent(bool useAnim = true);

            void updateContentHeight();

            tgui::Group::Ptr header;
            Components::Button::Ptr button;
            tgui::Picture::Ptr icon;

            Components::List::Ptr content;
            bool isOpened = false;
            int closeAnimationSignalId = -1;
    };
}
