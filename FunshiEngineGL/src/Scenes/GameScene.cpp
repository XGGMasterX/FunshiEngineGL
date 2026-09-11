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
#include "../Objetos/Modelos3D.h"
#include "../Objetos/Componentes/Colliders/EsfereCollider.h"
#include "../Objetos/Componentes/Colliders/CubeCollider.h"
#include "EditorController.h"
#include "SceneRegistry.h"
#include "SceneSerializer.h"
#include "ImGuizmo.h"
#include <GL/gl.h>
#include <imgui.h>
#include <iostream>
#include <algorithm>
#include <cmath>

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

static bool intersectRayAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                             const glm::vec3& boxMin, const glm::vec3& boxMax,
                             float& tHit) {
    float tmin = -1e30f;
    float tmax = 1e30f;

    for (int i = 0; i < 3; ++i) {
        if (std::abs(rayDir[i]) < 1e-7f) {
            if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i])
                return false;
        } else {
            float invD = 1.0f / rayDir[i];
            float t1 = (boxMin[i] - rayOrigin[i]) * invD;
            float t2 = (boxMax[i] - rayOrigin[i]) * invD;
            if (t1 > t2) std::swap(t1, t2);
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            if (tmin > tmax) return false;
        }
    }
    if (tmax < 0.0f) return false;
    tHit = (tmin < 0.0f) ? 0.0f : tmin;
    return true;
}

GameObject* GameScene::pickObject(float mouseX, float mouseY) {
    auto* gameObjects = getGameObjectsScene();
    if (!gameObjects || gameObjects->isEmpty() || !camera) return nullptr;

    ImGuiIO& io = ImGui::GetIO();
    float screenW = io.DisplaySize.x;
    float screenH = io.DisplaySize.y;
    if (screenW <= 0.0f || screenH <= 0.0f) return nullptr;

    float x = (2.0f * mouseX) / screenW - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenH;

    float view[16], projection[16];
    camera->getViewMatrix(view);
    camera->getProjectionMatrix(projection, 45.0f, screenW / screenH);

    glm::mat4 viewMat = glm::make_mat4(view);
    glm::mat4 projMat = glm::make_mat4(projection);
    glm::mat4 invVP = glm::inverse(projMat * viewMat);

    glm::vec4 rayStartClip(x, y, -1.0f, 1.0f);
    glm::vec4 rayEndClip(x, y, 1.0f, 1.0f);

    glm::vec4 rayStartWorld = invVP * rayStartClip;
    if (std::abs(rayStartWorld.w) < 1e-6f) return nullptr;
    rayStartWorld /= rayStartWorld.w;

    glm::vec4 rayEndWorld = invVP * rayEndClip;
    if (std::abs(rayEndWorld.w) < 1e-6f) return nullptr;
    rayEndWorld /= rayEndWorld.w;

    glm::vec3 rayOrigin = glm::vec3(rayStartWorld);
    glm::vec3 rayDir = glm::normalize(glm::vec3(rayEndWorld - rayStartWorld));

    GameObject* closestObject = nullptr;
    float minDistance = 1e30f;

    Position<GameObject*>* pos = gameObjects->first();
    while (pos && pos->getElement()) {
        GameObject* obj = pos->getElement();
        Transform* transform = obj->getGlobalTransform();
        if (transform) {
            float modelArr[16];
            buildMatrixFromTransform(transform, modelArr);
            glm::mat4 modelMat = glm::make_mat4(modelArr);
            glm::mat4 invModel = glm::inverse(modelMat);

            glm::vec3 localRayOrigin = glm::vec3(invModel * glm::vec4(rayOrigin, 1.0f));
            glm::vec3 localRayDir = glm::normalize(glm::vec3(invModel * glm::vec4(rayDir, 0.0f)));

            glm::vec3 boxMin(-1.0f, -1.0f, -1.0f);
            glm::vec3 boxMax(1.0f, 1.0f, 1.0f);

            if (auto* m3d = dynamic_cast<Modelos3D*>(obj)) {
                vec3 bMin, bMax;
                if (m3d->getBoundingBox(bMin, bMax)) {
                    boxMin = glm::vec3(bMin.x, bMin.y, bMin.z);
                    boxMax = glm::vec3(bMax.x, bMax.y, bMax.z);
                }
            } else if (auto* sc = obj->getComponent<EsfereCollider>()) {
                float r = sc->getRadio();
                boxMin = glm::vec3(-r, -r, -r);
                boxMax = glm::vec3(r, r, r);
            } else if (auto* cc = obj->getComponent<CubeCollider>()) {
                float r = cc->getRadio();
                boxMin = glm::vec3(-r, -r, -r);
                boxMax = glm::vec3(r, r, r);
            }

            for (int i = 0; i < 3; ++i) {
                if (boxMax[i] - boxMin[i] < 0.4f) {
                    boxMin[i] -= 0.2f;
                    boxMax[i] += 0.2f;
                }
            }

            float tHit = 0.0f;
            if (intersectRayAABB(localRayOrigin, localRayDir, boxMin, boxMax, tHit)) {
                glm::vec3 hitPointWorld = glm::vec3(modelMat * glm::vec4(localRayOrigin + localRayDir * tHit, 1.0f));
                float dist = glm::length(hitPointWorld - rayOrigin);
                if (glm::dot(hitPointWorld - rayOrigin, rayDir) > 0.0f && dist < minDistance) {
                    minDistance = dist;
                    closestObject = obj;
                }
            }
        }
        pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
    }

    return closestObject;
}

void GameScene::gameScene() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera->activate();

    float view[16], projection[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, view);
    glGetFloatv(GL_PROJECTION_MATRIX, projection);

    if (sun) sun->apply();
    glPushMatrix();
    mallaScene(70.0f);
    glPopMatrix();
    dibujarGameObjects();
    GUI();

    ImGuiIO& io = ImGui::GetIO();

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    ImGuizmo::SetRect(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::BeginFrame();

    // Selección de objetos con clic en la escena 3D
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (!io.WantCaptureMouse && !isGizmoCapturingInput()) {
            GameObject* clicked = pickObject(io.MousePos.x, io.MousePos.y);
            if (clicked) {
                if (selecteableGUI) selecteableGUI->setReturnableEntity(clicked);
                if (gizmoOperation == 0) gizmoOperation = ImGuizmo::TRANSLATE;
            } else {
                if (selecteableGUI) selecteableGUI->setReturnableEntity(nullptr);
            }
        }
    }

    gizmoReady = false;
    GameObject* selected = selecteableGUI ? selecteableGUI->getReturnableEntity() : nullptr;
    if (selected && gizmoOperation != 0) {
        Transform* globalTransform = selected->getGlobalTransform();
        if (globalTransform) {
            float matrix[16];
            buildMatrixFromTransform(globalTransform, matrix);
            static ImGuizmo::MODE mode = ImGuizmo::LOCAL;
            ImGuizmo::Manipulate(view, projection,
                                 static_cast<ImGuizmo::OPERATION>(gizmoOperation),
                                 mode, matrix, nullptr,
                                 nullptr, nullptr, nullptr);
            gizmoReady = true;
            if (ImGuizmo::IsUsing()) {
                Transform* localTransform = selected->getComponent<Transform>();
                if (localTransform) {
                    Transform* originTransform = selected->getOriginTransform();
                    if (originTransform) {
                        Transform temp;
                        decomposeMatrixToTransform(matrix, &temp);
                        float* tempPos = temp.getTranslatef();
                        float* originPos = originTransform->getTranslatef();
                        localTransform->setTranslatef(tempPos[0] - originPos[0],
                                                      tempPos[1] - originPos[1],
                                                      tempPos[2] - originPos[2]);
                        localTransform->setRotatef(temp.getRotatef()[0],
                                                   temp.getRotatef()[1],
                                                   temp.getRotatef()[2],
                                                   temp.getRotatef()[3]);
                        localTransform->setScalef(temp.getScalef()[0],
                                                  temp.getScalef()[1],
                                                  temp.getScalef()[2]);
                    } else {
                        decomposeMatrixToTransform(matrix, localTransform);
                    }
                }
            }
        }
    }
}

void GameScene::setGizmoOperation(int operation) {
    if (operation == ImGuizmo::TRANSLATE ||
        operation == ImGuizmo::ROTATE ||
        operation == ImGuizmo::SCALE ||
        operation == ImGuizmo::UNIVERSAL ||
        operation == 0) {
        gizmoOperation = operation;
    }
}

int GameScene::getGizmoOperation() const {
    return gizmoOperation;
}

bool GameScene::isGizmoCapturingInput() const {
    return gizmoReady && (ImGuizmo::IsOver() || ImGuizmo::IsUsing());
}

