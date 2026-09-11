#include "GameScene.h"

#include "../GUIManager/GUIManager.h"
#include "../GUI/SceneGUI/SceneMenuBarInterface.h"
#include "../GUI/SceneGUI/SceneSelectedInterface.h"
#include "../Gizmo/Camera.h"
#include "../Fisicas/PhysicsEngine.h"
#include "../Iluminacion/Ilumination.h"
#include "../Objetos/Componentes/Color.h"
#include "../Objetos/Componentes/Script.h"
#include "../Objetos/Componentes/Transform.h"
#include "EditorController.h"
#include "SceneRegistry.h"
#include "SceneSerializer.h"
#include "ImGuizmo.h"
#include <GL/gl.h>
#include <imgui.h>
#include <iostream>

GameScene::GameScene(Camera* value, GUIManager* manager)
    : camera(value), managerGUI(manager),
      sceneRegistry(std::make_unique<SceneRegistry>()),
      phisics(std::make_unique<PhysicsEngine>()),
      editorController(
          std::make_unique<EditorController>(sceneRegistry.get(), phisics.get(),
                                              &events)),
      sceneSerializer(
          std::make_unique<SceneSerializer>(sceneRegistry.get(),
                                             editorController.get())) {
    selecteableGUI = managerGUI->getSelecteableGUI();
    managerGUI->bindScene(sceneRegistry.get(), editorController.get(), &events);
    menuBarGUI = managerGUI->getMenuBarGUI(&start);
    selecteableGUI->setPhysics(phisics.get());
}

GameScene::~GameScene() {
    if (editorController) editorController->clearScene();
    if (selecteableGUI) selecteableGUI->bindScene(nullptr, nullptr, nullptr);
}

ListaDE<GameObject*>* GameScene::getGameObjectsScene() {
    sceneRegistry->refreshGameObjectView();
    return sceneRegistry->getGameObjects();
}

void GameScene::setSun(Ilumination* value) { sun = value; }

void GameScene::saveScene(const std::string& filename) {
    if (sceneSerializer) sceneSerializer->save(filename);
}

bool GameScene::isStart() { return start; }

void GameScene::loadScene(const std::string& pathTxt, const std::string& semiPath) {
    if (sceneSerializer) {
        sceneSerializer->load(pathTxt, semiPath);
        if (selecteableGUI)
            selecteableGUI->bindScene(sceneRegistry.get(), editorController.get(),
                                       &events);
        if (managerGUI) managerGUI->removeSettingsGUI();
    }
}

void GameScene::dibujarGameObjects() {
    auto* gameObjects = getGameObjectsScene();
    if (gameObjects->isEmpty()) return;
    Position<GameObject*>* pos = gameObjects->first();
    while (pos && pos->getElement()) {
        dibujarObject(pos->getElement());
        pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
    }
}

void GameScene::dibujarObject(GameObject* object) {
    object->setTam(10);
    object->setColor(object->auxColor);
    if (object->getComponent<Transform>()) object->dibujar(deltaTime);
}

void GameScene::mallaScene(float tam) {
    const float y = -0.5f;
    glColor3fv(branco_gelo);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, branco_gelo);
    glBegin(GL_LINES);
    for (float i = -tam; i <= tam; i += 1.0f) {
        glVertex3f(i, y, -tam);
        glVertex3f(i, y, tam);
        glVertex3f(-tam, y, i);
        glVertex3f(tam, y, i);
    }
    glEnd();
    glEndList();
}

void GameScene::GUI() {
    auto* gameObjects = getGameObjectsScene();
    selecteableGUI->printGUI();
    if (gameObjects->isElement(selecteableGUI->getReturnableEntity()) && phisics) {
        managerGUI->setPhysics(phisics.get());
        managerGUI->getSettingGUI(selecteableGUI->getReturnableEntity())->printGUI();
    }
    menuBarGUI->printGUI();
    if (menuBarGUI->getCargarScripts()) {
        menuBarGUI->setCargarScripts(false);
    }
}

void GameScene::update(float value) {
    deltaTime = value;
    if (phisics) phisics->stepSimulation(value);
}

void GameScene::gameScene() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera->activate();
    if (sun) sun->apply();
    glPushMatrix();
    mallaScene(70.0f);
    glPopMatrix();
    dibujarGameObjects();
    GUI();
    GameObject* selected = selecteableGUI->getReturnableEntity();
    if (!selected) return;
    Transform* transform = selected->getComponent<Transform>();
    if (!transform) return;
    float matrix[16], view[16], projection[16];
    buildMatrixFromTransform(transform, matrix);
    camera->getViewMatrix(view);
    ImGuiIO& io = ImGui::GetIO();
    camera->getProjectionMatrix(projection, 45.0f,
                                io.DisplaySize.x / io.DisplaySize.y);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuizmo::SetRect(viewport->Pos.x, viewport->Pos.y, viewport->Size.x,
                      viewport->Size.y);
    ImGuizmo::BeginFrame();
    static ImGuizmo::MODE mode = ImGuizmo::LOCAL;
    ImGuizmo::Manipulate(view, projection,
                         static_cast<ImGuizmo::OPERATION>(gizmoOperation),
                         mode, matrix, nullptr,
                         nullptr, nullptr, nullptr);
    gizmoReady = true;
    if (ImGuizmo::IsUsing()) {
        decomposeMatrixToTransform(matrix, transform);
        float* position = transform->getTranslatef();
        transform->setTranslatef(position[0], position[1], position[2]);
    }
}

void GameScene::setGizmoOperation(int operation) {
    if (operation == ImGuizmo::TRANSLATE ||
        operation == ImGuizmo::ROTATE ||
        operation == ImGuizmo::SCALE) {
        gizmoOperation = operation;
    }
}

bool GameScene::isGizmoCapturingInput() const {
    return gizmoReady && (ImGuizmo::IsOver() || ImGuizmo::IsUsing());
}
