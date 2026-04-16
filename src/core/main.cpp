#include "core/Application.h"

int main(int, char**)
{
    Application app;

    if (!app.Initialize()) {
		printf("Failed to initialize application\n");
        return 1;
    }

    app.Run();

    return 0;
}
