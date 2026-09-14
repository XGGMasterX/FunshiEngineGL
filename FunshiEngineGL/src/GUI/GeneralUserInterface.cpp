#include "GeneralUserInterface.h"
#include <utility>

GeneralUserInterface::GeneralUserInterface(std::string name, bool state, ImGuiWindowFlags flags)
    : nameGUI(std::move(name)), stateGUI(state), flagGUI(flags) {}
void GeneralUserInterface::setNameGui(std::string name) { nameGUI = std::move(name); }
void GeneralUserInterface::setStateGui(bool state) { stateGUI = state; }
std::string GeneralUserInterface::getNameGui() const { return nameGUI; }
bool GeneralUserInterface::getStateGui() const { return stateGUI; }
ImGuiWindowFlags GeneralUserInterface::getFlagGui() const { return flagGUI; }
void GeneralUserInterface::initGUI() { ImGui::Begin(nameGUI.c_str(), &stateGUI, flagGUI); }
void GeneralUserInterface::endGUI() { ImGui::End(); }
