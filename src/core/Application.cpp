#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

#include "core/Application.h"

Application::Application()
    : m_contactManager("contacts.csv")
{
}

Application::~Application()
{
    Shutdown();
}

// =====================================================
// INIT
// =====================================================
bool Application::initWindow()
{
    auto winConfig = SetupWindowEnv();

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

    ImFontConfig font_config;
    font_config.MergeMode = true;
        
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 20.0f);

    io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\msyh.ttc",
        20.0f,
        &font_config,
        io.Fonts->GetGlyphRangesChineseSimplifiedCommon()
    );

    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(winConfig.dpiScale);
    ImGui::GetStyle().FontScaleDpi = winConfig.dpiScale;

    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    return true;
}

bool Application::loadKeyPair(std::string privateKeyPath, std::string publicKeyPath) {
    bool loaded = false;

    if (FileUtils::Exists(privateKeyPath))
    {
        auto privateKey = FileUtils::LoadBinary(privateKeyPath);

        if (!privateKey.empty())
        {
            loaded = m_userRSA.ImportPrivateKey(privateKey);

            if (loaded)
            {
                std::cout << "Loaded existing keypair\n";
            }
        }
    }

    if (!loaded)
    {
        std::cout << "Generating new keypair...\n";

        if (!m_userRSA.GenerateKeyPair())
        {
            std::cout << "Key generation failed\n";
            return false;
        }

        auto publicKey = m_userRSA.ExportPublicKey();
        auto privateKey = m_userRSA.ExportPrivateKey();

        FileUtils::SaveBinary(publicKeyPath, publicKey);
        FileUtils::SaveBinary(privateKeyPath, privateKey);

        std::cout << "New keypair generated and saved\n";
    }

    std::string publicKeyHexString = Conversion::BytesToString(m_userRSA.ExportPublicKey());
    std::cout << "[Public Key] " << publicKeyHexString << std::endl;
}

bool Application::Initialize()
{
    return initWindow() && loadKeyPair();
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
    POINT p = m_mouse.getPos();
    ImVec2 mousePos((float)p.x, (float)p.y);
    m_popupState.curMousePos = mousePos;

    // =====================================================
    // input handling
    // =====================================================
    if (m_input.IsPressed(VK_F2))
    {
        m_uiState.display = !m_uiState.display;
        UpdateWindowState();
    }

    if (m_input.IsPressed(VK_F3)) {
        std::string res = "";

        try {
            // f1 -> copy -> read clipboard -> decrypt -> render popup
            m_keyboard.Combo({ VK_CONTROL, 'A' });
            m_keyboard.Combo({ VK_CONTROL, 'C' });
            std::string clipboardText = m_clipboard.GetText();
            res = m_userRSA.Decrypt(Conversion::StringToBytes(clipboardText));
        }
        catch (const std::exception& e) {
            std::cout << "[Decrypt failed] " << e.what() << std::endl;
        }

        m_popupState.text = Conversion::GBKToUTF8(res);
        m_popupState.visible = true;
        m_popupState.lastMousePos = mousePos;
    }

    if (m_input.IsPressed(VK_F4)) {
		// f3 -> select all -> copy -> read clipboard -> encrypt -> write clipboard -> paste
        m_peerRSA.ImportPublicKey(Conversion::StringToBytes(m_selectedContact.public_key));
        
        m_keyboard.Combo({ VK_CONTROL, 'A' });
        m_keyboard.Combo({ VK_CONTROL, 'C' });
        std::string clipboardText = m_clipboard.GetText();
		m_clipboard.SetText(Conversion::BytesToString(m_peerRSA.Encrypt(clipboardText)));
        m_keyboard.Combo({ VK_CONTROL, 'V' });
	}

    // =====================================================
    // handle UI events
    // =====================================================
    ProcessUIEvents();
}

// =====================================================
// EVENT PROCESSING
// =====================================================
void Application::ProcessUIEvents()
{
    for (const auto& e : m_uiState.events)
    {
        switch (e.type)
        {
            case UIEvent::Type::AddContact:
            {
                const auto& p = e.addContact;

                if (!p.name.empty() && !p.publicKey.empty())
                {
                    m_contactManager.addContact({
                        p.name,
                        p.publicKey
                    });
                }
                printf(
                    "[added] %s\n", p.name.c_str()
                );
                break;
            }

            case UIEvent::Type::SelectContact:
            {
                const auto& p = e.selectContact;
                m_selectedContact = p.selected;

                printf(
                    "[selected] %s\n", m_selectedContact.name.c_str()
                );

                break;
            }

            default:
                break;
            }
    }

    m_uiState.ClearEvents();
}

// =====================================================
// WINDOW STATE(ACTIVATE/ABLITY TO CLICK)
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
    UI::RenderPopUP(m_popupState);

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