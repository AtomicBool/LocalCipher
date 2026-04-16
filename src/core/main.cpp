#include "core/Application.h"

int main(int, char**)
{
    Application app;

    if (!app.Initialize()) {
        return 1;
    }

    app.Run();

    return 0;
}
