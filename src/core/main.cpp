#include "core/Application.h"

int main(int, char**)
{
    SetConsoleOutputCP(CP_UTF8);

    Application app;

    if (!app.Initialize()) {
		printf("Failed to initialize application\n");
        return 1;
    }

    app.Run();

    return 0;
}
