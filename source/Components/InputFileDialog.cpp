/*
** Project FileShare, 2022
**
** Author Léo Lhuile
**
** Started on  Tue May 28 23:28:58 2024 Léo Lhuile
** Last update Tue May 28 23:28:58 2024 Léo Lhuile
**
** InputFileDialog.cpp : Implementation of InputFileDialog class
*/

#include <TGUI/Backend/SFML-Graphics.hpp>
#include "Components/InputFileDialog.hpp"

namespace FileShare::GUI::Components {
    InputFileDialog::InputFileDialog(const char* typeName, bool initRenderer)
        : tgui::EditBox(typeName, initRenderer)
    {
        this->setRenderer(tgui::Theme::getDefault()->getRendererNoThrow("EditBox"));

        this->setReadOnly(true);
        this->onClick(&InputFileDialog::openFileDialog, this);
    }

    InputFileDialog::~InputFileDialog() {}

    void InputFileDialog::openFileDialog()
    {
        sf::RenderWindow window = { sf::VideoMode(800, 600), "FileShare - Select folder" };
        tgui::Gui gui = { window };

        auto fileDialog = tgui::FileDialog::create("", "Select", true);
        fileDialog->getRenderer()->setTitleBarHeight(0);
        fileDialog->setTitleButtons(tgui::ChildWindow::TitleButton::None);
        fileDialog->setPositionLocked(true);
        fileDialog->setResizable(false);
        fileDialog->setSelectingDirectory(this->mode == FileMode::Directory);
        fileDialog->setSize("100%", "100%");
        fileDialog->showWithEffect(tgui::ShowEffectType::Scale, sf::milliseconds(100));
        fileDialog->onFileSelect(&InputFileDialog::setText, this);
        fileDialog->onFileSelect(&sf::RenderWindow::close, &window);
        fileDialog->onClose(&sf::RenderWindow::close, &window);
        fileDialog->onCancel(&sf::RenderWindow::close, &window);
        gui.add(fileDialog);
        gui.mainLoop();
    }
}
