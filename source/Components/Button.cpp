/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** Button.cpp : Implementation of Button class
*/

#include "Components/Button.hpp"
#include "Debug.hpp"

namespace FileShare::GUI::Components {
    Button::Button(const char* typeName, bool initRenderer)
        : tgui::BitmapButton(typeName, initRenderer)
    {
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

    void Button::canBeActive(bool active, bool automatic)
    {
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

    void Button::setSize(Size size)
    {
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

    void Button::setSize(const tgui::Layout2d& size)
    {
        if (this->getImageScaling() >= 0) {
            tgui::BitmapButton::setSize(size);
            return;
        }

        tgui::Button::setSize(size);
        this->recalculateGlyphSize();
    }

    void Button::updateSize()
    {
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

            if (this->m_string.empty())
            {
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

    void Button::updateTextPosition()
    {
        if (this->getImageScaling() >= 0) {
            tgui::BitmapButton::updateTextPosition();
            return;
        }

        const tgui::Vector2f innerSize = this->m_backgroundComponent->getClientSize();
        tgui::Vector2f contentSize = this->m_textComponent->getSize();
        if (!this->m_string.empty())
        {
            contentSize.x += this->distanceBetweenTextAndImage + this->imageComponent->getSize().x;
            contentSize.y = std::max(contentSize.y, this->imageComponent->getSize().y);
        }

        this->m_textPosition.x.updateParentSize(innerSize.x);
        this->m_textPosition.y.updateParentSize(innerSize.y);

        auto offsetX = 6+ this->offset;
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

    void Button::recalculateGlyphSize()
    {
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
            this->setTextPosition({ "0%", "50%" }, { 0.f, 0.5f });
        } else if (alignment == Alignment::Center) {
            this->setTextPosition({ "50%", "50%" }, { 0.5f, 0.5f });
        } else if (alignment == Alignment::Right) {
            this->setTextPosition({ "100%", "50%" }, { 1.f, 0.5f });
        }
    }

    void Button::updateStyle() {
        auto renderer = this->getRenderer();
        renderer->setRoundedBorderRadius(4.f);

        if (type == Type::Primary) {
            renderer->setBorders({1, 1, 1, 1});

            this->backgroundColor = this->ghost ? tgui::Color::White : tgui::Color("#007BFF");
            this->textColor = this->ghost ? tgui::Color("#007BFF") : tgui::Color::White;
            this->borderColor = tgui::Color("#0056b3");

            this->backgroundColorActive = this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#0056b3");
            this->textColorActive = this->ghost ? tgui::Color("#0056b3") : tgui::Color::White;
            this->borderColorActive = tgui::Color("#003f7f");

            renderer->setBackgroundColorFocused(this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#0056b3"));
            renderer->setTextColorFocused(this->ghost ? tgui::Color("#0056b3") : tgui::Color::White);
            renderer->setBorderColorFocused(tgui::Color("#003f7f"));

            renderer->setBackgroundColorHover(this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#0069d9"));
            renderer->setTextColorHover(this->ghost ? tgui::Color("#0069d9") : tgui::Color::White);
            renderer->setBorderColorHover(tgui::Color("#004c91"));

            renderer->setBackgroundColorDown(this->ghost ? tgui::Color("#CCCCCC") : tgui::Color("#004085"));
            renderer->setTextColorDown(this->ghost ? tgui::Color("#004085") : tgui::Color::White);
            renderer->setBorderColorDown(tgui::Color("#002752"));

            renderer->setBackgroundColorDisabled(this->ghost ? tgui::Color("#E0E0E0") : tgui::Color("#7DA2BF"));
            renderer->setTextColorDisabled(this->ghost ? tgui::Color("#525252") : tgui::Color("#525252"));
            renderer->setBorderColorDisabled(tgui::Color("#A1A1A1"));
        } else if (type == Type::Secondary) {
            renderer->setBorders({1, 1, 1, 1});

            this->backgroundColor = this->ghost ? tgui::Color::White : tgui::Color("#6C757D");
            this->textColor = this->ghost ? tgui::Color("#6C757D") : tgui::Color::White;
            this->borderColor = tgui::Color("#545B62");

            this->backgroundColorActive = this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#5A6268");
            this->textColorActive = this->ghost ? tgui::Color("#5A6268") : tgui::Color::White;
            this->borderColorActive = tgui::Color("#3A3F44");

            renderer->setBackgroundColorFocused(this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#5A6268"));
            renderer->setTextColorFocused(this->ghost ? tgui::Color("#5A6268") : tgui::Color::White);
            renderer->setBorderColorFocused(tgui::Color("#3A3F44"));

            renderer->setBackgroundColorHover(this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#545B62"));
            renderer->setTextColorHover(this->ghost ? tgui::Color("#545B62") : tgui::Color::White);
            renderer->setBorderColorHover(tgui::Color("#3A3F44"));

            renderer->setBackgroundColorDown(this->ghost ? tgui::Color("#CCCCCC") : tgui::Color("#3A3F44"));
            renderer->setTextColorDown(this->ghost ? tgui::Color("#3A3F44") : tgui::Color::White);
            renderer->setBorderColorDown(tgui::Color("#1A1C1F"));

            renderer->setBackgroundColorDisabled(this->ghost ? tgui::Color("#E0E0E0") : tgui::Color("#B0B0B0"));
            renderer->setTextColorDisabled(this->ghost ? tgui::Color("#525252") : tgui::Color("#525252"));
            renderer->setBorderColorDisabled(tgui::Color("#A1A1A1"));
        } else if (type == Type::Soft) {
            renderer->setBorders({0, 0, 0, 0});

            this->backgroundColor = this->ghost ? tgui::Color::Transparent : tgui::Color("#F9F9F9");
            this->textColor = tgui::Color("#333333");
            this->borderColor = tgui::Color("#CCCCCC");

            this->backgroundColorActive = this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#F9F9F9");
            this->textColorActive = tgui::Color("#333333");
            this->borderColorActive = tgui::Color("#CCCCCC");

            renderer->setBackgroundColorHover(tgui::Color("#DDDDDD"));
            renderer->setTextColorHover(tgui::Color("#333333"));
            renderer->setBorderColorHover(tgui::Color("#CCCCCC"));

            renderer->setBackgroundColorDown(tgui::Color("#CCCCCC"));
            renderer->setTextColorDown(tgui::Color("#333333"));
            renderer->setBorderColorDown(tgui::Color("#CCCCCC"));

            renderer->setBackgroundColorFocused(tgui::Color("#EFEFEF"));
            renderer->setTextColorFocused(tgui::Color("#333333"));
            renderer->setBorderColorFocused(tgui::Color("#CCCCCC"));

            renderer->setBackgroundColorDisabled(tgui::Color("#E0E0E0"));
            renderer->setTextColorDisabled(tgui::Color("#525252"));
            renderer->setBorderColorDisabled(tgui::Color("#A1A1A1"));
        } else if (type == Type::Danger) {
            renderer->setBorders({1, 1, 1, 1});

            this->backgroundColor = this->ghost ? tgui::Color::White : tgui::Color("#DC3545");
            this->textColor = this->ghost ? tgui::Color("#DC3545") : tgui::Color::White;
            this->borderColor = tgui::Color("#C82333");

            this->backgroundColorActive = this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#DC3545");
            this->textColorActive = this->ghost ? tgui::Color("#DC3545") : tgui::Color::White;
            this->borderColorActive = tgui::Color("#8B1A25");

            renderer->setBackgroundColorFocused(this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#B02A37"));
            renderer->setTextColorFocused(this->ghost ? tgui::Color("#B02A37") : tgui::Color::White);
            renderer->setBorderColorFocused(tgui::Color("#8B1A25"));

            renderer->setBackgroundColorHover(this->ghost ? tgui::Color("#E8E8E8") : tgui::Color("#C82333"));
            renderer->setTextColorHover(this->ghost ? tgui::Color("#C82333") : tgui::Color::White);
            renderer->setBorderColorHover(tgui::Color("#A91D2F"));

            renderer->setBackgroundColorDown(this->ghost ? tgui::Color("#CCCCCC") : tgui::Color("#9B1C28"));
            renderer->setTextColorDown(this->ghost ? tgui::Color("#9B1C28") : tgui::Color::White);
            renderer->setBorderColorDown(tgui::Color("#7A1823"));

            renderer->setBackgroundColorDisabled(this->ghost ? tgui::Color("#E0E0E0") : tgui::Color("#D6A8A8"));
            renderer->setTextColorDisabled(this->ghost ? tgui::Color("#525252") : tgui::Color("#525252"));
            renderer->setBorderColorDisabled(tgui::Color("#A1A1A1"));
        }

        this->updateActiveStyle();
    }

    void Button::updateActiveStyle()
    {
        auto renderer = this->getRenderer();
        auto isActive = this->isActive();

        renderer->setBackgroundColor(isActive ? this->backgroundColorActive : this->backgroundColor);
        renderer->setTextColor(isActive ? this->textColorActive : this->textColor);
        renderer->setBorderColor(isActive ? this->borderColorActive : this->borderColor);
    }
}
