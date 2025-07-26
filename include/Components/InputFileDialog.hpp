/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Sat Jan 15 21:08:44 2024 Léo Lhuile
** Last update Wed Sep 14 22:26:27 2024 Léo Lhuile
**
** InputFileDialog.hpp : Class to manage an InputFileDialog.
*/

#pragma once

#include <TGUI/TGUI.hpp>

namespace FileShare::GUI::Components {
    class InputFileDialog : public tgui::EditBox {
        public:
            enum FileMode {
                File,
                Directory
            };

            InputFileDialog(const char *typeName = "Components::InputFileDialog", bool initRenderer = true);
            ~InputFileDialog();

            typedef std::shared_ptr<InputFileDialog> Ptr;
            typedef std::shared_ptr<const InputFileDialog> ConstPtr;

            static InputFileDialog::Ptr create() { return std::make_shared<InputFileDialog>(); }
            static InputFileDialog::Ptr copy(InputFileDialog::ConstPtr widget) { return widget ? std::static_pointer_cast<InputFileDialog>(widget->clone()) : nullptr; }

            void setMode(FileMode mode) { this->mode = mode; }
            FileMode getMode() const { return this->mode; }

        protected:
            tgui::Widget::Ptr clone() const override { return std::make_shared<InputFileDialog>(*this); }

        private:
            void openFileDialog();

            FileMode mode = FileMode::File;
    };
}
