/*
** Project FileShare, 2022
**
** Author Francois Michaut
**
** Started on  Sat Jan 15 01:26:14 2022 Francois Michaut
** Last update Sat Aug 27 21:22:00 2022 Francois Michaut
**
** main.cpp : Main entry point
*/

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

#include <FileShareProtocol/Protocol.hpp>

int main()
{
    sf::RenderWindow window{{800, 600}, "Test"};
    tgui::Gui gui{window};

    window.setFramerateLimit(30);
    while (window.isOpen()) {
        sf::Event event{};

        while (window.pollEvent(event)) {
            gui.handleEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        gui.draw();
        window.display();
    }
}
