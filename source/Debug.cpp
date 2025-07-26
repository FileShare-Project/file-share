/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun May 26 15:18:37 2024 Léo Lhuile
** Last update Sat Jul 26 21:26:53 2025 Léo Lhuile
**
** Debug.cpp : Implementation of Debug class
*/

#include "Debug.hpp"

namespace FileShare::GUI {
    void Debug::debug(tgui::Widget *widget, int depth) {
        tgui::String indent = "";
        for (int i = 0; i < depth; i++) {
            indent += "  ";
        }

        std::cout << indent << widget->getWidgetType() << ": "
            << "Name(" << widget->getWidgetName() << ") - "
            << Debug::getLogSize(widget->getFullSize()) << " - "
            << Debug::getLogPos(widget->getPosition())
            << std::endl;

        if (widget->isContainer()) {
            auto container = (tgui::Container *)(widget);

            for (const auto &child : container->getWidgets()) {
                Debug::debug(child.get(), depth + 1);
            }
        }
    }
}
