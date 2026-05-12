// dllmain.cpp (Elite vOnE - FIXED INIT)
#include "pch.h"
#include <d3d9.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include "scanner.h"
#include "ui_manager.h"
#include "target.h"

#pragma comment(lib, "d3d9.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LPDIRECT3D9              g_pD3D = NULL;
LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS    g_d3dpp = {};

bool CreateDeviceD3D(HWND hWnd) {
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL) return false;
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0) return false;
    return true;
}

void CleanupDeviceD3D() {
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    switch (msg) {
        case WM_SIZE: if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) { g_d3dpp.BackBufferWidth = LOWORD(lParam); g_d3dpp.BackBufferHeight = HIWORD(lParam); g_pd3dDevice->Reset(&g_d3dpp); } return 0;
        case WM_SYSCOMMAND: if ((wParam & 0xFFF0) == SC_KEYMENU) return 0; break;
        case WM_DESTROY: PostQuitMessage(0); return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

DWORD WINAPI MainThread(LPVOID lpParam) {
    Scanner scanner;
    scanner.m_base = (uintptr_t)GetModuleHandle(NULL);

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0, 0, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, TEXT("MEINWAV_LAYER"), NULL };
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, wc.lpszClassName, TEXT("MEINWAV ELITE PRO"), WS_POPUP, 0, 0, screenW, screenH, NULL, NULL, wc.hInstance, NULL);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_COLORKEY);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    if (!CreateDeviceD3D(hwnd)) return 1;

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Manual Turkish Glyph Ranges (Basic Latin + Latin-1 Supplement + Latin Extended-A)
    static const ImWchar ranges[] = {
        0x0020, 0x00FF, // Basic Latin + Latin-1 Supplement
        0x0100, 0x017F, // Latin Extended-A (Ğ, ğ, İ, ı, Ş, ş)
        0,
    };

    // Load Turkish Font (Segoe UI)
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 18.0f, NULL, ranges);
    if (!font) io.Fonts->AddFontDefault(); 

    g_serverMgr.Init(); // Load ElyM2 and others
    g_ui.Init(&scanner); 

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) { TranslateMessage(&msg); DispatchMessage(&msg); continue; }
        if (GetAsyncKeyState(VK_INSERT) & 1) g_ui.m_showMenu = !g_ui.m_showMenu;

        LONG curStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
        if (g_ui.m_showMenu) {
            if (curStyle & WS_EX_TRANSPARENT) SetWindowLong(hwnd, GWL_EXSTYLE, curStyle & ~WS_EX_TRANSPARENT);
        } else {
            if (!(curStyle & WS_EX_TRANSPARENT)) SetWindowLong(hwnd, GWL_EXSTYLE, curStyle | WS_EX_TRANSPARENT);
        }

        scanner.Update();
        // scanner.UpdateWallhack(); // MOVED TO UpdateHacks()
        scanner.UpdateHacks();
        g_targetSys.Update(scanner);

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        g_ui.Render();
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0) {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) CreateThread(0, 0, MainThread, 0, 0, 0);
    return TRUE;
}
