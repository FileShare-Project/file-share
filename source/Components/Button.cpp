/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun May 26 15:18:37 2024 Léo Lhuile
** Last update Sat Jul 26 21:26:53 2025 Léo Lhuile
**
** Button.cpp : Implementation of Button class
*/

#include "Components/Button.hpp"
#include "Debug.hpp"

namespace FileShare::GUI::Components {
    Button::Button(const char *typeName, bool initRenderer)
        : tgui::BitmapButton(typeName, initRenderer) {
        for (const auto &component : this->m_backgroundComponent->getComponents()) {
            auto imageComponent = std::dynamic_pointer_cast<tgui::priv::dev::ImageComponent>(component);
            if (imageComponent) {
                this->imageComponent = imageComponent;
                break;
            }
        }

        this->setSize(Size::Normal);
        this->updateAligment();
        this->updateStyle();
        this->onClick(&Button::setFocused, this, false);
    }

    Button::~Button() {}

    void Button::canBeActive(bool active, bool automatic) {
        if (active == this->isActive()) {
            return;
        }

        if (active) {
            this->activeSignalId = automatic ? this->onClick(&Button::toggleActive, this) : 1;
        } else {
            if (this->activeSignalId > 1) {
                this->onClick.disconnect(this->activeSignalId);
            }

            this->activeSignalId = 0;
        }
    }

    void Button::setSize(Size size) {
        this->size = size;
        if (size == Size::Small) {
            this->setHeight(24);
            this->setImageSize(12);
            this->setTextSize(10);
        } else if (size == Size::Normal) {
            this->setHeight(36);
            this->setImageSize(15);
            this->setTextSize(12);
        } else if (size == Size::Large) {
            this->setHeight(48);
            this->setImageSize(18);
            this->setTextSize(15);
        }
    }

    void Button::setSize(const tgui::Layout2d &size) {
        if (this->getImageScaling() >= 0) {
            tgui::BitmapButton::setSize(size);
            return;
        }

        tgui::Button::setSize(size);
        this->recalculateGlyphSize();
    }

    void Button::updateSize() {
        if (this->getImageScaling() >= 0) {
            tgui::BitmapButton::updateSize();
            return;
        }

        if (!this->imageComponent->isVisible()) {
            tgui::Button::updateSize();
            return;
        }

        auto size = tgui::BitmapButton::getSize();
        if (this->m_autoSize) {
            this->m_autoLayout = tgui::AutoLayout::Manual;

            const tgui::Outline &borders = this->m_backgroundComponent->getBorders();

            Widget::setSize({
                size.x,
                std::round(this->m_textComponent->getLineHeight() * 1.25f) + borders.getTop() + borders.getBottom()
            });

            this->recalculateGlyphSize();

            if (this->m_string.empty()) {
                const tgui::Vector2f innerSize = this->m_backgroundComponent->getClientSize();

                Widget::setSize({
                    this->imageComponent->getSize().x + (innerSize.y - this->imageComponent->getSize().y) + borders.getLeft() + borders.getRight(),
                    size.y
                });
            } else {
                const float spaceAroundImageAndText = this->m_textComponent->getLineHeight();

                Widget::setSize({
                    this->imageComponent->getSize().x + this->distanceBetweenTextAndImage + this->m_textComponent->getSize().x + spaceAroundImageAndText + borders.getLeft() + borders.getRight(),
                    size.y
                });
            }

            this->m_backgroundComponent->setSize(size);
        } else {
            this->m_backgroundComponent->setSize(size);
            this->recalculateGlyphSize();
        }

        this->updateTextPosition();
    }

    void Button::updateTextPosition() {
        if (this->getImageScaling() >= 0) {
            tgui::BitmapButton::updateTextPosition();
            return;
        }

        const tgui::Vector2f innerSize = this->m_backgroundComponent->getClientSize();
        tgui::Vector2f contentSize = this->m_textComponent->getSize();
        if (!this->m_string.empty()) {
            contentSize.x += this->distanceBetweenTextAndImage + this->imageComponent->getSize().x;
            contentSize.y = std::max(contentSize.y, this->imageComponent->getSize().y);
        }

        this->m_textPosition.x.updateParentSize(innerSize.x);
        this->m_textPosition.y.updateParentSize(innerSize.y);

        auto offsetX = 6 + this->offset;
        if (this->alignment == Alignment::Center) offsetX = 0;
        if (this->alignment == Alignment::Right) offsetX = -offsetX;

        if (!this->m_string.empty()) {
            this->imageComponent->setPosition({
                this->m_textPosition.x.getValue() - this->m_textOrigin.x * contentSize.x + offsetX,
                this->m_textPosition.y.getValue() - this->m_textOrigin.y * contentSize.y + (contentSize.y - this->imageComponent->getSize().y) / 2.f
            });
            this->m_textComponent->setPosition({
                this->imageComponent->getPosition().x + this->imageComponent->getSize().x + distanceBetweenTextAndImage,
                this->m_textPosition.y.getValue() - this->m_textOrigin.y * contentSize.y + (contentSize.y - this->m_textComponent->getSize().y) / 2.f
            });
        } else {
            this->m_textComponent->setPosition({
                this->m_textPosition.x.getValue() - this->m_textOrigin.x * contentSize.x + offsetX,
                this->m_textPosition.y.getValue() - this->m_textOrigin.y * contentSize.y + (contentSize.y - this->imageComponent->getSize().y) / 2.f
            });
        }
    }

    void Button::recalculateGlyphSize() {
        if (!this->imageComponent->isVisible()) {
            return;
        }

        auto imageScaling = this->getImageScaling();

        if (imageScaling >= 0) {
            auto imageSize = this->getImage().getImageSize();

            if (imageScaling == 0) {
                this->imageComponent->setSize(tgui::Vector2f{imageSize});
            } else {
                const tgui::Vector2f innerSize = this->m_backgroundComponent->getClientSize();
                this->imageComponent->setSize({
                    (imageScaling * innerSize.y) * imageSize.x / imageSize.y,
                    (imageScaling * innerSize.y)
                });
            }
        } else {
            this->imageComponent->setSize(this->imageSize);
        }
    }

    void Button::updateAligment() {
        if (alignment == Alignment::Left) {
            this->setTextPosition({"0%", "50%"}, {0.f, 0.5f});
        } else if (alignment == Alignment::Center) {
            this->setTextPosition({"50%", "50%"}, {0.5f, 0.5f});
        } else if (alignment == Alignment::Right) {
            this->setTextPosition({"100%", "50%"}, {1.f, 0.5f});
        }
    }

    void Button::updateStyle() {
        static const std::array<std::string, 4> baseNames = {"ButtonPrimary", "ButtonSecondary", "ButtonSoft", "ButtonDanger"};

        std::string themeSection = baseNames[static_cast<int>(type)];
        if (ghost) {
            themeSection += "Ghost";
        }
        if (this->isActive()) {
            themeSection += "Active";
        }

        this->setRenderer(tgui::Theme::getDefault()->getRenderer(themeSection));
    }
}
