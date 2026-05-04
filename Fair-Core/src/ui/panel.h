#pragma once
#include "Gui.h"
#include <string>
namespace panel {

	enum Page {
		Login, Main
	};
	std::string from_client(std::string msg);
	void on_draw(gui& gui);
	inline Page currentPage = Page::Login;

}