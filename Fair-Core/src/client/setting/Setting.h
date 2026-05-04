#pragma once
#include "SettingBase.h"

template<typename T>
class Setting : public SettingBase {
protected:
	std::string name;
	T value;
public:
	Setting(std::string name, const T& value) : name(name), value(value) {}
	bool display = true;
	int level = 0;
	std::string getNameKey() {
		return name;
	}
	std::string getName() {
		return name;
	}	
	T getValue() {
		return value;
	}
	T* getValuePtr() {
		return &value;
	}
	virtual void setValue(T value) {
		this->value = value;
	}

};