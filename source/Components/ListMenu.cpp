/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun May 26 15:18:37 2024 Léo Lhuile
** Last update Sat Jul 26 21:26:53 2025 Léo Lhuile
**
** ListMenu.cpp : Implementation of ListMenu class
*/

#include "Components/ListMenu.hpp"

namespace FileShare::GUI::Components {
    ListMenu::ListMenu(const char *typeName, bool initRenderer)
        : List(typeName, initRenderer) {
    }

    ListMenu::~ListMenu() {}

    tgui::Signal &ListMenu::getSignal(tgui::String signalName) {
        std::vector<tgui::Signal *> signals = {&this->onMenuClicked, &this->onMenuActive, &this->onMenuInactive, &this->onSelectionChanged};

        for (auto signal : signals) {
            if (signal->getName() == signalName) {
                return *signal;
            }
        }

        return tgui::Widget::getSignal(signalName);
    }

    tgui::Widget::Ptr ListMenu::addItem(const tgui::String &icon, const tgui::String &title, const tgui::String &widgetName) {
        if (this->currentFoldout && this->activeItems.size() == 0 && (this->activeMode == ListMenu::ActiveMode::AlwaysOne || this->activeMode == ListMenu::ActiveMode::AtLeastOne)) {
            this->currentFoldout->open();
            this->activeItems.push_back(this->currentFoldout);
            this->onMenuActive.emit(this, this->currentFoldout->getText());
            for (auto subItem : this->currentFoldout->getContentWidgets()) {
                auto subItemButton = std::dynamic_pointer_cast<Components::Button>(subItem);
                if (subItemButton) {
                    subItemButton->setActive(true);
                    this->activeSubItems.push_back(subItemButton);
                    this->onMenuActive.emit(this, subItemButton->getText());
                    break;
                }
            }

            this->emitSelectionChanged();
        }

        auto item = Components::Foldout::create();
        item->setText(title);
        item->setImage(icon);
        item->setFoldable(false);
        if (this->activeMode != ListMenu::ActiveMode::None) {
            item->buttonCanBeActive(true);
            item->onClick.connect([this, item]() { this->handleItemClicked(item); });
        }
        item->onClick.connect([this, title]() { this->onMenuClicked.emit(this, title); });
        this->add(item, widgetName);

        this->currentFoldout = item;

        return item;
    }

    tgui::Widget::Ptr ListMenu::addSubItem(const tgui::String &title, const tgui::String &widgetName) {
        if (!this->currentFoldout) {
            throw std::runtime_error("No foldout to add sub item to");
        }

        auto subItem = Components::Button::create();
        subItem->setSize(Components::Button::Size::Small);
        subItem->setOffset(15);
        subItem->setAlignment(Components::Button::Alignment::Left);
        subItem->setType(Components::Button::Type::Soft);
        subItem->setGhost(true);
        subItem->setText(title);
        if (this->activeMode != ListMenu::ActiveMode::None) {
            subItem->canBeActive(true);
            subItem->onClick.connect([this, subItem]() { this->handleSubItemClicked(subItem); });
        }
        subItem->onClick.connect([this, title]() { this->onMenuClicked.emit(this, title); });
        this->currentFoldout->addToContent(subItem, widgetName);
        this->currentFoldout->setFoldable(true);
        this->currentFoldout->setFoldImageVisible(false);
        // TODO: validate the first sub item if needed

        return subItem;
    }

    tgui::Widget::Ptr ListMenu::addTitle(const tgui::String &title, const tgui::String &widgetName) {
        if (this->autoSeparators && this->getWidgets().size() > 0) {
            this->addSpacer(6);
            this->addSeparator();
            this->addSpacer(6);
        }

        this->currentFoldout = nullptr;

        auto label = tgui::Label::create();
        label->setHeight(24);
        label->setText(title);
        label->setTextSize(16);
        label->setVerticalAlignment(tgui::VerticalAlignment::Center);
        this->add(label, widgetName);

        return label;
    }

    void ListMenu::handleItemClicked(Components::Foldout::Ptr item) {
        if (this->activeMode == ListMenu::ActiveMode::None) {
            return;
        }

        if (item->isOpen()) {
            if (this->activeItems.size() > 0 && (this->activeMode == ListMenu::ActiveMode::Single || this->activeMode == ListMenu::ActiveMode::AlwaysOne)) {
                for (auto activeItem : this->activeItems) {
                    activeItem->close(true);
                    this->onMenuInactive.emit(this, activeItem->getText());
                }

                this->activeItems.clear();

                for (auto subItem : this->activeSubItems) {
                    subItem->setActive(false);
                    this->onMenuInactive.emit(this, subItem->getText());
                }
                this->activeSubItems.clear();
            }
        } else {
            if (this->activeItems.size() <= 1 && (this->activeMode == ListMenu::ActiveMode::AlwaysOne || this->activeMode == ListMenu::ActiveMode::AtLeastOne)) {
                item->open();
                return;
            }
        }

        if (item->isOpen()) {
            this->activeItems.push_back(item);
            this->onMenuActive.emit(this, item->getText());
            if (this->activeMode == ListMenu::ActiveMode::AlwaysOne || this->activeMode == ListMenu::ActiveMode::AtLeastOne) {
                for (auto subItem : item->getContentWidgets()) {
                    auto subItemButton = std::dynamic_pointer_cast<Components::Button>(subItem);

                    if (subItemButton) {
                        subItemButton->setActive(true);
                        this->activeSubItems.push_back(subItemButton);
                        this->onMenuActive.emit(this, subItemButton->getText());
                        break;
                    }
                }
            }
        } else {
            this->activeItems.erase(std::remove(this->activeItems.begin(), this->activeItems.end(), item), this->activeItems.end());
            this->onMenuInactive.emit(this, item->getText());
            for (auto subItem : item->getContentWidgets()) {
                auto subItemButton = std::dynamic_pointer_cast<Components::Button>(subItem);

                if (subItemButton) {
                    subItemButton->setActive(false);
                    this->activeSubItems.erase(std::remove(this->activeSubItems.begin(), this->activeSubItems.end(), subItemButton), this->activeSubItems.end());
                    this->onMenuInactive.emit(this, subItemButton->getText());
                }
            }
        }

        this->emitSelectionChanged();
    }

    void ListMenu::handleSubItemClicked(Components::Button::Ptr subItem) {
        if (this->activeMode == ListMenu::ActiveMode::None) {
            return;
        }

        if (subItem->isActive()) {
            if (this->activeSubItems.size() > 0 && (this->activeMode == ListMenu::ActiveMode::Single || this->activeMode == ListMenu::ActiveMode::AlwaysOne)) {
                for (auto activeSubItem : this->activeSubItems) {
                    activeSubItem->setActive(false);
                    this->onMenuInactive.emit(this, activeSubItem->getText());
                }

                this->activeSubItems.clear();
            }
        } else {
            if (this->activeSubItems.size() <= 1 && (this->activeMode == ListMenu::ActiveMode::AlwaysOne || this->activeMode == ListMenu::ActiveMode::AtLeastOne)) {
                subItem->setActive(true);
                return;
            }
        }

        if (subItem->isActive()) {
            this->activeSubItems.push_back(subItem);
            this->onMenuActive.emit(this, subItem->getText());
        } else {
            this->activeSubItems.erase(std::remove(this->activeSubItems.begin(), this->activeSubItems.end(), subItem), this->activeSubItems.end());
            this->onMenuInactive.emit(this, subItem->getText());
        }

        this->emitSelectionChanged();
    }

    void ListMenu::emitSelectionChanged() {
        size_t activeItemsSize = this->activeItems.size();
        size_t activeSubItemsSize = this->activeSubItems.size();

        std::vector<std::reference_wrapper<const tgui::String>> selection;
        selection.reserve(activeItemsSize + activeSubItemsSize);

        for (size_t i = 0; i < activeItemsSize; i++) {
            selection.push_back(this->activeItems[i]->getText());
        }
        for (size_t i = 0; i < activeSubItemsSize; i++) {
            selection.push_back(this->activeSubItems[i]->getText());
        }

        this->onSelectionChanged.emit(this, selection);
    }
}
