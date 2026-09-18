/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
#include "GUIManager.h"
#include "../Objetos/Modelos3D.h"
#include "../Scenes/EditorController.h"
#include "../Scenes/SceneRegistry.h"

#include <cstdlib>
#include <vector>

GUIManager::GUIManager(GLFWwindow* window)
    : menuGUI(new MenuGUI(window)),
      modeloSettings(std::make_unique<Modelos3D>()),
      settingGUI(new SettingsObjectInterface(modeloSettings.get(), false)),
      selecteableGUI(new SceneSelectedInterface(true)),
      menuBarGUI(std::make_unique<SceneMenuBarInterface>(true)),
      treeFilesGUI(nullptr), contentOfThisFolder(nullptr) {
    const char* homeDir = std::getenv("HOME");
    const std::string path = std::string(homeDir ? homeDir : ".") + "/MotorGrafico";
    // El FileManager (fachada + modelo + seleccion) viva tanto como los
    // paneles que lo consumen.
    fileManager = std::make_unique<FileManager>(path);
    treeFilesGUI = std::make_unique<TreeFilesInterface>(true, fileManager.get());
    contentOfThisFolder = std::make_unique<ContentFolderInterface>(false, fileManager.get());
    dockSpaceGUI = std::make_unique<DockSpaceInterface>(true);
    iconosGUI = std::make_unique<IconosGUI>();
    iconosGUI->init();
    statusBarGUI = std::make_unique<StatusBarInterface>(true);
    treeFilesGUI->setIconosGUI(iconosGUI.get());
    contentOfThisFolder->setIconosGUI(iconosGUI.get());
    selecteableGUI->setIconosGUI(iconosGUI.get());
}

GUIManager::~GUIManager() {
}

void GUIManager::bindScene(SceneRegistry* scene, EditorController* editor,
                           EventBus* events) {
    this->editor = editor;
    selecteableGUI->bindScene(scene, editor, events);
    statusBarGUI->bindScene(scene);
    settingGUI->setEditor(editor);
}
MenuGUI* GUIManager::getMenuGUI() { return menuGUI.get(); }
TreeFilesInterface* GUIManager::getTreeFilesGUI() { return treeFilesGUI.get(); }

SettingsObjectInterface* GUIManager::getSettingGUI(GameObject* gameObject) {
    if (gameObject == nullptr) {
        settingGUI->setStateGui(false);
        return settingGUI.get();
    }
    settingGUI->setEditor(editor);
    // Solo se refresca el contenido al cambiar de objeto; no se recrea la
    // ventana (mismo patron que ContentFolderInterface).
    if (settingGUI->getObjectInInspector() != gameObject) {
        settingGUI->setTargetObject(gameObject);
    }
    settingGUI->setStateGui(true);
    return settingGUI.get();
}

void GUIManager::removeSettingsGUI() { settingGUI->setStateGui(false); }
void GUIManager::setSelecteableGUI(PriorityListaDE<GameObject*>* gameObjects) {
    // Kept for source compatibility. Scene state is injected with bindScene.
    (void)gameObjects;
}
SceneMenuBarInterface* GUIManager::getMenuBarGUI(bool* targetBool) {
    menuBarGUI->setActivador(targetBool);
    return menuBarGUI.get();
}
SceneSelectedInterface* GUIManager::getSelecteableGUI() { return selecteableGUI.get(); }
ContentFolderInterface* GUIManager::getContentFolderGUI() { return contentOfThisFolder.get(); }
DockSpaceInterface* GUIManager::getDockSpaceGUI() { return dockSpaceGUI.get(); }

StatusBarInterface* GUIManager::getStatusBarGUI() { return statusBarGUI.get(); }

void GUIManager::restaurarEstadosVentanas(const std::map<std::string, bool>& estados) {
    const std::vector<GeneralUserInterface*> ventanas = {
        selecteableGUI.get(), menuBarGUI.get(), treeFilesGUI.get(),
        contentOfThisFolder.get(), dockSpaceGUI.get()};
    for (GeneralUserInterface* ventana : ventanas) {
        if (!ventana) continue;
        auto it = estados.find(ventana->getNameGui());
        if (it != estados.end()) ventana->setStateGui(it->second);
    }
}

std::map<std::string, bool> GUIManager::obtenerEstadosVentanas() const {
    std::map<std::string, bool> estados;
    const std::vector<GeneralUserInterface*> ventanas = {
        selecteableGUI.get(), menuBarGUI.get(), treeFilesGUI.get(),
        contentOfThisFolder.get(), dockSpaceGUI.get()};
    for (const GeneralUserInterface* ventana : ventanas) {
        if (!ventana) continue;
        estados[ventana->getNameGui()] = ventana->getStateGui();
    }
    return estados;
}
