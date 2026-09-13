#include "GUIManager.h"
#include "../Objetos/Modelos3D.h"
#include "../Scenes/EditorController.h"
#include "../Scenes/SceneRegistry.h"

#include <cstdlib>

GUIManager::GUIManager(GLFWwindow* window)
    : menuGUI(new MenuGUI(window)),
      settingGUI(new SettingsObjectInterface(new Modelos3D(), false)),
      selecteableGUI(new SceneSelectedInterface(true)),
      menuBarGUI(std::make_unique<SceneMenuBarInterface>(true)),
      treeFilesGUI(nullptr), phisics(nullptr), contentOfThisFolder(nullptr) {
    const char* homeDir = std::getenv("HOME");
    const std::string path = std::string(homeDir ? homeDir : ".") + "/MotorGrafico";
    treeFilesGUI = std::make_unique<TreeFilesInterface>(true, path);
    contentOfThisFolder = std::make_unique<ContentFolderInterface>(false);
    dockSpaceGUI = std::make_unique<DockSpaceInterface>(true);
    iconosGUI = std::make_unique<IconosGUI>();
    iconosGUI->init();
    treeFilesGUI->setIconosGUI(iconosGUI.get());
    contentOfThisFolder->setIconosGUI(iconosGUI.get());
    selecteableGUI->setIconosGUI(iconosGUI.get());
}

GUIManager::~GUIManager() {
}

void GUIManager::setPhysics(PhysicsEngine* value) { phisics = value; }
void GUIManager::bindScene(SceneRegistry* scene, EditorController* editor,
                           EventBus* events) {
    this->editor = editor;
    selecteableGUI->bindScene(scene, editor, events);
}
MenuGUI* GUIManager::getMenuGUI() { return menuGUI.get(); }
TreeFilesInterface* GUIManager::getTreeFilesGUI() { return treeFilesGUI.get(); }

SettingsObjectInterface* GUIManager::getSettingGUI(GameObject* gameObject) {
    if (gameObject == nullptr) {
        settingGUI->setStateGui(false);
        return settingGUI.get();
    }
    settingGUI->setPhysics(phisics);
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
void GUIManager::setContentFolderGUI() {
    if (treeFilesGUI->getFolderContent() == nullptr) {
        contentOfThisFolder->setStateGui(false);
    } else {
        contentOfThisFolder->setFolderRoot(treeFilesGUI->getFolderContent());
        contentOfThisFolder->setStateGui(true);
    }
}
ContentFolderInterface* GUIManager::getContentFolderGUI() { return contentOfThisFolder.get(); }
DockSpaceInterface* GUIManager::getDockSpaceGUI() { return dockSpaceGUI.get(); }
