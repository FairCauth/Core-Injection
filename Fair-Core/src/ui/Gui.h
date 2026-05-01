#pragma once
#include <windows.h>
#include "..\ImGui\imgui.h"
class gui {
public:
	void init();
	ImFont* Font;
	ImFont* glitchText;
	static inline float WINDOW_WIDTH = 840;
	static inline float WINDOW_HEIGHT = 550;
private:
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void ResetDevice();

};