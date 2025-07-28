/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun May 26 15:18:37 2024 Léo Lhuile
** Last update Mon Jul 28 10:03:12 2025 Léo Lhuile
**
** Foldout.cpp : Implementation of Foldout class
*/

#include "Components/Foldout.hpp"
#include "ThemeManager.hpp"

namespace FileShare::GUI::Components {
    Foldout::Foldout(const char *typeName, bool initRenderer)
        : tgui::Group(typeName, initRenderer)
    {
        this->build();
    }

    tgui::Signal &Foldout::getSignal(tgui::String signalName) {
        std::vector<tgui::Signal *> signals = {&this->onClick, &this->onOpen, &this->onClose};

        for (auto signal : signals) {
            if (signal->getName() == signalName) {
                return *signal;
            }
        }

        return tgui::Widget::getSignal(signalName);
    }

    void Foldout::addToContent(const tgui::Widget::Ptr &item, const tgui::String &widgetName) {
        this->content->add(item, widgetName);
        this->updateContentHeight();
    }

    bool Foldout::removeFromContent(const Widget::Ptr &widget) {
        if (!this->content) {
            return false;
        }

        auto success = this->content->remove(widget);

        if (success) {
            this->updateContentHeight();
        }

        return success;
    }

    void Foldout::build() {
        this->buildHeader();

        this->content = Components::List::create();
        this->content->setPosition({0, this->header->getFullSize().y});
        this->content->setWidth("100%");
        this->content->setVisible(false);
        this->add(this->content);

        this->setHeight(this->header->getFullSize().y);
        this->button->onClick([this]() {
            this->isOpened = !this->isOpened;
            this->toggleContent();
            this->onClick.emit(this);
        });
    }

    void Foldout::buildHeader() {
        this->header = tgui::Group::create();
        this->header->setPosition({0, 0});
        this->header->setWidth("100%");
        this->add(this->header);

        this->button = Components::Button::create();
        this->button->setWidth("100%");
        this->button->setAlignment(Components::Button::Alignment::Left);
        this->button->setType(Components::Button::Type::Soft);
        this->button->setGhost(true);
        this->header->add(this->button);
        this->header->setHeight(this->button->getFullSize().y);

        this->icon = tgui::Picture::create(tgui::String(ThemeManager::getThemedAssetPath("arrow_down.svg")));
        this->icon->setSize({24, 24});
        this->icon->setPosition({"100% - width", "50% - height / 2"});
        this->icon->setRotation(180, {0.5f, 0.5f});
        this->icon->setIgnoreMouseEvents(true);
        this->header->add(this->icon);
    }

    void Foldout::toggleContent(bool useAnim) {
        if (this->button->isActive() != this->isOpened) {
            this->button->setActive(this->isOpened);
        }

        if (!this->isFoldable()) {
            return;
        }

        if (this->closeAnimationSignalId != -1) {
            this->onAnimationFinish.disconnect(this->closeAnimationSignalId);
            this->closeAnimationSignalId = -1;
        }

        this->icon->setRotation(this->isOpened ? 0 : 180, {0.5f, 0.5f});
        this->finishAllAnimations();
        if (this->isOpened) {
            this->content->setVisible(true);
            if (useAnim) {
                this->resizeWithAnimation({this->getSize().x, header->getFullSize().y + this->content->getFullSize().y}, sf::milliseconds(200));
            } else {
                this->setHeight(header->getFullSize().y + this->content->getFullSize().y);
            }
            this->onOpen.emit(this);
        } else {
            if (useAnim) {
                this->resizeWithAnimation({this->getSize().x, this->button->getFullSize().y}, sf::milliseconds(200));
                this->closeAnimationSignalId = this->onAnimationFinish([this]() {
                    this->content->setVisible(false);
                    this->onAnimationFinish.disconnect(this->closeAnimationSignalId);
                    this->closeAnimationSignalId = -1;
                });
            } else {
                this->setHeight(this->button->getFullSize().y);
                this->content->setVisible(false);
            }
            this->onClose.emit(this);
        }
    }

    void Foldout::updateContentHeight() {
        float maxHeight = 0;
        for (const auto &widget : this->content->getWidgets()) {
            const auto position = widget->getPosition();
            const auto size = widget->getFullSize();
            const auto testMaxHeight = position.y + size.y;

            if (testMaxHeight > maxHeight) {
                maxHeight = testMaxHeight;
            }
        }

        this->content->setHeight(maxHeight);
        if (this->isOpened) {
            this->setHeight(header->getFullSize().y + this->content->getFullSize().y);
        }
    }
}
