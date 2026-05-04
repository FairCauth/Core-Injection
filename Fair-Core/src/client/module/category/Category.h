#pragma once
#include <string>

class Category {
public:
	bool display = false;
	int selectedItemIndex = -1;
	Category(const char* name) : name(name){}
	Category(const char* name, bool display) : name(name), display(display) {}
	const char* getName() {
		return name;
	}

protected:
	const char* name;
};	