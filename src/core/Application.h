#pragma once

#include <tchar.h>
#include <vector>

#include "ui/RenderUtils.h"
#include "ui/UI.h"
#include "utils/contact.h"
#include "utils/Keyboard.h"

class Application {
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
    bool HandleEvents();

private:
    HWND m_hwnd = nullptr;
    UIState m_uiState;
    ContactManager m_contactManager;
    Keyboard& m_keyboard;
    
    DWORD m_dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;
    bool m_done = false;
};
