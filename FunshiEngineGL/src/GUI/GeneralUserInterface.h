#ifndef GENERALUSERINTERFACE_H
#define GENERALUSERINTERFACE_H
#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


using namespace std;

class GeneralUserInterface {
protected:
	string nameGUI;
	bool stateGUI;
	ImGuiWindowFlags flagGUI;

public:
	GeneralUserInterface(string nameGUI, bool stateGUI, ImGuiWindowFlags flagGUI);

	void setNameGui(string nameGUI);
	void setStateGui(bool stateGUI);

	string getNameGui();
	bool getStateGui();
	ImGuiWindowFlags getFlagGui();

	virtual void initGUI();

	virtual void contentGUI() = 0;
	virtual void printGUI() = 0;

	virtual void endGUI();
};
#endif