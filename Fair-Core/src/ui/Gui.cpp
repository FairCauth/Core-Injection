#include "Gui.h"
#include "..\ImGui\imconfig.h"
#include "..\ImGui\imgui.h"
#include "..\ImGui\imgui_impl_dx9.h"
#include "..\ImGui\imgui_impl_win32.h"
#include "..\ImGui\imgui_internal.h"
#include "..\ImGui\imstb_rectpack.h"
#include "..\ImGui\imstb_textedit.h"
#include "..\ImGui\imstb_truetype.h"
#include <d3d9.h>
#include <string>


static HWND hwnd = NULL;

bool done = false;
bool g_NeedReset = false;
#include "font\Font.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


#include "panel.h"
#include "font/fonts.h"
#include "font/icon.h"
void gui::init() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Window", NULL };
    ::RegisterClassEx(&wc);
    std::string title = "Gui";
    HWND hwnd = ::CreateWindow(
        wc.lpszClassName,
        std::wstring(title.begin(), title.end()).c_str(),
        WS_OVERLAPPEDWINDOW | WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        100, 100,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        NULL, NULL,
        wc.hInstance, NULL);
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return;
    }
    ::ShowWindow(hwnd, SW_SHOW);
    ::UpdateWindow(hwnd);


    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);
    ImFontConfig Font_cfg;
    Font_cfg.FontDataOwnedByAtlas = false;

    //fonts::font_10 = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 10, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    //fonts::font_12 = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 12, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    fonts::font_14 = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 14, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    fonts::font_16 = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 16, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    fonts::icons = io.Fonts->AddFontFromMemoryTTF((void*)front_data_data, front_data_size, 30, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    //fonts::font_18 = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 18, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());

    glitchText = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 32, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;
        if (g_NeedReset)
        {
            ResetDevice();
            g_NeedReset = false;
        }
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();


        ImGui::NewFrame();
        {
            ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);

            ImGuiStyle& style = ImGui::GetStyle();

            ImGui::Begin("ImGui Window", nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
            {
                style.FrameRounding = 5;
               
                ImGui::PushFont(fonts::font_16);
                panel::on_draw(*this);
                ImGui::PopFont();
                style.FrameRounding = 0;

            }
            ImGui::End();
        }

        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        ImVec4 clear_color = ImVec4(0, 0, 0, 1.00f);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {

            ImGui::Render();

            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    ExitProcess(0);
}

bool gui::CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void gui::CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void gui::ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    hwnd = hWnd;
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        gui::WINDOW_WIDTH = LOWORD(lParam);
        gui::WINDOW_HEIGHT = HIWORD(lParam);

        if (gui::g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            gui::g_d3dpp.BackBufferWidth = LOWORD(lParam);
            gui::g_d3dpp.BackBufferHeight = HIWORD(lParam);
            g_NeedReset = true;
            /*         ResetDevice();*/
        }
        return 0;
    case WM_EXITSIZEMOVE:
        g_NeedReset = true;
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_CLOSE)  // µã»÷X
        {
            ::DestroyWindow(hWnd);
            return 0;
        }
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_CLOSE:
        ::ShowWindow(hWnd, SW_HIDE); // Hide the window instead of quitting
        return 0;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}