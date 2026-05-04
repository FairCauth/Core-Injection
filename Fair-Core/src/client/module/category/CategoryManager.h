#pragma once
#include "Category.h"
#include "Categories.h"
class CategoryManager {
public:
	static void init() {
		registerCategory(new CombatCategory());
		registerCategory(new MovementCategory());
		registerCategory(new RenderCategory());
		registerCategory(new WorldCategory());
		registerCategory(new MiscCategory());
		//registerCategory(new SettingsCategory());
		//registerCategory(new ConfigCategory());
		//registerCategory(new HudCategory());
	}

	static Category* getCategory(std::string name) {
		auto it = categoriesMap.find(name);
		if (it != categoriesMap.end())
			return it->second;
		return nullptr;
	}
	static std::map<std::string, Category*> getCategories() {
		return categoriesMap;
	}
private:
	static void registerCategory(Category* c) {
		categoriesMap.insert(std::pair<std::string, Category*>(c->getName(), c));
	}
protected:
	static inline std::map<std::string, Category*> categoriesMap;

};
