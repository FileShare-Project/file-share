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
    class View : public tgui::ScrollablePanel {
        public:
            View(const char* typeName = "Settings::View", bool initRenderer = true);
            ~View();

			typedef std::shared_ptr<View> Ptr;
			typedef std::shared_ptr<const View> ConstPtr;

			static View::Ptr create() { return std::make_shared<View>(); }
			static View::Ptr copy(View::ConstPtr widget) { return widget ? std::static_pointer_cast<View>(widget->clone()) : nullptr; }

            tgui::Signal &getSignal(tgui::String signalName) override;

            tgui::SignalTyped<const std::string&> onSelectDevice = { "onSelectDevice" };
            tgui::Signal onToggleDevice = { "onToggleDevice" };

		protected:
			tgui::Widget::Ptr clone() const override { return std::make_shared<View>(*this); }

        private:
            void createSettings();

            Components::ListMenu::Ptr menu;
    };
}
