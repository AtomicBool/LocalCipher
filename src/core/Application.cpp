#include "core/Application.h"

Application::Application()
    : m_contactManager("contacts.csv")
    , m_keyboard(Keyboard::Get())
{
}

Application::~Application() {
    Shutdown();
}

bool Application::Initialize() {
    // 1. Setup Window and DPI
    auto winConfig = SetupWindowEnv(0.65f, 0.3f);
    m_hwnd = CreateAppWindow(winConfig, WndProc);
    if (!m_hwnd) return false;

    // 2. Initialize Alpha Compositing and Direct3D
    ImGui_ImplWin32_EnableAlphaCompositing(m_hwnd);
    if (!CreateDeviceD3D(m_hwnd)) {
        CleanupDeviceD3D();
        return false;
    }

    // 3. Show the window
    ::SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
    ::ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(m_hwnd);

    // 4. Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup style and scaling for IMGUI
    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(winConfig.dpiScale);
    ImGui::GetStyle().FontScaleDpi = winConfig.dpiScale;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    return true;
}

void Application::Run() {
    while (!m_done) {
        if (!HandleEvents()) break;

        // skip if window is occuluded
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED) {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // 1. logic part
        Update();

        // 2. render
        Render();
    }
}

bool Application::HandleEvents() {
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            m_done = true;
            return false;
        }
    }
    return true;
}

void Application::Update() {
    if (m_keyboard.IsKeyPressed(VK_F2)) {
        m_uiState.display = !m_uiState.display;
        
        UpdateWindowState();
    }
}

/**
 * Updates the overlay window's interactive state based on UI visibility.
 *
 * When the UI is visible:
 * - allow the window to receive focus and mouse input
 * - bring the window to the foreground
 * - mark the next frame as the first visible frame
 *
 * When the UI is hidden:
 * - make the window click-through
 * - prevent it from taking focus
 *
 * The window is always kept topmost and its frame style is refreshed.
 */
void Application::UpdateWindowState() {
    if (m_uiState.display) {
        SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, m_dwExStyle);
        SetForegroundWindow(m_hwnd);
        m_uiState.firstFrame = true;
    }
    else {
        SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, m_dwExStyle | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
    }
    SetWindowPos(m_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}

void Application::Render() {
    // ImGUI buffer
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // render ui
    UI::Render(m_uiState, m_contactManager);

    ImGui::Render();
    const float fClear[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const float fDebug[4] = { 0.0f, 0.3f, 0.3f, 0.3f };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (m_uiState.debug) ? fDebug : fClear);
    
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    g_pSwapChain->Present(1, 0); // 1 = VSync, 0 = No VSync
}

void Application::Shutdown() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    if (m_hwnd) {
        ::DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    ::UnregisterClassW(L"LocalCipher", GetModuleHandle(nullptr));
}
