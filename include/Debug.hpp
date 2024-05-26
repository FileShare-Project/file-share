/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2022 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2022 Léo Lhuile
**
** Debug.hpp : Class to manage debug functions.
*/

#pragma once

#include <TGUI/TGUI.hpp>

namespace FileShare::GUI {
    class Debug {
        public:
            static void debug(tgui::Widget::Ptr widget) { Debug::debug(widget.get()); }
            static void debug(tgui::Widget *widget) { Debug::debug(widget, 0); }
            template <typename T> static void debug(const tgui::Vector2<T> &vector, const tgui::String &prefix = "");

        private:
            static void debug(tgui::Widget *widget, int depth);

            static tgui::String getLog(const tgui::Vector2<tgui::AbsoluteOrRelativeValue> &vector, const tgui::String &prefix = "") { return prefix + "(" + vector.x.toString() + ", " + vector.y.toString() + ")"; }
            template <typename T> static tgui::String getLog(const tgui::Vector2<T> &vector, const tgui::String &prefix = "") { return prefix + "(" + tgui::String::fromNumberRounded(vector.x, 0) + ", " + tgui::String::fromNumberRounded(vector.y, 0) + ")"; }
            template <typename T> static tgui::String getLogPos(const tgui::Vector2<T> &pos) { return Debug::getLog(pos, "Pos"); }
            template <typename T> static tgui::String getLogSize(const tgui::Vector2<T> &size) { return Debug::getLog(size, "Size"); }
    };
}
