#include "DockSpaceInterface.h"
#include <imgui.h>
#include <imgui_internal.h>

DockSpaceInterface::DockSpaceInterface(bool state)
    : GeneralUserInterface("EditorDockSpace", state,
                           ImGuiWindowFlags_NoDocking) {}

void DockSpaceInterface::initGUI() {
    // Setup del layout inicial (solo la primera vez; luego se persiste en imgui.ini)
    dockspaceId = ImGui::GetID("EditorDockSpace");
    if (ImGui::DockBuilderGetNode(dockspaceId) == nullptr) {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);

        ImGuiID central = dockspaceId;
        ImGuiID left    = ImGui::DockBuilderSplitNode(central, ImGuiDir_Left,  0.20f, nullptr, &central);
        ImGuiID right   = ImGui::DockBuilderSplitNode(central, ImGuiDir_Right, 0.25f, nullptr, &central);
        ImGuiID top     = ImGui::DockBuilderSplitNode(central, ImGuiDir_Up,    0.06f, nullptr, &central);
        ImGuiID bottom  = ImGui::DockBuilderSplitNode(central, ImGuiDir_Down,  0.25f, nullptr, &central);

        // La columna izquierda se divide: arbol de objetos arriba, archivos abajo
        ImGuiID leftBottom = ImGui::DockBuilderSplitNode(left, ImGuiDir_Up, 0.5f, nullptr, &left);

        ImGui::DockBuilderDockWindow("SelectedObjects", left);
        ImGui::DockBuilderDockWindow("BrowseFile",      leftBottom);
        ImGui::DockBuilderDockWindow("Settings",        right);
        ImGui::DockBuilderDockWindow("MenuBar",         top);
        ImGui::DockBuilderDockWindow("Show Folder ",    bottom);

        ImGui::DockBuilderFinish(dockspaceId);
    }
}

void DockSpaceInterface::contentGUI() {
    // PassthruCentralNode: el nodo central queda transparente y deja ver la escena 3D
    ImGui::DockSpaceOverViewport(dockspaceId,
                                 ImGui::GetMainViewport(),
                                 ImGuiDockNodeFlags_PassthruCentralNode);
}

void DockSpaceInterface::endGUI() {}

void DockSpaceInterface::printGUI() {
    if (stateGUI) { initGUI(); contentGUI(); endGUI(); }
}