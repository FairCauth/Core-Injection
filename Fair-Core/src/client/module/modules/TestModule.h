#pragma once
#include "..\Module.h"

#include "..\..\setting\Setting.h"
#include "..\..\setting\Settings.h"
class TestModule : public Module
{
public:
	TestModule() : Module("ClientSetting", "Combat", 0) {
		Setting<bool>* setting = new BooleanSetting("TestSetting", true);
		Setting<float>* setting2 = new NumberSetting("NumberSetting", 5, 0, 100,"%.3f");
		Setting < std::string > * setting3 = new ModeSetting("ModeSetting", "Mode1", { "Mode1", "Mode12", "Mode13" });
		Setting< std::string>* setting4 = new ModeSetting("ModeSetting2", "A", { "A", "B", "C" });
		Setting<bool>* setting5 = new BooleanSetting("BoolSetting", false);
		Setting<ImColor>* setting55 = new ColorSetting("GuiColor", ImColor(255,0,0));
		Setting< std::string>* GuiStyle = new ModeSetting("GuiStyle", "2", { "1", "2", "C" });

		registerSetting(GuiStyle);

		Setting<bool>* setting5sdgf = new BooleanSetting(u8"ɱ¾�⻷", false);
		Setting<bool>* setting51f = new BooleanSetting("BoolSdfgetsdting", false);
		Setting<bool>* setting5sdg = new BooleanSetting("BoogdfglSetting", false);
		Setting<bool>* setting5as = new BooleanSetting("BoolSgetting", false);
		Setting<bool>* setting5g = new BooleanSetting("BoolSdfgfdgdsfetting", false);
		Setting<bool>* settingd5 = new BooleanSetting("BoolSedfgtting", false);
		Setting<bool>* setting5gdf = new BooleanSetting("BoodffggSetting", false);
		registerSetting(setting5sdgf);
		registerSetting(setting51f);
		registerSetting(setting5sdg);
		registerSetting(setting5as);
		registerSetting(setting5g);
		registerSetting(settingd5);
		registerSetting(setting5gdf);

		Setting<bool>* setting5sdgf2 = new BooleanSetting("BoolSettifsdfdgsdng2", false);
		Setting<bool>* setting51f2 = new BooleanSetting("BoolSdfgetsdting2", false);
		Setting<bool>* setting5sdg2 = new BooleanSetting("BoogdfglSetting2", false);
		Setting<bool>* setting5as2 = new BooleanSetting("BoolSgetting2", false);
		Setting<bool>* setting5g2 = new BooleanSetting("BoolSdfgfdgdsfetting2", false);
		Setting<bool>* settingd52 = new BooleanSetting("BoolSedfgtting2", false);
		Setting<bool>* setting5gdf2 = new BooleanSetting("BoodffggSetting2", false);
		registerSetting(setting5sdgf2);
		registerSetting(setting51f2);
		registerSetting(setting5sdg2);
		registerSetting(setting5as2);
		registerSetting(setting5g2);
		registerSetting(settingd52);
		registerSetting(setting5gdf2);

		registerSetting(setting2);
		registerSetting(setting3);
		registerSetting(setting4);
		registerSetting(setting55);
		registerSetting(setting5);
	}
};
class TestModule33 : public Module
{
public:
	TestModule33() : Module("TestModule33", "Movement", 0) {
		Setting<bool>* setting = new BooleanSetting("TestSetting", true);
		Setting<float>* setting2 = new NumberSetting("NumberSetting", 5, 0, 100, "#");
		Setting< std::string>* setting3 = new ModeSetting("ModeSetting", "Mode1", { "Mode1", "Mode12", "Mode13" });
		Setting< std::string>* setting4 = new ModeSetting("ModeSetting2", "A", { "A", "B", "C" });
		Setting<bool>* setting5 = new BooleanSetting("BoolSetting", false);



		registerSetting(setting);
		registerSetting(setting2);

		registerSetting(setting4);
		registerSetting(setting5);

	}
};
class TestModule44 : public Module
{
public:
	TestModule44() : Module("TestModule44", "Combat", 0) {
		Setting<bool>* setting = new BooleanSetting("TestSetting", true);
		Setting<float>* setting2 = new NumberSetting("NumberSetting", 5, 0, 100, "#");
		Setting< std::string>* setting3 = new ModeSetting("ModeSetting", "Mode1", { "Mode1", "Mode12", "Mode13" });
		Setting< std::string>* setting4 = new ModeSetting("ModeSetting2", "A", { "A", "B", "C" });
		Setting<bool>* setting5 = new BooleanSetting("BoolSetting", false);



		registerSetting(setting);
		registerSetting(setting2);
		registerSetting(setting5);

	}
};
class TestModule55 : public Module
{
public:
	TestModule55() : Module("TestModule55", "Combat", 0) {
		Setting<bool>* setting = new BooleanSetting("TestSetting", true);
		Setting<float>* setting2 = new NumberSetting("NumberSetting", 5, 0, 100, "#");
		Setting< std::string>* setting3 = new ModeSetting("ModeSetting", "Mode1", { "Mode1", "Mode12", "Mode13" });
		Setting< std::string>* setting4 = new ModeSetting("ModeSetting2", "A", { "A", "B", "C" });
		Setting<bool>* setting5 = new BooleanSetting("BoolSetting", false);



		registerSetting(setting);
		registerSetting(setting2);

		registerSetting(setting5);

	}
};