#pragma once
#include <windows.h>
class gui {
public:
	void init();

private:
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void ResetDevice();

};