#include "main.h"
#include "RenderUtils.h"
#include "search.h"

// UI Variables
bool render = false;
int windowWidth = 1200, windowHeight = 800;
char buffer[256] = "";
float sizesPercentage[2] = {0.3f, 0.2f};

// Private Variables & Constants
ULONGLONG g_lastKeyTime[256] = { 0 };
bool firstFrame = false;

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

static bool KeyPressed(int vkCode, ULONGLONG interval = 150) { // win+tab大概200ms延迟
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
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"LocalCipher", nullptr };
    ::RegisterClassExW(&wc);

    hwnd = ::CreateWindowExW(
        dwExStyle | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        L"LocalCipher",         // 窗口标题
        WS_POPUP,               // 无边框窗口
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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); // or StyleColorsLight()

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
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

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImVec2 viewportSize = viewport->Size;  // DX11 window size

            if (KeyPressed(VK_HOME)) {
                render = !render;
                if (render) {
                    SetWindowLongPtr(hwnd, GWL_EXSTYLE, dwExStyle);
                    firstFrame = true;
                }
                else {
                    SetWindowLongPtr(hwnd, GWL_EXSTYLE, dwExStyle | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
                }
            }

            ImGuiWindowFlags window_flags =
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove;

            if (render)
            {
                SetForegroundWindow(hwnd); // SetFocus | SetActiveWindow didn't work

                ImVec2 nextWindowSize = ImVec2(viewportSize.x * sizesPercentage[0], viewportSize.y * sizesPercentage[1]);
                ImGui::SetNextWindowSize(nextWindowSize);
                ImGui::SetNextWindowPos(
                    ImVec2(
                        (viewportSize.x - nextWindowSize.x) / 2,
                        (viewportSize.y - nextWindowSize.y) / 2
                    )
                );

                ImGui::Begin("LocalCipherMain", nullptr, window_flags);

                ImGui::DragFloat2("Sizes", sizesPercentage, 0.001f, 0.3f, 0.7f);

                if (firstFrame) ImGui::SetKeyboardFocusHere(0);
                if (ImGui::InputText("##input", buffer, sizeof(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {

                }

                ImGui::Text("%.1f FPS @ %.0f*%.0f", io.Framerate, viewportSize.x, viewportSize.y);
                ImGui::End();

                firstFrame = false;
            }
        }

        // Rendering
        ImGui::Render();
        const float fClear[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, fClear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
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