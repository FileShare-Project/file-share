/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sun Feb 13 22:03:32 2022 Léo Lhuile
** Last update Mon Aug 29 20:45:51 2022 Léo Lhuile
**
** Debug.cpp : Implementation of Debug class
*/

#include "Debug.hpp"

namespace FileShare::GUI {
    void Debug::debug(tgui::Widget *widget, int depth)
    {
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
            auto container = (tgui::Container*)(widget);

            for (const auto &child : container->getWidgets()) {
                Debug::debug(child.get(), depth + 1);
            }
        }
    }

    template <typename T>
    void Debug::debug(const tgui::Vector2<T> &vector, const tgui::String &prefix)
    {
        std::cout << Debug::getLog(vector, prefix) << std::endl;
    }
}