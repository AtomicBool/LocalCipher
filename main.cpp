#include "main.h"
#include "RenderUtils.h"
#include "search.h"
#include "UI.h"

// UI State
UIState uiState;
ContactManager contact_manager("contacts.csv");

// Private Variables & Constants
int windowWidth, windowHeight;
ULONGLONG g_lastKeyTime[256] = { 0 };

float scaleFactor;
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

HWND hwnd;
DWORD dwExStyle = WS_EX_TOPMOST | WS_EX_LAYERED;

extern ID3D11Device*            g_pd3dDevice;
extern ID3D11DeviceContext*     g_pd3dDeviceContext;
extern IDXGISwapChain*          g_pSwapChain;
extern bool                     g_SwapChainOccluded;
extern UINT                     g_ResizeWidth, g_ResizeHeight;
extern ID3D11RenderTargetView*  g_mainRenderTargetView;

static bool KeyPressed(int vkCode, ULONGLONG interval = 150) {
    ULONGLONG now = GetTickCount64();
    if (GetAsyncKeyState(vkCode) & 0x8000) {
        if (now - g_lastKeyTime[vkCode] > interval) {
            g_lastKeyTime[vkCode] = now;
            return true;
        }
    }
    return false;
}

// Main code
int main(int, char**)
{
    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    scaleFactor = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
	screenWidth *= scaleFactor;
	screenHeight *= scaleFactor;
    windowWidth = screenWidth * 0.65;
	windowHeight = screenHeight * 0.3;

    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"LocalCipher", nullptr };
    ::RegisterClassExW(&wc);

    hwnd = ::CreateWindowExW(
        dwExStyle | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        L"LocalCipher",         // Window Title
        WS_POPUP,               // No border
        (screenWidth - windowWidth) / 2,
        (screenHeight - windowHeight) / 2,
        windowWidth,
        windowHeight,
        nullptr, nullptr, wc.hInstance, nullptr
    );

    // allowing transparent background
    ImGui_ImplWin32_EnableAlphaCompositing(hwnd);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scaleFactor);
    style.FontScaleDpi = scaleFactor;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            if (KeyPressed(VK_F2)) {
                uiState.display = !uiState.display;
                if (uiState.display) {
                    SetWindowLongPtr(hwnd, GWL_EXSTYLE, dwExStyle);
                    SetForegroundWindow(hwnd);
                    uiState.firstFrame = true;
                }
                else {
                    SetWindowLongPtr(hwnd, GWL_EXSTYLE, dwExStyle | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
                }
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
            }

            UI::Render(uiState, contact_manager);
        }

        // Rendering
        ImGui::Render();
        const float fClear[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        const float fDebug[4] = { 0.0f, 0.3f, 0.3f, 0.3f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (uiState.debug) ? fDebug : fClear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}
