#include "GameScene.h"

#include "../GUIManager/GUIManager.h"
#include "../GUI/SceneGUI/SceneMenuBarInterface.h"
#include "../GUI/SceneGUI/SceneSelectedInterface.h"
#include "../Objetos/Componentes/CameraComponent.h"
#include "../Fisicas/PhysicsEngine.h"
#include "../Iluminacion/LightSystem.h"
#include "../Objetos/Componentes/Color.h"
#include "../Objetos/Componentes/Light.h"
#include "../Objetos/Componentes/Script.h"
#include "../Objetos/Componentes/Transform.h"
#include "../Objetos/Modelos3D.h"
#include "../Objetos/Componentes/Colliders/EsfereCollider.h"
#include "../Objetos/Componentes/Colliders/CubeCollider.h"
#include "../Objetos/Componentes/Colliders/Collider.h"
#include "../Objetos/Componentes/RigidBody/RigidBody.h"
#include "EditorController.h"
#include "SceneRegistry.h"
#include "SceneSerializer.h"
#include "../Rendering/RenderTarget.h"
#include "ImGuizmo.h"
#include <GL/gl.h>
#include <imgui.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

GameScene::GameScene(GUIManager* manager)
    : managerGUI(manager),
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
}

GameScene::~GameScene() {
    if (editorController) editorController->clearScene();
    if (selecteableGUI) selecteableGUI->bindScene(nullptr, nullptr, nullptr);
}

ListaDE<GameObject*>* GameScene::getGameObjectsScene() {
    sceneRegistry->refreshGameObjectView();
    return sceneRegistry->getGameObjects();
}

void GameScene::saveScene(const std::string& filename) {
    if (sceneSerializer) sceneSerializer->save(filename);
}

bool GameScene::isStart() { return start; }

void GameScene::loadScene(const std::string& pathTxt, const std::string& semiPath) {
    if (sceneSerializer) {
        sceneSerializer->load(pathTxt, semiPath);
        // Los RigidBody deserializados nunca pasan por EditorController: la
        // malla se carga despues de los componentes (shape provisional) y el
        // cuerpo no se registra en el mundo. Aqui se reconstruye la shape con
        // los vertices recien cargados y se registra el cuerpo.
        if (editorController) editorController->registerSceneRigidBodies();
        if (selecteableGUI)
            selecteableGUI->bindScene(sceneRegistry.get(), editorController.get(),
                                       &events);
        if (managerGUI) managerGUI->removeSettingsGUI();
    }
}

CameraComponent* GameScene::getActiveCamera() {
    auto* gameObjects = getGameObjectsScene();

    // Camara elegida con "Usar": se valida que siga viva en la escena y que
    // conserve su CameraComponent (si fue eliminada se vuelve al scan normal).
    if (requestedActiveCamera) {
        if (sceneRegistry->contains(requestedActiveCamera) &&
            requestedActiveCamera->getComponent<CameraComponent>()) {
            CameraComponent* camara =
                requestedActiveCamera->getComponent<CameraComponent>();
            camara->setUp(requestedActiveCamera);
            activeCamera = camara;
            activeCameraObject = requestedActiveCamera;
            return camara;
        }
        requestedActiveCamera = nullptr;
    }

    if (gameObjects && !gameObjects->isEmpty()) {
        Position<GameObject*>* pos = gameObjects->first();
        while (pos && pos->getElement()) {
            GameObject* objeto = pos->getElement();
            if (CameraComponent* camara =
                    objeto->getComponent<CameraComponent>()) {
                camara->setUp(objeto);
                activeCamera = camara;
                activeCameraObject = objeto;
                return camara;
            }
            pos = (pos != gameObjects->last()) ? gameObjects->next(pos)
                                               : nullptr;
        }
    }

    // Si la escena no tiene camara, se siembra "CamaraPrincipal": un objeto
    // vacio con Transform + CameraComponent (misma posicion que la vieja).
    if (editorController) {
        GameObject* creada = editorController->createGameObject(
            std::make_unique<Modelos3D>(nullptr), nullptr);
        if (creada) {
            std::snprintf(creada->inputName, sizeof(creada->inputName),
                          "CamaraPrincipal");
            creada->addComponent(std::make_unique<Transform>());
            Transform* transform = creada->getComponent<Transform>();
            if (transform) transform->setTranslatef(1.f, 1.f, -50.f);
            creada->addComponent(std::make_unique<CameraComponent>());
            return getActiveCamera();
        }
    }

    activeCamera = nullptr;
    activeCameraObject = nullptr;
    return nullptr;
}

void GameScene::setActiveCamera(GameObject* object) {
    if (!object || !sceneRegistry || !sceneRegistry->contains(object) ||
        !object->getComponent<CameraComponent>()) {
        requestedActiveCamera = nullptr;
        return;
    }
    requestedActiveCamera = object;
}

GameObject* GameScene::agregarCamaraEnVistaActiva() {
    if (!editorController) return nullptr;

    getActiveCamera();
    Transform* transformOrigen =
        activeCameraObject ? activeCameraObject->getGlobalTransform()
                           : nullptr;

    GameObject* creada = editorController->createGameObject(
        std::make_unique<Modelos3D>(nullptr), nullptr);
    if (!creada) return nullptr;

    creada->addComponent(std::make_unique<Transform>());
    Transform* transform = creada->getComponent<Transform>();
    if (transform) {
        if (transformOrigen) {
            transform->setTranslatef(transformOrigen->getTranslatef()[0],
                                     transformOrigen->getTranslatef()[1],
                                     transformOrigen->getTranslatef()[2]);
            const float* rot = transformOrigen->getRotatef();
            transform->setRotatef(rot[0], rot[1], rot[2], rot[3]);
        } else {
            transform->setTranslatef(0.f, 0.f, 0.f);
        }
    }

    // Nombre unico tipo "Camara N" para que las ventanas de vista previa no
    // colisionen (ImGui identifica ventanas por titulo).
    int sufijo = ++contadorCamaras;
    std::string nombre;
    for (;;) {
        nombre = "Camara " + std::to_string(sufijo);
        bool usado = false;
        auto* lista = getGameObjectsScene();
        if (lista && !lista->isEmpty()) {
            Position<GameObject*>* pos = lista->first();
            while (pos && pos->getElement()) {
                if (std::string(pos->getElement()->inputName) == nombre) {
                    usado = true;
                    break;
                }
                pos = (pos != lista->last()) ? lista->next(pos) : nullptr;
            }
        }
        if (!usado) break;
        ++sufijo;
    }
    contadorCamaras = sufijo;
    std::snprintf(creada->inputName, sizeof(creada->inputName), "%s",
                  nombre.c_str());

    auto componente = std::make_unique<CameraComponent>();
    componente->setPintar(true); // preview automatica de la cámara nueva
    creada->addComponent(std::move(componente));

    // Queda activa y seleccionada para ubicarla con el gizmo.
    setActiveCamera(creada);
    editorController->selectObject(creada);
    return creada;
}

void GameScene::dibujarGameObjects() {
    dibujarGameObjectsConOjo(activeCameraObject);
}

void GameScene::dibujarGameObjectsConOjo(GameObject* camaraOjo) {
    auto* gameObjects = getGameObjectsScene();
    if (gameObjects->isEmpty()) return;
    Position<GameObject*>* pos = gameObjects->first();
    while (pos && pos->getElement()) {
        dibujarObjectConOjo(pos->getElement(), camaraOjo);
        pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
    }
}

void GameScene::dibujarObject(GameObject* object) {
    dibujarObjectConOjo(object, activeCameraObject);
}

void GameScene::dibujarObjectConOjo(GameObject* object, GameObject* camaraOjo) {
    object->setTam(10);
    object->setColor(object->auxColor);
    if (object->getComponent<Transform>()) object->dibujar(deltaTime);
    if (object->getComponent<Light>()) dibujarMarcadorLuz(object);
    if (object->getComponent<CameraComponent>() && object != camaraOjo)
        dibujarMarcadorCamara(object);

    // Gizmo visual del collider del objeto seleccionado: el wireframe del
    // collider (p.ej. el hull de la malla) queda visible en la escena 3D,
    // acompañando al gizmo de transform cuando se edita el componente.
    if (isEditorActivo() && object != camaraOjo) {
        GameObject* selected = selecteableGUI ? selecteableGUI->getReturnableEntity() : nullptr;
        if (selected) {
            if (object == selected) {
                if (Collider* collider = object->getComponent<Collider>())
                    collider->dibujarCollider();
            } else if (editorController && editorController->hasGizmoTarget()) {
                const GizmoTarget& t = editorController->getGizmoTarget();
                if (t.owner == object && object->getComponent<Collider>())
                    object->getComponent<Collider>()->dibujarCollider();
            }
        }
    }
}

// Gizmo visual de una luz: un octaedro alambre amarillo en la posicion del
// objeto, para poder ubicar y seleccionar luces que no tienen cuerpo.
void GameScene::dibujarMarcadorLuz(GameObject* object) {
    Transform* transform = object->getGlobalTransform();
    if (!transform) return;

    float modelArr[16];
    buildMatrixFromTransform(transform, modelArr);

    const float size = 0.5f;
    const float v[6][3] = {
        { 1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f},
        { 0.f, 1.f, 0.f}, { 0.f,-1.f, 0.f},
        { 0.f, 0.f, 1.f}, { 0.f, 0.f,-1.f}};
    const int edges[12][2] = {
        {0,2},{0,3},{0,4},{0,5},
        {1,2},{1,3},{1,4},{1,5},
        {2,4},{2,5},{3,4},{3,5}};

    glPushMatrix();
    glMultMatrixf(modelArr);
    glDisable(GL_LIGHTING);
    glColor3f(1.f, 0.85f, 0.1f);
    glBegin(GL_LINES);
    for (int i = 0; i < 12; ++i) {
        glVertex3f(v[edges[i][0]][0] * size, v[edges[i][0]][1] * size,
                   v[edges[i][0]][2] * size);
        glVertex3f(v[edges[i][1]][0] * size, v[edges[i][1]][1] * size,
                   v[edges[i][1]][2] * size);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// Gizmo visual de una camara secundaria: frustum de vision alambre cian. La
// camara activa no dibuja el suyo (seria visera en la propia vista).
void GameScene::dibujarMarcadorCamara(GameObject* object) {
    CameraComponent* camara = object->getComponent<CameraComponent>();
    Transform* transform = object->getGlobalTransform();
    if (!camara || !transform) return;

    float modelArr[16];
    buildMatrixFromTransform(transform, modelArr);

    ImGuiIO& io = ImGui::GetIO();
    const float aspect = (io.DisplaySize.x > 0.f && io.DisplaySize.y > 0.f)
                             ? io.DisplaySize.x / io.DisplaySize.y
                             : 1.77f;

    const float tanHalf =
        std::tan(camara->getFov() * 0.5f * 3.14159265358979f / 180.f);
    const float nearDist = camara->getNearPlane();
    const float farDist = camara->getFarPlane();
    const float halfHNear = tanHalf * nearDist;
    const float halfWNear = halfHNear * aspect;
    const float halfHFar = tanHalf * farDist;
    const float halfWFar = halfHFar * aspect;

    // Frustum: 4 esquinas del plano near (0..3) + 4 del far (4..7). Los 12
    // bordes de "edges" indexan los 8 puntitos, por eso todo vive en un solo
    // array (antes far y near estaban separados y se leia fuera de rango).
    const float vFrustum[8][3] = {
        {-halfWNear, -halfHNear, -nearDist},
        { halfWNear, -halfHNear, -nearDist},
        {-halfWNear,  halfHNear, -nearDist},
        { halfWNear,  halfHNear, -nearDist},
        {-halfWFar,  -halfHFar,  -farDist},
        { halfWFar,  -halfHFar,  -farDist},
        {-halfWFar,   halfHFar,  -farDist},
        { halfWFar,   halfHFar,  -farDist}};
    const int edges[12][2] = {
        {0,1},{0,2},{3,1},{3,2},
        {4,5},{4,6},{7,5},{7,6},
        {0,4},{1,5},{2,6},{3,7}};

    glPushMatrix();
    glMultMatrixf(modelArr);
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.8f, 0.9f);
    glBegin(GL_LINES);
    for (int i = 0; i < 12; ++i) {
        glVertex3f(vFrustum[edges[i][0]][0], vFrustum[edges[i][0]][1],
                   vFrustum[edges[i][0]][2]);
        glVertex3f(vFrustum[edges[i][1]][0], vFrustum[edges[i][1]][1],
                   vFrustum[edges[i][1]][2]);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
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
    if (gameObjects->isElement(selecteableGUI->getReturnableEntity())) {
        managerGUI->getSettingGUI(selecteableGUI->getReturnableEntity())->printGUI();
    }
    pintarViewportsGUI();
    menuBarGUI->printGUI();
    pintarVentanaCamaras();
    if (menuBarGUI->getCargarScripts()) {
        menuBarGUI->setCargarScripts(false);
    }
}

// Dibuja la escena 3D completa (grilla + objetos + marcadores) desde una
// vista/proyeccion dadas. La "camaraOjo" es el objeto con CameraComponent que
// esta viendo (no dibuja su propio marcador).
void GameScene::dibujarEscena(const float view[16], const float projection[16],
                              GameObject* camaraOjo) {
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projection);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(view);

    lightSystem.beginFrame(getGameObjectsScene());
    mallaScene(70.0f);
    dibujarGameObjectsConOjo(camaraOjo);
}

// Pasada de vista previa (Fase 2): por cada camara con "Vista previa" activo
// se pinta la escena a una textura FBO que luego muestra una ventana ImGui.
void GameScene::dibujarViewportsPrevios() {
    auto* gameObjects = getGameObjectsScene();
    std::vector<std::unique_ptr<RenderTarget>> nuevos;
    std::vector<GameObject*> nuevosObjetos;
    if (gameObjects && !gameObjects->isEmpty()) {
        Position<GameObject*>* pos = gameObjects->first();
        while (pos && pos->getElement()) {
            GameObject* objeto = pos->getElement();
            CameraComponent* camara = objeto->getComponent<CameraComponent>();
            if (camara && camara->getPintar()) {
                camara->setUp(objeto);

                // Reutilizar el FBO del frame anterior del mismo objeto en
                // lugar de recrearlo (evita churn de texturas en el GPU).
                std::unique_ptr<RenderTarget> target;
                for (size_t i = 0; i < viewportsCamaras.size(); ++i) {
                    if (viewportsObjetos[i] == objeto) {
                        target.reset(viewportsCamaras[i].release());
                        break;
                    }
                }
                if (!target) target = std::make_unique<RenderTarget>();

                target->resize(kPreviewW, kPreviewH);
                target->bind();
                glViewport(0, 0, kPreviewW, kPreviewH);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                float view[16], projection[16];
                camara->getViewMatrix(view);
                camara->getProjectionMatrix(
                    projection,
                    static_cast<float>(kPreviewW) /
                        static_cast<float>(kPreviewH));
                dibujarEscena(view, projection, objeto);

                RenderTarget::unbind();

                nuevos.push_back(std::move(target));
                nuevosObjetos.push_back(objeto);
            }
            pos = (pos != gameObjects->last()) ? gameObjects->next(pos)
                                               : nullptr;
        }
    }
    viewportsCamaras = std::move(nuevos);
    viewportsObjetos = std::move(nuevosObjetos);
}

void GameScene::pintarViewportsGUI() {
    for (size_t index = 0; index < viewportsCamaras.size(); ++index) {
        RenderTarget* target = viewportsCamaras[index].get();
        GameObject* objeto = viewportsObjetos[index];
        if (!target || !objeto) continue;

        char title[64];
        std::snprintf(title, sizeof(title), "Vista previa: %s",
                      objeto->inputName[0] != '\0' ? objeto->inputName
                                                   : "Camara");

        ImGui::SetNextWindowSize(
            ImVec2(static_cast<float>(target->getWidth()) + 16.f,
                   static_cast<float>(target->getHeight()) + 38.f),
            ImGuiCond_Once);
        ImGui::Begin(title);
        // La textura del FBO tiene origen abajo-izquierda; se voltea el UV
        // vertical ("v" invertida) para que la vista previa no quede dada
        // vuelta.
        ImGui::Image(
            (ImTextureID)(intptr_t)target->getColorTexture(),
            ImVec2(static_cast<float>(target->getWidth()),
                   static_cast<float>(target->getHeight())),
            ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
        ImGui::End();
    }
}

// Ventana unica para colocar camaras: crear una nueva (en la vista activa),
// elegir cual se usa para navegar/ver, prender o apagar su vista previa y
// eliminar. Ventana de inicio de la Fase 2: con "Agregar camara" aparece
// automaticamente la vista previa de cada camara.
void GameScene::pintarVentanaCamaras() {
    if (!ImGui::Begin("Camaras", &ventanaCamarasAbierta)) {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Agregar camara")) {
        agregarCamaraEnVistaActiva();
    }
    ImGui::SameLine();
    ImGui::TextUnformatted(
        "Crea una camara en la vista activa y abre su vista previa.");

    ImGui::Separator();

    auto* gameObjects = getGameObjectsScene();
    if (gameObjects && !gameObjects->isEmpty()) {
        Position<GameObject*>* pos = gameObjects->first();
        while (pos && pos->getElement()) {
            GameObject* objeto = pos->getElement();
            if (CameraComponent* camara =
                    objeto->getComponent<CameraComponent>()) {
                const std::string nombre =
                    objeto->inputName[0] ? objeto->inputName : "Camara";
                const bool esActiva =
                    objeto == requestedActiveCamera ||
                    (requestedActiveCamera == nullptr &&
                     objeto == activeCameraObject);

                ImGui::PushID(static_cast<int>(objeto->getId()));
                if (ImGui::Selectable(nombre.c_str(), esActiva)) {
                    setActiveCamera(objeto);
                    if (editorController) editorController->selectObject(objeto);
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("Usar esta camara (navegacion + vista)");
                }

                bool pintar = camara->getPintar();
                if (ImGui::Checkbox("Vista previa", &pintar)) {
                    camara->setPintar(pintar);
                }
                ImGui::SameLine();
                if (ImGui::SmallButton("Eliminar")) {
                    GameObject* objetivo = objeto;
                    if (objetivo == requestedActiveCamera)
                        requestedActiveCamera = nullptr;
                    if (editorController) editorController->deleteGameObject(objetivo);
                    ImGui::PopID();
                    break; // la lista se muto; se cierra el paso por el frame
                }
                ImGui::PopID();
            }
            pos = (pos != gameObjects->last()) ? gameObjects->next(pos)
                                               : nullptr;
        }
    }

    ImGui::End();
}

void GameScene::update(float value) {
    deltaTime = value;
    if (phisics) phisics->stepSimulation(value);

    // Sincronizar la fisica de vuelta a los GameObjects del mundo
    // (GameObject::update escribe en los Transforms via RigidBody).
    if (start) {
        auto* gameObjects = getGameObjectsScene();
        if (!gameObjects->isEmpty()) {
            Position<GameObject*>* pos = gameObjects->first();
            while (pos && pos->getElement()) {
                pos->getElement()->update(value);
                pos = (pos != gameObjects->last()) ? gameObjects->next(pos)
                                                   : nullptr;
            }
        }
    }
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
    CameraComponent* camara = getActiveCamera();
    if (!gameObjects || gameObjects->isEmpty() || !camara) return nullptr;

    ImGuiIO& io = ImGui::GetIO();
    float screenW = io.DisplaySize.x;
    float screenH = io.DisplaySize.y;
    if (screenW <= 0.0f || screenH <= 0.0f) return nullptr;

    float x = (2.0f * mouseX) / screenW - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenH;

    float view[16], projection[16];
    camara->getViewMatrix(view);
    camara->getProjectionMatrix(projection, screenW / screenH);

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
    CameraComponent* camara = getActiveCamera();
    if (!camara) return;

    ImGuiIO& io = ImGui::GetIO();
    const int fbW = static_cast<int>(io.DisplaySize.x);
    const int fbH = static_cast<int>(io.DisplaySize.y);
    if (fbW <= 0 || fbH <= 0) return;

    // Pasada de vistas previas (Fase 2): cada camara con "Vista previa" activo
    // pinta la escena a su textura FBO antes de la pasada principal.
    dibujarViewportsPrevios();

    // Pass principal: vuelve al framebuffer de la ventana con su viewport.
    RenderTarget::unbind();
    glViewport(0, 0, fbW, fbH);

    float view[16], projection[16];
    camara->getViewMatrix(view);
    camara->getProjectionMatrix(projection,
                                static_cast<float>(fbW) /
                                    static_cast<float>(fbH));
    dibujarEscena(view, projection, activeCameraObject);

    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
    ImGuizmo::SetRect(0.0f, 0.0f, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::BeginFrame();

    // Selección de objetos con clic en la escena 3D: siempre activa, es el
    // disparador que enciende las interfaces de edición (el mismo sistema que
    // activa el gizmo). Clic en zona vacía deselecciona y vuelve a la
    // navegación libre (gizmo e interfaces se ocultan).
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (!io.WantCaptureMouse && !isGizmoCapturingInput()) {
            GameObject* clicked = pickObject(io.MousePos.x, io.MousePos.y);
            if (clicked) {
                if (selecteableGUI) selecteableGUI->setReturnableEntity(clicked);
                if (gizmoOperation == 0) gizmoOperation = ImGuizmo::TRANSLATE;
            } else {
                clearSelection();
            }
        }
    }

    /*
     * Navegación libre (sin E y sin objeto seleccionado): el sistema de
     * ventanas y el gizmo no se dibujan, solo la escena 3D.
     */
    if (!isEditorActivo()) {
        gizmoReady = false;
        return;
    }

    GUI();

    gizmoReady = false;
    GameObject* selected = selecteableGUI ? selecteableGUI->getReturnableEntity() : nullptr;

    // Gizmo generico: se edita el Transform que diga el GizmoTarget activo.
    // Default: el Transform del objeto seleccionado con el global de su padre
    // como contexto. Si SettingsCollider* armo un target (offset del collider),
    // se edita ESE transform local y el owner es el objeto con el RigidBody.
    GizmoTarget target;
    if (editorController && editorController->hasGizmoTarget()) {
        target = editorController->getGizmoTarget();
    } else if (selected) {
        target.local = selected->getComponent<Transform>();
        Entity* parentEnt = selected->getParentEntity();
        target.parentGlobal = parentEnt ? parentEnt->getGlobalTransform() : nullptr;
        target.owner = selected;
    }

    if (target.local && gizmoOperation != 0) {
        // Matriz que maniula el gizmo: parentGlobal * local (o solo local si
        // no hay contexto padre, p.ej. un objeto raiz).
        float localArr[16];
        buildMatrixFromTransform(target.local, localArr);
        float matrix[16];
        if (target.parentGlobal) {
            float parentArr[16];
            buildMatrixFromTransform(target.parentGlobal, parentArr);
            glm::mat4 mGlobal = glm::make_mat4(parentArr) * glm::make_mat4(localArr);
            const float* ptr = glm::value_ptr(mGlobal);
            for (int i = 0; i < 16; ++i) matrix[i] = ptr[i];
        } else {
            for (int i = 0; i < 16; ++i) matrix[i] = localArr[i];
        }

        static ImGuizmo::MODE mode = ImGuizmo::LOCAL;
        ImGuizmo::Manipulate(view, projection,
                             static_cast<ImGuizmo::OPERATION>(gizmoOperation),
                             mode, matrix, nullptr,
                             nullptr, nullptr, nullptr);
        gizmoReady = true;
        if (ImGuizmo::IsUsing()) {
            // Congelar hijos SOLO al editar el transform de un objeto; el
            // offset local de un componente (collider) no arrastra hijos.
            const bool esObjeto =
                !(editorController && editorController->hasGizmoTarget());
            bool freeze = false;
            if (esObjeto && target.local) freeze = target.local->childsFreeze;

            std::vector<std::pair<Entity*, glm::mat4>> childSnapshots;
            if (freeze && target.owner) {
                for (auto* child : target.owner->getChildEntities()) {
                    if (child && child->getComponent<Transform>()) {
                        float m[16];
                        buildMatrixFromTransform(child->getGlobalTransform(), m);
                        childSnapshots.push_back({child, glm::make_mat4(m)});
                    }
                }
            }

            // Escribir de vuelta AL local: newLocal = inv(parentGlobal) * matrix
            Transform* localTransform = target.local;
            if (target.parentGlobal) {
                float parentGlobalArr[16];
                buildMatrixFromTransform(target.parentGlobal, parentGlobalArr);
                glm::mat4 invParentGlobal = glm::inverse(glm::make_mat4(parentGlobalArr));
                glm::mat4 newLocal = invParentGlobal * glm::make_mat4(matrix);
                float localMatArr[16];
                const float* ptr = glm::value_ptr(newLocal);
                for (int i = 0; i < 16; ++i) localMatArr[i] = ptr[i];
                decomposeMatrixToTransform(localMatArr, localTransform);
            } else {
                decomposeMatrixToTransform(matrix, localTransform);
            }

            if (freeze && !childSnapshots.empty() && target.owner) {
                float pM[16];
                buildMatrixFromTransform(target.owner->getGlobalTransform(), pM);
                glm::mat4 invParent = glm::inverse(glm::make_mat4(pM));
                for (auto& snap : childSnapshots) {
                    glm::mat4 newLocal = invParent * snap.second;
                    float localArr2[16];
                    const float* ptr = glm::value_ptr(newLocal);
                    for (int i = 0; i < 16; ++i) localArr2[i] = ptr[i];
                    decomposeMatrixToTransform(localArr2, snap.first->getComponent<Transform>());
                }
            }

            // El gizmo movio el transform del target: empujarlo hacia el
            // cuerpo fisico para que la simulacion parta de donde quedo
            // visualmente (INCLUYE los hijos con RigidBody).
            if (target.owner) {
                if (RigidBody* body = target.owner->getComponent<RigidBody>())
                    body->syncGameObjectToPhysics();
                for (auto* child : target.owner->getChildEntities()) {
                    if (child && child->getComponent<RigidBody>())
                        child->getComponent<RigidBody>()->syncGameObjectToPhysics();
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

void GameScene::toggleEditorInterfaces() {
    // E siempre apaga TODO de un toque (todas las pestanas, el gizmo y el
    // bloqueo de la camara) cuando hay algo activo; si no hay nada activo,
    // enciende el modo editor. La logica antigua (menuActivo = !menuActivo)
    // no limpiaba la seleccion: con un objeto seleccionado, isEditorActivo()
    // segui a true tras E y la camara parecia no desbloquearse nunca.
    const bool hayAlgoActivo =
        menuActivo ||
        (selecteableGUI &&
         selecteableGUI->getReturnableEntity() != nullptr);
    if (hayAlgoActivo) {
        menuActivo = false;
        clearSelection();
    } else {
        menuActivo = true;
    }
}

float GameScene::getSensibilidadCamara() const noexcept {
    return sensibilidadCamara;
}

void GameScene::setSensibilidadCamara(float sensibilidad) noexcept {
    if (sensibilidad > 0.0f) sensibilidadCamara = sensibilidad;
}

bool GameScene::getVentanaCamarasAbierta() const noexcept {
    return ventanaCamarasAbierta;
}

void GameScene::setVentanaCamarasAbierta(bool abierta) noexcept {
    ventanaCamarasAbierta = abierta;
}

bool GameScene::isEditorActivo() const {
    return menuActivo ||
           (selecteableGUI && selecteableGUI->getReturnableEntity() != nullptr);
}

void GameScene::clearSelection() {
    if (selecteableGUI) selecteableGUI->setReturnableEntity(nullptr);
}

