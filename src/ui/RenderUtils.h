#pragma once

#include <windows.h>
#include <d3d11.h>

// =====================================================
// Window
// =====================================================
struct WindowConfig {
    int width, height;
    int x, y;
    float dpiScale;
};

WindowConfig SetupWindowEnv(float widthPercent, float heightPercent);
HWND CreateAppWindow(const WindowConfig& config, WNDPROC wndProc);

// =====================================================
// D3D11 lifecycle
// =====================================================
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();

void CreateRenderTarget();
void CleanupRenderTarget();

// =====================================================
// Win32
// =====================================================
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// =====================================================
// GLOBAL RENDER STATE
// =====================================================
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;

extern bool g_SwapChainOccluded;
extern UINT g_ResizeWidth;
extern UINT g_ResizeHeight;