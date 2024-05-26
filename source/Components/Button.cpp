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

// TODO: Don't inherit from BitmapButton in order to set the text and image positions wherever we want and set a real image size instead of a ratio (which can be done too)

namespace FileShare::GUI::Components {
    Button::Button(const char* typeName, bool initRenderer)
        : tgui::BitmapButton(typeName, initRenderer)
    {
        this->updateAligment();
        this->updateStyle();

        this->onClick(&Button::setFocused, this, false);

        this->setHeight(24);
        this->setImageScaling(0.5f);
    }

    Button::~Button() {}

    void Button::updateAligment() {
        if (alignment == Alignment::Left) {
            this->setTextPosition({ 6 + this->offset, "50%" }, { 0.f, 0.5f });
        } else if (alignment == Alignment::Center) {
            this->setTextPosition({ "50%", "50%" }, { 0.5f, 0.5f });
        } else if (alignment == Alignment::Right) {
            this->setTextPosition({ "95%", "50%" }, { 1.f, 0.5f });
        }
    }

    void Button::updateStyle() {
        auto renderer = this->getRenderer();
        renderer->setRoundedBorderRadius(4.f);

        if (type == Type::Primary) {
            renderer->setBackgroundColor(this->ghost ? tgui::Color::White : tgui::Color("#007BFF"));
            renderer->setTextColor(this->ghost ? tgui::Color("#007BFF") : tgui::Color::White);
            renderer->setBorderColor(tgui::Color("#0056b3"));
            renderer->setBorders({1, 1, 1, 1});

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
            renderer->setBackgroundColor(this->ghost ? tgui::Color::White : tgui::Color("#6C757D"));
            renderer->setTextColor(this->ghost ? tgui::Color("#6C757D") : tgui::Color::White);
            renderer->setBorderColor(tgui::Color("#545B62"));
            renderer->setBorders({1, 1, 1, 1});

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
            renderer->setBackgroundColor(this->ghost ? tgui::Color::Transparent : tgui::Color("#F9F9F9"));
            renderer->setTextColor(tgui::Color("#333333"));
            renderer->setBorderColor(tgui::Color("#CCCCCC"));
            renderer->setBorders({0, 0, 0, 0});

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
            renderer->setBackgroundColor(this->ghost ? tgui::Color::White : tgui::Color("#DC3545"));
            renderer->setTextColor(this->ghost ? tgui::Color("#DC3545") : tgui::Color::White);
            renderer->setBorderColor(tgui::Color("#C82333"));
            renderer->setBorders({1, 1, 1, 1});

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
    }
}
