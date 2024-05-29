/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2022 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2022 Léo Lhuile
**
** Settings/View.hpp : Class to manage the settings view
*/

#pragma once

#include "Model.hpp"
#include "Components/ListMenu.hpp"
#include <TGUI/TGUI.hpp>

namespace FileShare::GUI::Settings {
    class View : public Components::ListMenu {
        public:
            View(const char* typeName = "Settings::View", bool initRenderer = true);
            ~View();

			typedef std::shared_ptr<View> Ptr;
			typedef std::shared_ptr<const View> ConstPtr;

			static View::Ptr create() { return std::make_shared<View>(); }
			static View::Ptr copy(View::ConstPtr widget) { return widget ? std::static_pointer_cast<View>(widget->clone()) : nullptr; }

            tgui::Signal &getSignal(tgui::String signalName) override;

            tgui::SignalTyped<tgui::Widget::Ptr> onMenuChange = { "MenuChanged" };

            const tgui::Widget::Ptr &getCurrentMenuContent() const { return this->currentMenuContent; }

		protected:
			tgui::Widget::Ptr clone() const override { return std::make_shared<View>(*this); }

        private:
            void createSettings();
            tgui::Widget::Ptr createApplicationSettings();
            tgui::Widget::Ptr createDevicesGeneralSettings();
            tgui::Widget::Ptr createDevicesVirtualFolderSettings();
            tgui::Widget::Ptr createDevicesAdvancedSettings();
            tgui::Widget::Ptr createAccountSettings();

            tgui::Widget::Ptr createSection(const tgui::String &title, std::vector<tgui::Widget::Ptr> contents);
            tgui::Widget::Ptr createSectionInput(const tgui::Widget::Ptr &input, const tgui::String &label = "");

            std::map<tgui::String, tgui::Widget::Ptr> menuItems;
            tgui::Widget::Ptr currentMenuContent = nullptr;

            void handleMenuSelectionChanged(const std::vector<const tgui::String> selection);
    };
}
