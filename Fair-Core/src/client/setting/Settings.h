#pragma once
#include "Setting.h"
#include <sstream>
#include <vector>
#include <cstring>
#include "..\..\socket\localserver.h"
#include "..\..\dist\json\json.h"
class BooleanSetting : public Setting<bool> {
public:
    BooleanSetting(std::string name, const bool& value) : Setting<bool>(name, value) {}

    //void setValue(bool value) override {
    //    Setting<bool>::setValue(value);
    //}
};
class NumberSetting : public Setting<float> {
public:
    float minValue, maxValue;
    std::string precisePattern;
    NumberSetting(std::string name, const float& value, float min, float max, std::string precisePattern) : Setting<float>(name, value), minValue(min), maxValue(max), precisePattern(precisePattern) {}

    //void setValue(float value) override {
    //    Setting<float>::setValue(value);
    //    localserver::send("bool change11");
    //}
};

class ModeSetting : public Setting<std::string> {
public:
    void setModeValue(int index) {
        setValue(values[index]);
    }
    const char** arr = nullptr;
    std::vector<std::string> values;
    int current = 0;
    ModeSetting(std::string name, std::string value, const std::vector<std::string>& values) : Setting<std::string>(name, value), values(values) {
        size_t n = values.size();
        
        arr = new const char* [n + 1];
        for (size_t i = 0; i < n; i++) {
            if (values[i] == value) current = i;
             char* c = (char*)malloc(strlen(values[i].c_str()) + 1);
#pragma warning(suppress : 4996)
            std::strcpy(c, values[i].c_str());
            arr[i] = c;
        }
    }

};
class ColorSetting : public Setting<ImColor> {
public:

    ColorSetting(std::string name, const ImColor& value) : Setting<ImColor>(name, value) {

    }
        

};
class ButtonSetting : public Setting<std::string> {
public:

    ButtonSetting(std::string name, std::string value) : Setting<std::string>(name,value) {

    }


};