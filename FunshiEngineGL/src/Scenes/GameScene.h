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
#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <memory>
#include <string>
#include <vector>

#include "../Events/EventBus.h"
#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "../Iluminacion/LightSystem.h"

class CameraComponent;
class EditorController;
class GUIManager;
class GameObject;
class MeshRenderer;
class PhysicsEngine;
class RenderTarget;
class SceneMenuBarInterface;
class SceneRegistry;
class SceneSelectedInterface;
class SceneSerializer;
class AssetManager;

class GameScene {
private:
    GUIManager* managerGUI;
    SceneSelectedInterface* selecteableGUI = nullptr;
    SceneMenuBarInterface* menuBarGUI = nullptr;
    CameraComponent* activeCamera = nullptr;
    GameObject* activeCameraObject = nullptr;
    // Camara elegida con "Usar" en la ventana Camaras. Con nullptr la activa
    // es la primera camara de la escena (o la que se siembra al inicio).
    GameObject* requestedActiveCamera = nullptr;
    int contadorCamaras = 0;
    bool ventanaCamarasAbierta = true;
    std::unique_ptr<SceneRegistry> sceneRegistry;
    std::unique_ptr<PhysicsEngine> phisics;
    std::unique_ptr<EditorController> editorController;
    std::unique_ptr<SceneSerializer> sceneSerializer;
    // Registro central de assets (meshes CPU compartidos). Se inyecta a
    // EditorController y SceneSerializer para que cada Modelos3D pida su
    // geometria al cache (Flyweight) en lugar de parsear Assimp por objeto.
    std::unique_ptr<AssetManager> assetManager;
    // Renderer moderno (VBO/VAO + shader) de los objetos de la escena. Vive
    // aqui porque necesita las matrices de camara y las luces de la pasada; si
    // el pipeline moderno no esta disponible, cada objeto degrada al modo
    // inmediato (fallback preservado).
    std::unique_ptr<MeshRenderer> meshRenderer;
    EventBus events;
    LightSystem lightSystem;
    float deltaTime = 0.0f;
    bool start = false;
    // version anterior de start: detecta la transicion false->true para
    // sincronizar los cuerpos a la pose VISUAL del editor ANTES de que el
    // primer stepSimulation los dispare desde la pose vieja.
    bool previousStart = false;
    bool menuActivo = false;
    // Sensibilidad global del mouse look, sincronizada desde MenuGUI (vista
    // Opciones). La aplica main al offset del raton antes de updateYaw().
    float sensibilidadCamara = 1.0f;
    int gizmoOperation = 7; // ImGuizmo::TRANSLATE
    bool gizmoReady = false;

    // Vista previa viva por camara con el checkbox "Vista previa" (Fase 2).
    // Se reconstruye cada frame: texturas FBO + el objeto que las genera.
    std::vector<std::unique_ptr<RenderTarget>> viewportsCamaras;
    std::vector<GameObject*> viewportsObjetos;
    static constexpr int kPreviewW = 400;
    static constexpr int kPreviewH = 250;

    void dibujarEscena(const float view[16], const float projection[16],
                       GameObject* camaraOjo);
    void dibujarGameObjectsConOjo(GameObject* camaraOjo,
                                  const float view[16],
                                  const float projection[16]);
    void dibujarObjectConOjo(GameObject* object, GameObject* camaraOjo,
                             const float view[16], const float projection[16]);
    void dibujarViewportsPrevios();
    void pintarViewportsGUI();
    // Recoge las luces de la escena (LightSystem::collectLights) y se las
    // pasa al MeshRenderer como uniforms de la pasada en curso.
    void prepararLucesFrame();

public:
    GameScene(GUIManager* managerGUI);
    ~GameScene();

    ListaDE<GameObject*>* getGameObjectsScene();
    void saveScene(const std::string& filename);
    bool isStart();
    void loadScene(const std::string& pathTxt, const std::string& semiPath);
    void dibujarGameObjects();
    void dibujarObject(GameObject* object);
    void dibujarMarcadorLuz(GameObject* object);
    void dibujarMarcadorCamara(GameObject* object);
    void mallaScene(float tam);
    void GUI();
    void pintarVentanaCamaras();
    void update(float deltaTime);
    void gameScene();
    void setGizmoOperation(int operation);
    int getGizmoOperation() const;
    bool isGizmoCapturingInput() const;
    bool gizmoInUse() const;
    GameObject* pickObject(float mouseX, float mouseY);

    // Camara de la escena como Component: devuelve el primer objeto que tenga
    // una CameraComponent (crea "CamaraPrincipal" si la escena no tiene).
    // The navigation FPS (WASD + mouse) opera sobre esta camara.
    CameraComponent* getActiveCamera();

    // Cambia la camara activa (por la que se navega y se ve la escena).
    void setActiveCamera(GameObject* object);

    // Crea un GameObject vacio con camara en la posicion/orientacion de la
    // camara activa, activa su vista previa y la deja seleccionada para
    // ubicarla con el gizmo. Ventana "Camaras" -> "Agregar camara".
    GameObject* agregarCamaraEnVistaActiva();

    // Modo editor: interfaces (gizmo, jerarquia, settings, folders) activas
    // si se aprieta E (toggleEditorInterfaces) o hay un objeto seleccionado.
    void toggleEditorInterfaces();
    bool isEditorActivo() const;
    void clearSelection();

    // Sensibilidad del mouse look (la setea main desde MenuGUI/Opciones).
    float getSensibilidadCamara() const noexcept;
    void setSensibilidadCamara(float sensibilidad) noexcept;

    // Estado de la ventana "Camaras" (persistido por EditorConfig).
    bool getVentanaCamarasAbierta() const noexcept;
    void setVentanaCamarasAbierta(bool abierta) noexcept;
};

#endif
