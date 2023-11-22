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
        delete app;
        return 1;
    }

    app->loop();
    delete app;
    return 0;
}
