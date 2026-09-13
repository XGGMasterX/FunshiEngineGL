#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <iostream>
#include <memory>
#include <cstdlib>
#include "../GUI/MenusGUI/MenuGUI.h"
#include "../GUI/SceneGUI/SceneSelectedInterface.h"
#include "../GUI/FileManagerGUI/TreeFilesInterface.h"
#include "../GUI/FileManagerGUI/ContentFolderInterface.h"
#include "../GUI/ObjetosGUI/SettingsObjectInterface.h"
#include "../GUI/SceneGUI/SceneMenuBarInterface.h"
#include "../GUI/DockSpaceGUI/DockSpaceInterface.h"
#include "../Herramientas/IconosGUI/IconosGUI.h"
#include "../Estructuras/ListasEnlazadas/ListasConPrioridad/PriorityListaDE.h"

// Forward declarations
class SceneRegistry;
class EditorController;
class EventBus;
class PhysicsEngine;

using namespace std;

class GUIManager {
private:
	// Use unique_ptr for owned resources
	// menuGUI es la fachada del paquete MenuGUI (GUI de inicio del motor):
	// GUIManager y main solo conversan con ella, no con las clases internas.
	std::unique_ptr<MenuGUI> menuGUI;
	std::unique_ptr<SettingsObjectInterface> settingGUI;
	std::unique_ptr<SceneSelectedInterface> selecteableGUI;
	std::unique_ptr<SceneMenuBarInterface> menuBarGUI;
	std::unique_ptr<TreeFilesInterface> treeFilesGUI;
	std::unique_ptr<DockSpaceInterface> dockSpaceGUI;
	std::unique_ptr<IconosGUI> iconosGUI;
	PhysicsEngine* phisics = nullptr;
	EditorController* editor = nullptr;
	std::unique_ptr<ContentFolderInterface> contentOfThisFolder;

public:
	GUIManager(GLFWwindow* window);
	~GUIManager();

	void setPhysics(PhysicsEngine* phisics);
	void bindScene(SceneRegistry* scene, EditorController* editor, EventBus* events);

	MenuGUI* getMenuGUI();
	TreeFilesInterface* getTreeFilesGUI();
	SettingsObjectInterface* getSettingGUI(GameObject* gameObject);
	void removeSettingsGUI();
	void setSelecteableGUI(PriorityListaDE<GameObject*>* gameObjects);
	SceneMenuBarInterface* getMenuBarGUI(bool* targetBool);
	SceneSelectedInterface* getSelecteableGUI();
	void setContentFolderGUI();
	ContentFolderInterface* getContentFolderGUI();
	DockSpaceInterface* getDockSpaceGUI();
};
#endif
