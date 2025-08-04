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
	GeneralUserInterface(string nameGUI,bool stateGUI,ImGuiWindowFlags flagGUI) {
		this->nameGUI = nameGUI;
		this->stateGUI = stateGUI;
		this->flagGUI = flagGUI;
	}

	void setNameGui(string nameGUI) { this->nameGUI = nameGUI; }
	void setStateGui(bool stateGUI) { this->stateGUI = stateGUI; }

	string getNameGui() { return nameGUI; }
	bool getStateGui() { return stateGUI; }
	ImGuiWindowFlags getFlagGui() { return flagGUI; }

	virtual void initGUI() {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
	}

	virtual void contentGUI() = 0;
	virtual void printGUI() = 0;

	virtual void endGUI() {
		ImGui::End();
	}
};
#endif