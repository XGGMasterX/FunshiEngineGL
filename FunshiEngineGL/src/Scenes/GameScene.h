#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <memory>
#include <string>
#include <vector>

#include "../Events/EventBus.h"
#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "../Iluminacion/LightSystem.h"

class Camera;
class EditorController;
class GUIManager;
class GameObject;
class PhysicsEngine;
class SceneMenuBarInterface;
class SceneRegistry;
class SceneSelectedInterface;
class SceneSerializer;

class GameScene {
private:
    Camera* camera;
    GUIManager* managerGUI;
    SceneSelectedInterface* selecteableGUI = nullptr;
    SceneMenuBarInterface* menuBarGUI = nullptr;
    std::unique_ptr<SceneRegistry> sceneRegistry;
    std::unique_ptr<PhysicsEngine> phisics;
    std::unique_ptr<EditorController> editorController;
    std::unique_ptr<SceneSerializer> sceneSerializer;
    EventBus events;
    LightSystem lightSystem;
    float deltaTime = 0.0f;
    bool start = false;
    int gizmoOperation = 7; // ImGuizmo::TRANSLATE
    bool gizmoReady = false;

public:
    GameScene(Camera* camera, GUIManager* managerGUI);
    ~GameScene();

    ListaDE<GameObject*>* getGameObjectsScene();
    void saveScene(const std::string& filename);
    bool isStart();
    void loadScene(const std::string& pathTxt, const std::string& semiPath);
    void dibujarGameObjects();
    void dibujarObject(GameObject* object);
    void dibujarMarcadorLuz(GameObject* object);
    void mallaScene(float tam);
    void GUI();
    void update(float deltaTime);
    void gameScene();
    void setGizmoOperation(int operation);
    int getGizmoOperation() const;
    bool isGizmoCapturingInput() const;
    GameObject* pickObject(float mouseX, float mouseY);
};

#endif
