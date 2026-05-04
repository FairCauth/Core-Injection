#pragma once
#include "Category.h"

class CombatCategory : public Category {
public:
	CombatCategory() : Category("Combat") { }
};
class MovementCategory : public Category {
public:
	MovementCategory() : Category("Movement") { }
};
class RenderCategory : public Category {
public:
	RenderCategory() : Category("Render") { }
};
class MiscCategory : public Category {
public:
	MiscCategory() : Category("Misc") { }
};
class WorldCategory : public Category {
public:
	WorldCategory() : Category("World") { }
};
class SettingsCategory : public Category {
public:
	SettingsCategory() : Category("Settings") { }
};
class ConfigCategory : public Category {
public:
	ConfigCategory() : Category("Config") { }
};
class HudCategory : public Category {
public:
	HudCategory() : Category("Hud") {}
};