#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "core/Application.h"

Application::Application()
    : m_contactManager("contacts.csv")
    , m_keyboard(Keyboard::Get())
{
}

Application::~Application()
{
    Shutdown();
}

// =====================================================
// INIT
// =====================================================
bool Application::Initialize()
{
    auto winConfig = SetupWindowEnv(0.65f, 0.3f);

    m_hwnd = CreateAppWindow(winConfig, WndProc);
    if (!m_hwnd) return false;

    ImGui_ImplWin32_EnableAlphaCompositing(m_hwnd);

    if (!CreateDeviceD3D(m_hwnd))
    {
        CleanupDeviceD3D();
        return false;
    }

    SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(winConfig.dpiScale);
    ImGui::GetStyle().FontScaleDpi = winConfig.dpiScale;

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    return true;
}

// =====================================================
// MAIN LOOP
// =====================================================
void Application::Run()
{
    while (!m_done)
    {
        MSG msg;

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                m_done = true;
            }
        }

        if (m_done)
            break;

        if (g_SwapChainOccluded &&
            g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            Sleep(10);
            continue;
        }

        g_SwapChainOccluded = false;

        Update();
        Render();
    }
}

// =====================================================
// UPDATE (logic layer)
// =====================================================
void Application::Update()
{
    // =====================================================
    // input handling
    // =====================================================
    if (m_keyboard.IsKeyPressed(VK_F2))
    {
        m_uiState.display = !m_uiState.display;
        UpdateWindowState();
    }

    // =====================================================
    // handle UI events (NEW MODEL)
    // =====================================================
    ProcessUIEvents();
}

// =====================================================
// EVENT PROCESSING (replaces pendingAdd system)
// =====================================================
void Application::ProcessUIEvents()
{
    for (const auto& e : m_uiState.events)
    {
        switch (e.type)
        {
        case UIEventType::AddContact:
        {
            // payload: "name|key"
            size_t sep = e.payload.find('|');

            if (sep != std::string::npos)
            {
                std::string name = e.payload.substr(0, sep);
                std::string key = e.payload.substr(sep + 1);

                if (!name.empty() && !key.empty())
                {
                    m_contactManager.addContact({ name, key });
                }
            }
            break;
        }

        case UIEventType::SelectContact:
        {
            // currently UI-only, no backend action needed
            break;
        }

        default:
            break;
        }
    }

    m_uiState.ClearEvents();
}

// =====================================================
// WINDOW STATE
// =====================================================
void Application::UpdateWindowState()
{
    if (m_uiState.display)
    {
        SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, m_dwExStyle);
        SetForegroundWindow(m_hwnd);
        m_uiState.firstFrame = true;
    }
    else
    {
        SetWindowLongPtr(
            m_hwnd,
            GWL_EXSTYLE,
            m_dwExStyle | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE
        );
    }

    SetWindowPos(
        m_hwnd,
        HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED
    );
}

// =====================================================
// RENDER
// =====================================================
void Application::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // build view model
    ContactViewModel vm;
    vm.contacts = m_contactManager.search(m_uiState.searchBuffer);

    UI::Render(m_uiState, vm);

    ImGui::Render();

    const float clear[4] = { 0, 0, 0, 0 };
    const float debug[4] = { 0, 0.3f, 0.3f, 0.3f };

    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
    g_pd3dDeviceContext->ClearRenderTargetView(
        g_mainRenderTargetView,
        m_uiState.debug ? debug : clear
    );

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    g_pSwapChain->Present(1, 0);
}

// =====================================================
// SHUTDOWN
// =====================================================
void Application::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();

    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }

    UnregisterClassW(L"ChatRSA", GetModuleHandle(nullptr));
}