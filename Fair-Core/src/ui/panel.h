#pragma once
#include "Gui.h"
#include <string>
namespace panel {
	std::string from_client(std::string msg);
	void on_draw(gui& gui);
}