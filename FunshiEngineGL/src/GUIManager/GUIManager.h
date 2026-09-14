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
#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <cstdlib>
#include "../GUI/MenusGUI/MenuGUI.h"
#include "../GUI/SceneGUI/SceneSelectedInterface.h"
#include "../GUI/FileManagerGUI/TreeFilesInterface.h"
#include "../GUI/FileManagerGUI/ContentFolderInterface.h"
#include "../GUI/ObjetosGUI/SettingsObjectInterface.h"
#include "../GUI/SceneGUI/SceneMenuBarInterface.h"
#include "../GUI/DockSpaceGUI/DockSpaceInterface.h"
#include "../Herramientas/IconosGUI/IconosGUI.h"
#include "../FileManager/FileManager.h"
#include "../Estructuras/ListasEnlazadas/ListasConPrioridad/PriorityListaDE.h"

// Forward declarations
class SceneRegistry;
class EditorController;
class EventBus;

using namespace std;

class GUIManager {
private:
	// Use unique_ptr for owned resources
	// menuGUI es la fachada del paquete MenuGUI (GUI de inicio del motor):
	// GUIManager y main solo conversan con ella, no con las clases internas.
	// fileManager es la fachada del explorador de archivos (R1): posee el
	// modelo y la seleccion compartida que ambos paneles leen cada frame.
	std::unique_ptr<FileManager> fileManager;
	std::unique_ptr<MenuGUI> menuGUI;
	std::unique_ptr<SettingsObjectInterface> settingGUI;
	std::unique_ptr<SceneSelectedInterface> selecteableGUI;
	std::unique_ptr<SceneMenuBarInterface> menuBarGUI;
	std::unique_ptr<TreeFilesInterface> treeFilesGUI;
	std::unique_ptr<DockSpaceInterface> dockSpaceGUI;
	std::unique_ptr<IconosGUI> iconosGUI;
	EditorController* editor = nullptr;
	std::unique_ptr<ContentFolderInterface> contentOfThisFolder;

public:
	GUIManager(GLFWwindow* window);
	~GUIManager();

	void bindScene(SceneRegistry* scene, EditorController* editor, EventBus* events);

	MenuGUI* getMenuGUI();
	TreeFilesInterface* getTreeFilesGUI();
	SettingsObjectInterface* getSettingGUI(GameObject* gameObject);
	void removeSettingsGUI();
	void setSelecteableGUI(PriorityListaDE<GameObject*>* gameObjects);
	SceneMenuBarInterface* getMenuBarGUI(bool* targetBool);
	SceneSelectedInterface* getSelecteableGUI();
	ContentFolderInterface* getContentFolderGUI();
	DockSpaceInterface* getDockSpaceGUI();

	// Persistencia del estado de las ventanas (EditorConfig): restaura el
	// stateGUI de cada ventana persistente por su nombre y recoge el estado
	// actual. La ventana Settings es dinamica (depende de la seleccion) y se
	// deja fuera.
	void restaurarEstadosVentanas(const std::map<std::string, bool>& estados);
	std::map<std::string, bool> obtenerEstadosVentanas() const;
};
#endif
