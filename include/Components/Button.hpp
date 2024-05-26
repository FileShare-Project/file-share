/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2022 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2022 Léo Lhuile
**
** Button.hpp : Class to manage a button with icon and extra informations.
*/

#pragma once

#include <TGUI/TGUI.hpp>

namespace FileShare::GUI::Components {
    class Button : public tgui::BitmapButton {
        public:
            enum Alignment {
                Left,
                Center,
                Right
            };

            enum Type {
                Primary,
                Secondary,
                Soft,
                Danger,
            };

            Button(const char* typeName = "Components::Button", bool initRenderer = true);
            ~Button();

            typedef std::shared_ptr<Button> Ptr;
			typedef std::shared_ptr<const Button> ConstPtr;

			static Button::Ptr create() { return std::make_shared<Button>(); }
			static Button::Ptr copy(Button::ConstPtr widget) { return widget ? std::static_pointer_cast<Button>(widget->clone()) : nullptr; }

            void setImageSize(float size) { this->imageSize = {size, size}; this->setImageScaling(-1); }
            void setImageSize(const tgui::Vector2f &size) { this->imageSize = size; this->setImageScaling(-1); }
            tgui::Vector2f getImageSize() const { return this->imageSize; }

            void setDistanceBetweenTextAndImage(float distance) { this->distanceBetweenTextAndImage = distance; this->updateSize(); }
            float getDistanceBetweenTextAndImage() const { return this->distanceBetweenTextAndImage; }

            void setAlignment(Alignment alignment) { this->alignment = alignment; this->updateAligment(); }
            Alignment getAlignment() const { return alignment; }

            void setOffset(int offset) { this->offset = offset; this->updateSize(); }
            int getOffset() const { return this->offset; }

            void setType(Type type) { this->type = type; this->updateStyle(); }
            Type getType() const { return type; }

            void setGhost(bool ghost) { this->ghost = ghost; this->updateStyle(); }
            bool isGhost() const { return ghost; }

		protected:
			tgui::Widget::Ptr clone() const override { return std::make_shared<Button>(*this); }

            void setSize(const tgui::Layout2d &size) override;
            void updateSize() override;
            void updateTextPosition() override;
            void recalculateGlyphSize();

        private:
            void updateAligment();
            void updateStyle();

            std::shared_ptr<tgui::priv::dev::ImageComponent> imageComponent;
            tgui::Vector2f imageSize;
            float distanceBetweenTextAndImage = 6;
            Alignment alignment = Alignment::Center;
            int offset = 0;
            Type type = Type::Primary;
            bool ghost = false;
    };
}
