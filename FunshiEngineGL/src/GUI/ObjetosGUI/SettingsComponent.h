#ifndef SETTINGSCOMPONENT_H
#define SETTINGSCOMPONENT_H
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Component;

class SettingsComponent {
public:
	SettingsComponent(){}
	virtual ~SettingsComponent() = default;
	virtual void showDataComponent() = 0;
	virtual Component* getComponent() = 0;
};
#endif
