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
#include "GameScene.h"

#include "../GUIManager/GUIManager.h"
#include "../Events/EditorEventBus.h"
#include "../GUI/SceneGUI/SceneMenuBarInterface.h"
#include "../GUI/SceneGUI/SceneSelectedInterface.h"
#include "../Objetos/Componentes/CameraComponent.h"
#include "../Fisicas/PhysicsEngine.h"
#include "../Iluminacion/LightSystem.h"
#include "../Objetos/Componentes/Script.h"
#include "../Objetos/Componentes/Transform.h"
#include "../Objetos/Componentes/Grid.h"
#include "../Objetos/Modelos3D.h"
#include "../Objetos/SimpleObject.h"
#include "../Objetos/Componentes/Colliders/EsfereCollider.h"
#include "../Objetos/Componentes/Colliders/CubeCollider.h"
#include "../Objetos/Componentes/Colliders/Collider.h"
#include "../Objetos/Componentes/RigidBody/RigidBody.h"
#include "EditorController.h"
#include "ManifiestoAssets.h"
#include "SceneRegistry.h"
#include "SceneSerializer.h"
#include "../Assets/AssetManager.h"
#include "../Assets/AssimpMeshLoader.h"
#include "../Assets/StbImageLoader.h"
#include "../Assets/TextureManager.h"
#include "../Rendering/Backend/IRenderBackend.h"
#include "../Rendering/RenderTarget.h"
#include "../Rendering/SceneRenderer.h"
#include "../Audio/AudioEngine.h"
#include "../Audio/MiniAudioBackend.h"
#include "../GUI/CreadorUI/CreadorDeInterfaces.h"
#include "../GUI/CreadorUI/CanvasInterface.h"
#include "../Objetos/Componentes/AudioSource.h"
#include "../Objetos/Componentes/InterfaceComponent.h"
#include "../Configuracion/EditorConfig.h"
#include "ImGuizmo.h"
#include <cmath>
#include <imgui.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>

// True si la matriz 4x4 tiene algun elemento no finito (NaN/Inf). El gizmo
// nunca debe operar ni escribir matrices no finitas: al tocar un gizmo con
// una matriz corrupta, todo el transform quedaria en -nan y el objeto
// desapareceria de la escena.
static bool matrizNoFinita(glm::mat4 m) {
    const float* p = glm::value_ptr(m);
    for (int i = 0; i < 16; ++i) {
        if (!std::isfinite(p[i])) return true;
    }
    return false;
}

// Duracion minima del overlay de carga de scripts: aunque la compilacion venga
// de cache (instantanea) la barra se ve un instante, y al terminar deja un
// aviso breve con el resultado para que el usuario no dependa de la consola.
static constexpr float kOverlayProgresoMin = 0.7f;
static constexpr float kOverlayResultadoSeg = 2.2f;

GameScene::GameScene(GUIManager* manager)
    : managerGUI(manager),
      sceneRegistry(std::make_unique<SceneRegistry>()),
      phisics(std::make_unique<PhysicsEngine>()),
      assetManager(
          std::make_unique<AssetManager>(std::make_unique<AssimpMeshLoader>())),
      sceneRenderer(std::make_unique<SceneRenderer>()),
      textureManager(std::make_unique<TextureManager>(
          std::make_unique<StbImageLoader>())),
      editorController(
          std::make_unique<EditorController>(sceneRegistry.get(), phisics.get(),
                                              &events, assetManager.get())),
      sceneSerializer(
          std::make_unique<SceneSerializer>(sceneRegistry.get(),
                                             editorController.get(),
                                             assetManager.get())),
      // El backend real es miniaudio; si no hay device de audio, iniciar()
      // devuelve false y el motor queda en modo mudo (todos los reproducir
      // devuelven -1), sin romper nada: los clips existen pero no suenan.
      audioEngine(
          std::make_unique<AudioEngine>(std::make_unique<MiniAudioBackend>())) {
    selecteableGUI = managerGUI->getSelecteableGUI();
    managerGUI->bindScene(sceneRegistry.get(), editorController.get(), &events);
    managerGUI->setAudioEngine(audioEngine.get());
    asegurarGrilla();
    menuBarGUI = managerGUI->getMenuBarGUI(&start);
    // El menu del editor alterna LOCAL/GLOBAL del gizmo editando este mismo
    // bool (mismo patron que el boton play/stop con toggleBool).
    if (menuBarGUI) menuBarGUI->setGizmoGlobal(&gizmoGlobal);
    // El renderer resuelve la textura de cada Material con el cache de imagenes
    // de la escena (un solo decode por archivo, imagen compartida).
    sceneRenderer->setTextureManager(textureManager.get());
}

GameScene::~GameScene() {
    // Libera las geometrias cacheadas del SceneRenderer (meshes SUV + grilla)
    // si se llegaron a compilar. En el flujo normal main() crea GameScene con
    // new y no la destruye antes de glfwTerminate, asi que esto es higiene
    // defensiva (contexto GL vivo).
    if (sceneRenderer) sceneRenderer->destruir();
    if (editorController) editorController->clearScene();
    if (selecteableGUI) selecteableGUI->bindScene(nullptr, nullptr, nullptr);
}

void GameScene::configurarProyecto(const std::string& nombreProyecto) {
    EditorConfig::asegurarEstructuraProyecto(nombreProyecto);
    // Re-escanea Sonidos/ del proyecto: limpia el registro y lo repuebla por
    // nombre (los widgets de AudioSource/interfaces hablan por nombre, no ruta).
    clipsAudio.configurarCarpeta(EditorConfig::directorioSonidos(nombreProyecto),
                                 audioEngine.get());
    // El creador apunta a Memory/Interfaces del nuevo proyecto.
    if (managerGUI) {
        CreadorDeInterfaces* creador = managerGUI->getCreadorInterfacesGUI();
        if (creador) creador->configurarProyecto(
            EditorConfig::directorioInterfaces(nombreProyecto));
    }
}

AudioEngine* GameScene::getAudioEngine() const noexcept {
    return audioEngine.get();
}

// Reproduce o detiene los AudioSource de la escena segun la transicion de
// modo play. Al ENTRAR en play: inyecta el motor y dispara los de reproduccion
// automatica. Al SALIR de play: detiene todo (evita colas de audio en el
// editor). El motor se inyecta siempre, para que el inspector pueda probar.
void GameScene::sincronizarAudioPlay(bool entrarEnPlay) {
    auto* gameObjects = getGameObjectsScene();
    if (!gameObjects || gameObjects->isEmpty()) return;
    Position<GameObject*>* pos = gameObjects->first();
    while (pos && pos->getElement()) {
        AudioSource* source = pos->getElement()->getComponent<AudioSource>();
        if (!source) {
            pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
            continue;
        }
        source->setMotor(audioEngine.get());
        if (entrarEnPlay) {
            if (source->isReproduccionAutomatica() && !source->isReproduciendo())
                source->reproducir();
        } else {
            source->detener();
        }
        pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
    }
}

ListaDE<GameObject*>* GameScene::getGameObjectsScene() {
    sceneRegistry->refreshGameObjectView();
    return sceneRegistry->getGameObjects();
}

void GameScene::asegurarGrilla() {
    if (!sceneRegistry || !editorController) return;
    auto* lista = getGameObjectsScene();

    // Buscar grilla existente en la escena (sin importar si la escena esta vacia o no)
    bool tieneGrilla = false;
    if (lista && !lista->isEmpty()) {
        Position<GameObject*>* pos = lista->first();
        while (pos && pos->getElement()) {
            if (std::string(pos->getElement()->inputName) == "Grilla") {
                tieneGrilla = true;
                break;
            }
            pos = (pos != lista->last()) ? lista->next(pos) : nullptr;
        }
    }

    if (tieneGrilla) return;

    // Usar SimpleObject en lugar de Modelos3D para evitar que el dibujado
    // de la grilla se ancle a un modelo inexistente. SimpleObject no tiene malla.
    // Si no hay root, crear uno implicitamente (SceneRegistry::createObject lo hace
    // si entitys esta vacia). Si entitys no esta vacia pero getRoot() devuelve nullptr
    // (no deberia pasar), usamos nullptr como parent y createObject creara el root.
    GameObject* root = sceneRegistry->getRoot();
    GameObject* crea = editorController->createGameObject(
        std::make_unique<SimpleObject>(), root);
    if (!crea) {
        // Si fallo (p. ej., porque root era nullptr y entitys no estaba vacia),
        // intentar con parent = nullptr para que createObject cree un nuevo root.
        crea = editorController->createGameObject(
            std::make_unique<SimpleObject>(), nullptr);
        if (!crea) return;
    }
    std::snprintf(crea->inputName, sizeof(crea->inputName), "Grilla");
    // Forzar actualizacion de la vista de objetos para incluir la grilla recien creada
    getGameObjectsScene();
    Transform* transform = crea->getComponent<Transform>();
    if (!transform) {
        crea->addComponent(std::make_unique<Transform>());
        transform = crea->getComponent<Transform>();
    }
    if (transform) transform->setTranslatef(0.f, -0.5f, 0.f);
    crea->addComponent(std::make_unique<Grid>());
}

void GameScene::saveScene(const std::string& filename) {
    if (sceneSerializer) sceneSerializer->save(filename);
    // Manifiesto de assets (add-on): se regenera en CADA guardado, asi el
    // JSON centraliza siempre el estado vigente de las rutas (mover/renombrar
    // assets lo renueva de paso). Es independiente del .db binario.
    ManifiestoAssets::guardar(filename + "SceneAssets.json",
                              getGameObjectsScene());
}

bool GameScene::isStart() { return start; }

// La maquina de estados (orquestador) es la fuente de verdad de la simulacion:
// EditorInput la refleja aca en el path de F5/F7 (F5 -> true, F7 -> false),
// comparte flag con el boton Activar/Detener del menu de escena.
void GameScene::setStart(bool activo) noexcept { start = activo; }

bool GameScene::isSimulacionPausada() const noexcept { return simulacionPausada; }

// Pausa (F6): congela la simulacion SIN salir de play; al reanudar se retoma
// desde donde quedo (fisica y scripts solo avanzan con !pausada).
void GameScene::setSimulacionPausada(bool pausada) noexcept {
    simulacionPausada = pausada;
}

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
        // Las escenas viejas no guardan el objeto "Grilla": se crea sobre la
        // marcha si falta, conservando la visibilidad por defecto.
        asegurarGrilla();

        // Manifiesto de assets (add-on de la serializacion binaria): si
        // existe, sus rutas tienen precedencia sobre las que dejo el .db.
        // El pathTxt es <prefijo>BBDDObjetos.txt; el manifiesto comparte el
        // prefijo con nombre SceneAssets.json.
        const std::string sufijoBBDD = "BBDDObjetos.txt";
        if (pathTxt.size() >= sufijoBBDD.size() &&
            pathTxt.compare(pathTxt.size() - sufijoBBDD.size(),
                            sufijoBBDD.size(), sufijoBBDD) == 0) {
            const std::string prefijo =
                pathTxt.substr(0, pathTxt.size() - sufijoBBDD.size());
            ManifiestoAssets::cargar(prefijo + "SceneAssets.json",
                                     getGameObjectsScene());
        }
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
    // vacio con Transform + CameraComponent, encarando la grilla. La camara
    // nueva nace sin rotacion y mira en -Z (convencion del CameraComponent),
    // asi que se coloca en +Z mirando hacia el origen: la posicion vieja
    // (1, 1, -50) quedaba DE ESPALDAS a la grilla (que vive en el origen y
    // con tam 70 se extiende varios metros a la redonda) y al entrar al
    // editor parecia que la grilla "no se imprimia". Como en modo editor el
    // mouse no rota la camara (main.onMouse solo mira con !isEditorActivo),
    // esa orientacion inicial es la que ve el usuario hasta salir al menu.
    if (editorController) {
        GameObject* creada = editorController->createGameObject(
            std::make_unique<Modelos3D>(nullptr), nullptr);
        if (creada) {
            std::snprintf(creada->inputName, sizeof(creada->inputName),
                          "CamaraPrincipal");
            creada->addComponent(std::make_unique<Transform>());
            Transform* transform = creada->getComponent<Transform>();
            if (transform) transform->setTranslatef(0.f, 8.f, 40.f);
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

int GameScene::getActiveCameraId() const noexcept {
    return requestedActiveCamera ? requestedActiveCamera->getId() : -1;
}

void GameScene::setActiveCameraById(int id) {
    if (id < 0 || !sceneRegistry) return;

    auto* gameObjects = getGameObjectsScene();
    if (!gameObjects || gameObjects->isEmpty()) return;

    Position<GameObject*>* pos = gameObjects->first();
    while (pos && pos->getElement()) {
        GameObject* objeto = pos->getElement();
        if (objeto->getId() == id && objeto->getComponent<CameraComponent>()) {
            setActiveCamera(objeto);
            return;
        }
        pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
    }
    // No se encontro la camara persistida: se deja el modo automatico.
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

void GameScene::GUI() {
    auto* gameObjects = getGameObjectsScene();
    selecteableGUI->printGUI();
    if (gameObjects->isElement(selecteableGUI->getReturnableEntity())) {
        managerGUI->getSettingGUI(selecteableGUI->getReturnableEntity())->printGUI();
    }
    pintarViewportsGUI();
    menuBarGUI->printGUI();
    pintarVentanaCamaras();
    if (managerGUI) managerGUI->getStatusBarGUI()->printGUI();
    if (menuBarGUI->getCargarScripts()) {
        menuBarGUI->setCargarScripts(false);
    }

    // Sistema de audio + creador de interfaces: el catalogo de clips se
    // refresca por frame (tolerante y barato), el canvas refleja la interfaz
    // activa y cambia su presentacion segun el modo play.
    if (managerGUI) {
        CreadorDeInterfaces* creador = managerGUI->getCreadorInterfacesGUI();
        CanvasInterface* canvas = managerGUI->getCanvasGUI();
        if (creador) {
            creador->setClipNames(audioEngine->nombresClips());
            creador->printGUI();
        }
        if (canvas) {
            canvas->setAudioEngine(audioEngine.get());
            canvas->setModoPlay(start);
            UserInterfaceCustom* uiParaCanvas = nullptr;
            if (start && creador) {
                // En modo play: buscar objeto con InterfaceComponent y activar
                // su interfaz a pantalla completa (HUD del juego).
                auto* objs = getGameObjectsScene();
                if (objs && !objs->isEmpty()) {
                    Position<GameObject*>* pos = objs->first();
                    while (pos && pos->getElement()) {
                        if (auto* ic =
                                pos->getElement()->getComponent<InterfaceComponent>()) {
                            const std::string& nombre = ic->getInterfaz();
                            if (!nombre.empty())
                                uiParaCanvas = creador->activarInterfaz(nombre);
                            break; // la primera gana
                        }
                        pos = (pos != objs->last()) ? objs->next(pos) : nullptr;
                    }
                }
            } else if (creador) {
                // En editor: usar la interfaz activa del creador (prueba manual).
                uiParaCanvas = creador->getInterfazActiva();
            }
            canvas->setUI(uiParaCanvas);
            canvas->printGUI();
        }
    }
}

void GameScene::pintarViewportsGUI() {
    if (!sceneRenderer) return;
    const auto& targets = sceneRenderer->viewportTargets();
    const auto& objetos = sceneRenderer->viewportObjects();
    for (size_t index = 0; index < targets.size(); ++index) {
        RenderTarget* target = targets[index].get();
        GameObject* objeto = objetos[index];
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
        // vuelta. La GUI no ve handles concretos: pide al backend el descriptor
        // opaco (en GL el GLuint viaja como puntero = ImTextureID).
        const void* imguiTex = Rendering::Backend::activeBackend()
                                   .imguiTextureId(target->getColorTexture());
        ImGui::Image(
            static_cast<ImTextureID>(reinterpret_cast<std::intptr_t>(imguiTex)),
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
                    // "Usar" una camara publica el cambio en el bus de GUI: la
                    // fachada (GUIManager) selecciona el objeto para el
                    // inspector y los demas suscriptores reaccionan sin que la
                    // escena conozca a las ventanas.
                    if (managerGUI) {
                        EditorEvent ev;
                        ev.type = EditorEventType::CamaraActivaCambio;
                        ev.camara = objeto;
                        managerGUI->getEditorEventBus()->publish(ev);
                    }
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
    // Mientras se manipula el gizmo NO se avanza la simulacion: de lo
    // contrario la gravedad/contactos eyectan el cuerpo y la sync de vuelta
    // arrastra al objeto (efecto 'sale disparado'). Al soltar, la sim sigue.
    // La fisica SOLO corre en modo play (start==true); en editor (start==false)
    // stepSimulation no tiene consumidor (syncPhysicsToGameObject no escribe),
    // y solo causa explosiones por penetracion con el suelo (plano y=-1).

    // Transicion editor->play: el cuerpo fue creado en una pose PASADA (al
    // agregar el RigidBody o al ultimo sync). Mientras estuvo en pausa el
    // usuario pudo mover el objeto o el offset del collider; si el primer
    // stepSimulation corre con el body viejo, la sync de vuelta escribe la
    // pose del collider desde una posicion descartada. Se empuja el body a
    // la pose VISUAL actual antes de arrancar.
    if (start && !previousStart) {
        // Feedback visual inmediato: aunque no haya nada que recompilar, se ve
        // que "Activar" disparo la carga/verificacion de scripts.
        overlayProgresoVisible_ = true;
        overlayProgresoTimer_ = kOverlayProgresoMin;
        overlayResultadoPendiente_ = true;
        // Los campos de SerializeField que referencian GameObjects se guardan
        // por nombre; aca se configura el resolver hacia los objetos de ESTA
        // escena (valido mientras se construye el arbol de valores).
        ReflejoScripts::fijarResolverObjetos(
            [this](const std::string& nombre) -> GameObject* {
                auto* objetos = getGameObjectsScene();
                if (objetos->isEmpty()) return nullptr;
                Position<GameObject*>* pos = objetos->first();
                while (pos && pos->getElement()) {
                    if (nombre == pos->getElement()->inputName)
                        return pos->getElement();
                    pos = (pos != objetos->last()) ? objetos->next(pos)
                                                   : nullptr;
                }
                return nullptr;
            });

        auto* gameObjects = getGameObjectsScene();
        if (!gameObjects->isEmpty()) {
            Position<GameObject*>* pos = gameObjects->first();
            while (pos && pos->getElement()) {
                if (RigidBody* body =
                        pos->getElement()->getComponent<RigidBody>())
                    body->syncGameObjectToPhysics();
                pos = (pos != gameObjects->last()) ? gameObjects->next(pos)
                                                   : nullptr;
            }
        }

        // Audio: inyectar el motor y disparar los AudioSource automaticos.
        sincronizarAudioPlay(true);

        // Scripts: cablear los servicios de escena (audio, busqueda, teclado)
        // a la tabla que consultan los comportamientos via `servicios->...`.
        MotorScript::inyectarServiciosScript(audioEngine.get(),
                                             sceneRegistry.get(),
                                             &inputScripts);

        // Todos los scripts que necesitan (re)compilarse entran a la cola: su
        // progreso se ve en la barra "Estado" antes de bloquear con g++/javac.
        encolarScriptsIniciales();
    }

    // Transicion play->editor: avisar a los scripts para que hagan limpieza
    // (onStop) y conservar los valores editados en play mode para la GUI.
    if (previousStart && !start) {
        limpiarColaCompilacion();
        // Scripts: desconectar los servicios (los comportamientos deben
        // tolerar servicios == nullptr / tablas inoperativas al salir).
        MotorScript::inyectarServiciosScript(nullptr, nullptr, nullptr);
        // Audio: detener los AudioSource (no dejar sonando en el editor).
        sincronizarAudioPlay(false);
        auto* gameObjects = getGameObjectsScene();
        if (!gameObjects->isEmpty()) {
            Position<GameObject*>* pos = gameObjects->first();
            while (pos && pos->getElement()) {
                if (Script* script =
                        pos->getElement()->getComponent<Script>())
                    script->detener(pos->getElement());
                pos = (pos != gameObjects->last()) ? gameObjects->next(pos)
                                                   : nullptr;
            }
        }
    }
    previousStart = start;

    // La fisica y los scripts SOLO avanzan en modo play (start==true) y sin
    // pausa (F6): con simulacionPausada congelada se congela el motor pero la
    // GUI/editor sigue, para reanudar desde el mismo frame.
    if (phisics && start && !gizmoInUse() && !simulacionPausada)
        phisics->stepSimulation(value);

    // Sincronizar la fisica de vuelta a los GameObjects del mundo
    // (GameObject::update escribe en los Transforms via RigidBody).
    procesarColaCompilacion();
    if (start && !gizmoInUse() && !simulacionPausada) {
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

    // Overlay de carga de scripts: mientras hay trabajo en cola se mantiene
    // visible un minimo; al terminar deja un aviso breve con el resultado.
    if (compilacionEnCurso_ && !overlayEnCursoPrev_)
        overlayResultadoPendiente_ = true;
    overlayEnCursoPrev_ = compilacionEnCurso_;

    if (compilacionEnCurso_) {
        overlayProgresoTimer_ = kOverlayProgresoMin;
        overlayResultadoTimer_ = 0.0f;
        overlayProgresoVisible_ = true;
        overlayResultadoVisible_ = false;
    } else if (overlayProgresoTimer_ > 0.0f) {
        overlayProgresoTimer_ -= value;
        if (overlayProgresoTimer_ <= 0.0f) {
            overlayProgresoVisible_ = false;
            if (overlayResultadoPendiente_) {
                overlayResultadoTimer_ = kOverlayResultadoSeg;
                overlayResultadoVisible_ = true;
                overlayResultadoPendiente_ = false;
            }
        }
    } else if (overlayResultadoTimer_ > 0.0f) {
        overlayResultadoTimer_ -= value;
        if (overlayResultadoTimer_ <= 0.0f) overlayResultadoVisible_ = false;
    }

    // Publicar el estado de la compilacion para la barra "Estado" (se dibuja
    // al final del frame, por eso el "Mostrar" de la cola es visible).
    if (managerGUI)
        managerGUI->getStatusBarGUI()->setEstadoCompilacion(
            compilacionEnCurso_, cargaActual_, cargaHecha_, cargaTotal_,
            resultadosCarga_, overlayProgresoVisible_,
            overlayResultadoVisible_);
}

void GameScene::encolarScriptsIniciales() {
    auto* objs = getGameObjectsScene();
    if (!objs || objs->isEmpty()) return;

    bool hayPendientes = false;
    Position<GameObject*>* pos = objs->first();
    while (pos && pos->getElement()) {
        if (Script* s = pos->getElement()->getComponent<Script>()) {
            if (s->necesitaCompilar()) {
                colaCompilacion_.push_back({s, pos->getElement()});
                s->setAplazarCarga(true);
                hayPendientes = true;
            }
        }
        pos = (pos != objs->last()) ? objs->next(pos) : nullptr;
    }
    if (!hayPendientes) {
        compilacionEnCurso_ = false;
        cargaActual_.clear();
        cargaTotal_ = 0;
        cargaHecha_ = 0;
        return;
    }

    resultadosCarga_.clear();
    faseCarga_ = FaseCarga::Mostrar;
    indiceCarga_ = 0;
    cargaTotal_ = colaCompilacion_.size();
    cargaHecha_ = 0;
    compilacionEnCurso_ = true;
    cargaActual_ = colaCompilacion_[0].script->rutaFuente();
}

void GameScene::procesarColaCompilacion() {
    if (!start) {
        limpiarColaCompilacion();
        return;
    }

    // Recoleccion continua: un script editado a mitad de play tambien pasa por
    // la cola, asi su compilacion se ve en la barra de estado.
    if (colaCompilacion_.empty()) {
        auto* objs = getGameObjectsScene();
        if (!objs || objs->isEmpty()) return;
        bool hay = false;
        Position<GameObject*>* pos = objs->first();
        while (pos && pos->getElement()) {
            if (Script* s = pos->getElement()->getComponent<Script>()) {
                if (s->necesitaCompilar()) {
                    colaCompilacion_.push_back({s, pos->getElement()});
                    s->setAplazarCarga(true);
                    hay = true;
                }
            }
            pos = (pos != objs->last()) ? objs->next(pos) : nullptr;
        }
        if (hay) {
            faseCarga_ = FaseCarga::Mostrar;
            indiceCarga_ = 0;
            cargaTotal_ = colaCompilacion_.size();
            cargaHecha_ = 0;
            compilacionEnCurso_ = true;
            cargaActual_ = colaCompilacion_[0].script->rutaFuente();
        }
        return;
    }

    // Fase 1 (Mostrar): este frame solo informa cual script se va a compilar;
    // el dibujado al final del frame muestra la barra con su progreso.
    if (faseCarga_ == FaseCarga::Mostrar) {
        cargaActual_ = colaCompilacion_[indiceCarga_].script->rutaFuente();
        faseCarga_ = FaseCarga::Compilar;
        return;
    }

    // Fase 2 (Compilar): ejecuta la compilacion sincronica del iesimo script.
    {
        const CargaPendiente& item = colaCompilacion_[indiceCarga_];
        item.script->setAplazarCarga(false);
        ScriptRuntime::ResultadoCarga r;
        r.nombre = item.script->rutaFuente();
        r.ok = item.script->aplicarCarga(item.owner);
        r.mensaje = item.script->ultimoError();
        resultadosCarga_.push_back(std::move(r));
        ++cargaHecha_;
        ++indiceCarga_;
    }
    if (indiceCarga_ >= colaCompilacion_.size()) {
        limpiarColaCompilacion();
        compilacionEnCurso_ = false;
        cargaActual_.clear();
    } else {
        faseCarga_ = FaseCarga::Mostrar;
        cargaActual_ = colaCompilacion_[indiceCarga_].script->rutaFuente();
    }
}

void GameScene::limpiarColaCompilacion() {
    for (const CargaPendiente& item : colaCompilacion_)
        if (item.script) item.script->setAplazarCarga(false);
    colaCompilacion_.clear();
    indiceCarga_ = 0;
    faseCarga_ = FaseCarga::Mostrar;
    compilacionEnCurso_ = false;
    cargaActual_.clear();
}

void GameScene::descargarScripts() {
    limpiarColaCompilacion();
    auto* gameObjects = getGameObjectsScene();
    if (!gameObjects || gameObjects->isEmpty()) return;
    Position<GameObject*>* pos = gameObjects->first();
    while (pos && pos->getElement()) {
        if (Script* script = pos->getElement()->getComponent<Script>())
            script->liberarComportamiento();
        pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
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

    // Luces CPU de la pasada (misma semantica que GL_LIGHT0..7): la escena
    // recoge y el SceneRenderer las aplica al backend y a los uniforms.
    LightData luces[LightSystem::kMaxLights];
    int numLuces = 0;
    lightSystem.collectLights(getGameObjectsScene(), luces, numLuces);

    SceneRenderer::FrameContext ctx;
    ctx.gameObjects = getGameObjectsScene();
    ctx.apariencia = &apariencia;
    ctx.deltaTime = deltaTime;
    ctx.editorActivo = isEditorActivo();
    ctx.selectedObject =
        editorController ? editorController->getSelectedObject() : nullptr;
    ctx.lights = luces;
    ctx.lightCount = numLuces;
    ctx.globalAmbient = lightSystem.getGlobalAmbient();
    ctx.framebufferWidth = fbW;
    ctx.framebufferHeight = fbH;

    // Pasada de la escena (vistas previas + pasada principal + diag) en la
    // capa de Rendering.
    sceneRenderer->render(ctx, activeCameraObject, camara);

    // Matrices de la vista activa: las necesita el gizmo (la pasada principal
    // ya las aplico por su cuenta; aca se recalculan para ImGuizmo).
    float view[16], projection[16];
    camara->getViewMatrix(view);
    camara->getProjectionMatrix(projection,
                                static_cast<float>(fbW) /
                                    static_cast<float>(fbH));

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
    // como contexto. Un GizmoTarget externo (SettingsCollider*) tiene
    // prioridad. Si no hay target externo, el transfor del collider con su
    // gizmo habilitado toma prioridad sobre el del objeto: asi el checkbox
    // "Gizmo activo" del transform del collider (via SettingsTransform)
    // activa/dormita el gizmo del offset del collider cuando quieras.
    GizmoTarget target;
    if (editorController && editorController->hasGizmoTarget()) {
        target = editorController->getGizmoTarget();
        // Refrescar el contexto global del duenio cada frame: si el objeto o
        // sus ancestros se movieron, el parentGlobal almacenado quedaria
        // desactualizado y el offset local se recompondria contra una base
        // vieja (el puntero en si es estable: globalTransformCache del owner).
        if (target.owner)
            target.parentGlobal = target.owner->getGlobalTransform();
    } else if (selected) {
        // Collider offset primero: si su transform local tiene el gizmo
        // encendido se edita el offset; si no, el transform del objeto.
        if (Collider* collider = selected->getComponent<Collider>()) {
            Transform* colliderTransform = collider->getTransform();
            if (colliderTransform && colliderTransform->gizmoHabilitado) {
                target.local = colliderTransform;
                target.parentGlobal = selected->getGlobalTransform();
                target.owner = selected;
            }
        }
        if (!target.local) {
            Transform* objectTransform = selected->getComponent<Transform>();
            if (objectTransform && objectTransform->gizmoHabilitado) {
                target.local = objectTransform;
                Entity* parentEnt = selected->getParentEntity();
                target.parentGlobal =
                    parentEnt ? parentEnt->getGlobalTransform() : nullptr;
                target.owner = selected;
            }
        }
    }

    if (target.local && gizmoOperation != 0) {
        // Matriz que maniula el gizmo: parentGlobal * local. Para translate y
        // rotate ImGuizmo EXPLOTA con matrices escaladas (no-ortonormales):
        // con el objeto o su padre escalado, el objeto sale disparado al usar
        // el gizmo. Por eso se desescala antes de pasarla y se reinserta la
        // escala al leer el resultado.
        float scaleVec[3] = {1.0f, 1.0f, 1.0f};
        const bool sinEscala = gizmoOperation != ImGuizmo::SCALE;

        float localArr[16];
        buildMatrixFromTransform(target.local, localArr);
        glm::mat4 mFull = glm::make_mat4(localArr);
        if (target.parentGlobal) {
            float parentArr[16];
            buildMatrixFromTransform(target.parentGlobal, parentArr);
            mFull = glm::make_mat4(parentArr) * mFull;
        }

        // Si la matriz de entrada ya es no finita (transform del objeto o de
        // algun ancestro corrupto), NO se opera el gizmo con ella: un drag la
        // escribiria tal cual y quedaria -nan en el transform. Se sigue con el
        // resto del frame con el gizmo apagado (es seguro: solo se dibuja).
        if (matrizNoFinita(mFull)) {
            gizmoReady = false;
            return;
        }

        if (sinEscala) {
            // Ortonormalizar zoom: guardar escala por columna y normalizar.
            glm::vec4 c0 = mFull[0];
            glm::vec4 c1 = mFull[1];
            glm::vec4 c2 = mFull[2];
            scaleVec[0] = glm::length(c0);
            scaleVec[1] = glm::length(c1);
            scaleVec[2] = glm::length(c2);
            // Ojo: el guard < 0.0001 NO atrapa NaN (toda comparacion con NaN
            // es false). Sin isfinite, una escala NaN se divide por si misma y
            // contamina toda la matriz.
            for (int i = 0; i < 3; ++i) {
                if (!std::isfinite(scaleVec[i]) || scaleVec[i] < 0.0001f)
                    scaleVec[i] = 1.0f;
            }
            mFull[0] = c0 / scaleVec[0];
            mFull[1] = c1 / scaleVec[1];
            mFull[2] = c2 / scaleVec[2];
        }

        float matrix[16];
        const float* ptr = glm::value_ptr(mFull);
        for (int i = 0; i < 16; ++i) matrix[i] = ptr[i];

        // Sistema de coordenadas del gizmo: LOCAL (= ejercicio historico, los ejes
        // rotan con el objeto) o GLOBAL/WORLD (ejes del mundo fijos, el gizmo
        // NO rota con el objeto). En WORLD el redondeo de la matriz se hace
        // igual contra inv(parentGlobal), asi que ambos conviven sin tocar la
        // escritura de vuelta al local.
        const ImGuizmo::MODE modoGizmo =
            gizmoGlobal ? ImGuizmo::WORLD : ImGuizmo::LOCAL;
        ImGuizmo::Manipulate(view, projection,
                             static_cast<ImGuizmo::OPERATION>(gizmoOperation),
                             modoGizmo, matrix, nullptr,
                             nullptr, nullptr, nullptr);
        gizmoReady = true;
        if (ImGuizmo::IsUsing()) {
            // Reinsertar la escala que quitamos: M = M' * diag(scale).
            glm::mat4 mManip = glm::make_mat4(matrix);
            if (sinEscala) {
                glm::mat4 sMat = glm::scale(
                    glm::mat4(1.0f), glm::vec3(scaleVec[0], scaleVec[1], scaleVec[2]));
                mManip = mManip * sMat;
            }

            // Escribir de vuelta AL local: newLocal = inv(parentGlobal) * matrix
            Transform* localTransform = target.local;
            glm::mat4 newLocal = mManip;
            if (target.parentGlobal) {
                float parentGlobalArr[16];
                buildMatrixFromTransform(target.parentGlobal, parentGlobalArr);
                glm::mat4 invParentGlobal = glm::inverse(glm::make_mat4(parentGlobalArr));
                // glm::inverse de una matriz singular/no finita produce Inf/NaN.
                if (matrizNoFinita(glm::make_mat4(parentGlobalArr)) ||
                    matrizNoFinita(invParentGlobal)) {
                    return;
                }
                newLocal = invParentGlobal * mManip;
            }

            // El resultado del drag no debe corromper el transform con -nan:
            // si la matriz manipulada quedo no finita, se descarta este frame.
            if (matrizNoFinita(mManip) || matrizNoFinita(newLocal)) return;
            float localMatArr[16];
            const float* ptr2 = glm::value_ptr(newLocal);
            for (int i = 0; i < 16; ++i) localMatArr[i] = ptr2[i];
            decomposeMatrixToTransform(localMatArr, localTransform);

            // Congelar hijos SOLO al editar el transform de un objeto; el
            // offset local de un componente (collider) no arrastra hijos.
            const bool esObjeto =
                target.owner && target.owner->getComponent<Transform>() == target.local;
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

bool GameScene::isGizmoGlobal() const noexcept {
    return gizmoGlobal;
}

void GameScene::setGizmoGlobal(bool global) noexcept {
    gizmoGlobal = global;
}

bool GameScene::isGizmoCapturingInput() const {
    return gizmoReady && (ImGuizmo::IsOver() || ImGuizmo::IsUsing());
}

bool GameScene::gizmoInUse() const {
    return gizmoReady && ImGuizmo::IsUsing();
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

float GameScene::getSensibilidadMovimientoCamara() const noexcept {
    return sensibilidadMovimientoCamara;
}

void GameScene::setSensibilidadMovimientoCamara(float sensibilidad) noexcept {
    if (sensibilidad > 0.0f) sensibilidadMovimientoCamara = sensibilidad;
}

const Apariencia& GameScene::getApariencia() const noexcept {
    return apariencia;
}

void GameScene::setApariencia(const Apariencia& valor) noexcept {
    apariencia = valor;
}

bool GameScene::getVentanaCamarasAbierta() const noexcept {
    return ventanaCamarasAbierta;
}

void GameScene::setVentanaCamarasAbierta(bool abierta) noexcept {
    ventanaCamarasAbierta = abierta;
}

void GameScene::setMenuActivo(bool activo) noexcept { menuActivo = activo; }

bool GameScene::isEditorActivo() const {
    return menuActivo ||
           (selecteableGUI && selecteableGUI->getReturnableEntity() != nullptr);
}

void GameScene::clearSelection() {
    if (selecteableGUI) selecteableGUI->setReturnableEntity(nullptr);
}

