#pragma once
#include "Setting.h"
#include "..\..\ImGui\imconfig.h"
#include "..\..\ImGui\imgui.h"
#include "..\..\ImGui\imgui_impl_dx9.h"
#include "..\..\ImGui\imgui_impl_win32.h"
#include "..\..\ImGui\imgui_internal.h"
#include "..\..\ImGui\imstb_rectpack.h"
#include "..\..\ImGui\imstb_textedit.h"
#include "..\..\ImGui\imstb_truetype.h"
#include <map>
#include <vector>
class SettingManager {
public:
    SettingManager(void* ptr) : ptr(ptr) {}
    template<typename T>
    void registerSetting(Setting<T>* setting) {
        settings[ptr].push_back(std::make_pair(setting->getName(), setting));
    }
    template<typename T>
    static Setting<T>* getSetting(void* ptr, std::string name) {
        for (auto k : getSettings(ptr)) {
            if(k.first == name){
                return dynamic_cast<Setting<T>*>(k.second);
            }
        }
        return nullptr;
    }
    static void removeObjSettings(void * ptr) {
        settings.erase(ptr);    
    }
    static void clear() {
        settings.clear();
    }
    static std::vector<std::pair<std::string, SettingBase*>> getSettings(void* ptr) {
        std::vector<std::pair<std::string, SettingBase*>> result;

        auto it = settings.find(ptr);
        if (it != settings.end()) {
            result = it->second;
        }

        return result;
    }
private:
    void* ptr;
    static inline std::map<void*, std::vector<std::pair<std::string, SettingBase*>>> settings;
};