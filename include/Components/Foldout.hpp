/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2024 Léo Lhuile
** Last update Mon Jul 28 10:03:12 2025 Léo Lhuile
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
            Foldout(const char *typeName = "Components::Foldout", bool initRenderer = true);
            ~Foldout() = default;

            typedef std::shared_ptr<Foldout> Ptr;
            typedef std::shared_ptr<const Foldout> ConstPtr;

            static Foldout::Ptr create() { return std::make_shared<Foldout>(); }
            static Foldout::Ptr copy(Foldout::ConstPtr widget) { return widget ? std::static_pointer_cast<Foldout>(widget->clone()) : nullptr; }

            tgui::Signal &getSignal(tgui::String signalName) override;

            tgui::Signal onClick = { "Clicked" };
            tgui::Signal onOpen = { "Opened" };
            tgui::Signal onClose = { "Closed" };

            void addToContent(const tgui::Widget::Ptr &item, const tgui::String &widgetName = "");
            bool removeFromContent(const Widget::Ptr &widget);
            const std::vector<tgui::Widget::Ptr> &getContentWidgets() const { return this->content->getWidgets(); }

            void setButtonSize(Components::Button::Size size) { this->button->setSize(size); }
            Components::Button::Size getButtonSize() const { return this->button->getSize(); }

            void buttonCanBeActive(bool active) { this->button->canBeActive(active, false); }
            bool buttonCanBeActive() const { return this->button->canBeActive(); }

            void setText(const tgui::String &text) { this->button->setText(text); }
            const tgui::String &getText() const { return this->button->getText(); }

            void setImage(const tgui::String &image) { this->button->setImage(image); }
            const tgui::Texture &getImage() const { return this->button->getImage(); }

            void setImageSize(float size) { this->button->setImageSize(size); }
            void setImageSize(const tgui::Vector2f &size) { this->button->setImageSize(size); }
            tgui::Vector2f getImageSize() const { return this->button->getImageSize(); }

            void setFoldable(bool enable) { this->foldable = enable; this->setFoldImageVisible(enable); }
            bool isFoldable() const { return foldable; }

            void setFoldImageVisible(bool visible) { this->icon->setVisible(visible); }
            bool isFoldImageVisible() const { return this->icon->isVisible(); }

            void open(bool useAnim = false) { this->openClose(true, useAnim); }
            void close(bool useAnim = false) { this->openClose(false, useAnim); }
            bool isOpen() const { return this->isOpened; }

        protected:
            tgui::Widget::Ptr clone() const override { return std::make_shared<Foldout>(*this); }

        private:
            void build();
            void buildHeader();

            void openClose(bool isOpened, bool useAnim) { this->isOpened = isOpened; this->toggleContent(useAnim); }
            void toggleContent(bool useAnim = true);

            void updateContentHeight();

            tgui::Group::Ptr header;
            Components::Button::Ptr button;
            tgui::Picture::Ptr icon;

            Components::List::Ptr content;
            bool foldable = true;
            bool isOpened = false;
            int closeAnimationSignalId = -1;
    };
}
