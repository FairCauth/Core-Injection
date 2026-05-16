#pragma once
#include "Module.h"
#include "modules\TestModule.h"
#include <string>
#include <algorithm>
#include <vector>
#include "..\..\socket\localserver.h"
class ModuleManager {
public:
	static void init() {

		registerModule(new TestModule());
		registerModule(new TestModule33());

		//registerModule(new TestModule());
		registerModule(new TestModule44());
		registerModule(new TestModule55());
		//registerModule(new TestModule33());
		//registerModule(new TestModule44());
		//registerModule(new TestModule55());
		//registerModule(new SelfDestruction());

	}
	static Module* getModule(std::string name) {
		auto it = modulesMap.find(name);
		if (it != modulesMap.end())
			return it->second;
		return nullptr;
	}
	static void clear() {
		return modulesMap.clear();
	}
	static std::map<std::string, Module*> getModules() {
		return modulesMap;
	}
	static std::vector<Module*> getModulesFromCategory(Category category) {
		std::vector<Module*> r;
		for (auto m : getModules()) {
			if (m.second->category->getName() == category.getName()){
				r.push_back(m.second);
			}
		}
		return r;
	}
	static std::vector<Module*> getModulesFromCategory(Category category, std::string filterText) {
		std::vector<Module*> r;
		for (auto m : getModules()) {

			if (m.second->category->getName() == category.getName()) {

				if (filterText.empty()) {
					r.push_back(m.second);
				}
				else {
					std::string s(m.second->getName());
					std::transform(s.begin(), s.end(), s.begin(), ::tolower);
					std::transform(filterText.begin(), filterText.end(), filterText.begin(), ::tolower);

					if (s._Starts_with(filterText))
						r.push_back(m.second);
				}
					
			}
		}
		return r;
	}
	static std::vector<Module*> getFilteredModules(std::string filterText) {
		std::vector<Module*> r;
		for (auto m : getModules()) {

				if (filterText.empty()) {
					r.push_back(m.second);
				}
				else {
					std::string s(m.second->getName());
					std::transform(s.begin(), s.end(), s.begin(), ::tolower);
					std::transform(filterText.begin(), filterText.end(), filterText.begin(), ::tolower);

					if (s._Starts_with(filterText))
						r.push_back(m.second);
				}

		}
		return r;
	}
	static void registerModule(Module* module) {
		
		modulesMap.insert(std::pair<std::string, Module*>(module->getName(), module));
	}
	static void removeModule(std::string key) {
		modulesMap.erase(key);
	}
protected:
	static inline std::map<std::string, Module*> modulesMap;

};
