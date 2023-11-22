#include "Application.hpp"

int main()
{
    FileShare::GUI::Application app;

    try
    {
        app.init();
    }
    catch (const tgui::Exception& e)
    {
        std::cerr << "Failed to load TGUI widgets: " << e.what() << std::endl;
    }
    app.loop();
    return 1;
}
