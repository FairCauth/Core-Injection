#pragma once
#include <windows.h>
#include "..\ImGui\imgui.h"
#include <d3d9.h>
class gui {
public:
	void init();
	ImFont* glitchText;
	static inline float WINDOW_WIDTH = 840;
	static inline float WINDOW_HEIGHT = 550;
	static inline LPDIRECT3D9              g_pD3D = NULL;
	static inline LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
	static inline D3DPRESENT_PARAMETERS    g_d3dpp = {};

private:
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void ResetDevice();

};