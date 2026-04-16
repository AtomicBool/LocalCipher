#pragma once

#include <vector>

#include "ui/RenderUtils.h"
#include "ui/UI.h"
#include "utils/contact.h"
#include "utils/Input.h"

class Application {
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    // 所有的纯逻辑更新（按键、状态变化、计算）
    void Update();
    
    // 所有的渲染相关的操作（ImGui新帧、D3D绘制、Present）
    void Render();
    
    // 辅助函数：更新窗口显示状态（穿透/显示）
    void UpdateWindowState();

    bool HandleEvents();

private:
    HWND m_hwnd = nullptr;
    UIState m_uiState;
    ContactManager m_contactManager;
    
    DWORD m_dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;
    bool m_done = false;
};
