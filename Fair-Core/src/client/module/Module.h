#pragma once
#include <string>
#include "..\setting\Setting.h"
#include <iostream>
#include "..\setting\SettingManager.h"
#include "category/Category.h"
#include <themida/ThemidaSDK.h>
#include "category/CategoryManager.h"
class Module : public SettingManager {
public:
	Category* category;
	bool enable = false;
	int key = 0;
	std::string desc = "";
	Module(std::string name, std::string categoryName,int key) : SettingManager(this),
		name(name), 
		categoryName(categoryName), 
		category(CategoryManager::getCategory(categoryName)),
		key(key)
	{}

	std::string getKeyName() {
		return name;
	}
	std::string getName() {
		return name;
	}
	void onChanged() {

	}
	void toggle() {
		if (enable) {
			setEnable(false);
		}
		else {
			setEnable(true);
		}
	}
	
	bool isEnable() { return enable; }
	void setEnable(bool enable) {
		this->enable = enable;

		if (enable) {

			onEnable();
		}
		else {

			onDisable();
		}
	}

	virtual void onEnable() {}
	virtual void onDisable() {}
protected:
	std::string name;
private:
	std::string categoryName;

};	