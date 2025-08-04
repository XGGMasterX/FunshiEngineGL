#ifndef SETTINGSCOMPONENT_H
#define SETTINGSCOMPONENT_H
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class SettingsComponent {
public:
	SettingsComponent(){}
	virtual void showDataComponent() = 0;
};
#endif