#pragma once

#include <tchar.h>
#include <windows.h>

#include "crypto/RSA.h"
#include "ui/RenderUtils.h"
#include "ui/UI.h"
#include "utils/Contact.h"
#include "utils/os/InputEvents.h"

class Application
{
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    void Update();
    void Render();
    void UpdateWindowState();

    // now event-driven (replaces pending flag logic)
    void ProcessUIEvents();

private:
    HWND m_hwnd = nullptr;

    UIState m_uiState;
    ContactManager m_contactManager;

    InputEvents m_input;

    DWORD m_dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;
    bool m_done = false;
};