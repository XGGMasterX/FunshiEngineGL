# Estructura y relaciones del proyecto

## 1. Descripción general

`FunshiEngineGL` es un editor/motor gráfico 3D en C++17. El ejecutable combina:

- Ventana y contexto OpenGL mediante GLFW.
- Renderizado inmediato con OpenGL/GLU.
- Interfaz de editor con Dear ImGui.
- Manipulación de objetos y componentes.
- Jerarquía de entidades basada en árboles enlazados propios.
- Simulación física mediante Bullet Physics.
- Carga de modelos 3D mediante Assimp.
- Serialización binaria de escenas con preorden.
- EventBus para notificaciones desacopladas entre subsistemas.
- Máquina de estados explícita (`ApplicationStateMachine`).
- Soporte para scripts dinámicos (`.so`/`.dll`) via `IScriptBehaviour`.
- Estructuras de datos genéricas propias y jerarquía de excepciones.

El punto de entrada es `FunshiEngineGL/src/main.cpp`. La configuración de compilación está en `FunshiEngineGL/CMakeLists.txt`; también existe una solución de Visual Studio (`FunshiEngineGL.sln`).

---

## 2. Árbol de archivos relevante

Se omiten los archivos generados de `build/`, los archivos temporales de editor (`*.swp`, `*.swo`) y el contenido interno de las dependencias de terceros.

```text
FunshiEngineGL/                          ← raíz del repo
├── README.md
├── PROJECT_STRUCTURE.md
├── FunshiEngineGL.sln                   ← solución Visual Studio (Windows)
├── imgui.ini
├── .gitignore
├── .gitattributes
└── FunshiEngineGL/                      ← proyecto CMake principal
    ├── CMakeLists.txt
    ├── FunshiEngineGL.vcxproj
    ├── FunshiEngineGL.vcxproj.filters
    ├── Imagenes/                        ← íconos del editor (cpp.png, cubo.png, file.png, folder.png, hpp.png)
    ├── ImGuizmo/                        ← dependencia externa integrada
    │   ├── ImGuizmo.cpp/.h
    │   ├── ImCurveEdit.cpp/.h
    │   ├── ImGradient.cpp/.h
    │   ├── ImSequencer.cpp/.h
    │   └── GraphEditor.cpp/.h
    ├── build_temp/                      ← build de referencia funcional
    │   └── FunshiEngineGL               ← binario compilado (~50 MB, ASan)
    └── src/
        ├── main.cpp                     ← arranque, bucle principal, callbacks input
        ├── Time.h / Time.cpp            ← delta time
        ├── Ventana.h / Ventana.cpp      ← inicialización GLFW
        ├── Behaviour/
        │   └── IScriptBehaviour.h       ← interfaz para scripts dinámicos (onStart, onUpdate)
        ├── Entity/
        │   ├── Entity.h                 ← base abstracta: lista de componentes, Transform, serialización
        │   └── Entity.cpp
        ├── Estructuras/                 ← estructuras genéricas, header-only en su mayoría
        │   ├── Comparable.h
        │   ├── Ordenadora.h
        │   ├── ListasEnlazadas/
        │   │   ├── ListasDoblementeEnlazada/ListaDE.h
        │   │   ├── ListasConPrioridad/PriorityListaDE.h
        │   │   └── PositionList.h
        │   ├── MetodosDeOrdenamiento/ListMergeSort.h
        │   ├── Nodos/
        │   │   ├── DNodo.h              ← nodo doble (listas)
        │   │   └── TNodo.h              ← nodo árbol (contiene ListaDE de hijos)
        │   ├── Position/Position.h
        │   └── Trees/
        │       ├── Tree.h
        │       ├── ArbolesEnlazados/ArbolEnlazado.h   ← árbol n-ario enlazado, jerarquía GO
        │       └── ArbolesEnlazados/ArbolBinarioEnlazado.h
        ├── Events/
        │   ├── EventBus.h               ← pub/sub tipado con token de suscripción
        │   └── EventBus.cpp
        ├── ExcepcionesCPP/
        │   ├── Throwable.h
        │   ├── RuntimeException.h/.cpp
        │   ├── BoundaryViolationException.h
        │   ├── ClassCastException.h
        │   ├── InvalidOperationException.h
        │   ├── NullPointerException.h
        │   └── ExcepcionesEstructuras/
        ├── Fisicas/
        │   ├── IPhysicsBackend.h        ← contrato de backend de física
        │   ├── PhysicsEngine.h/.cpp     ← fachada; inicializa mundo Bullet con suelo estático
        │   ├── BulletPhysicsAdapter.h/.cpp ← adaptador concreto de Bullet
        ├── GestorDeArchivos/
        │   ├── Binario.h/.cpp           ← streams binarios para serialización
        │   ├── File.h/.cpp
        │   ├── Carpeta.h/.cpp
        │   └── GestorDeArchivos.h/.cpp  ← exploración de directorios del sistema
        ├── Gizmo/
        │   ├── Camera.h/.cpp            ← cámara FPS: forward/back/left/right/up/down
        │   └── Gizmo.h                  ← placeholder (lógica en ImGuizmo)
        ├── GUI/
        │   ├── GeneralUserInterface.h/.cpp  ← interfaz base de paneles ImGui
        │   ├── FileManagerGUI/              ← explorador de archivos visual
        │   ├── MenusGUI/                    ← menú principal e interfaces de opciones/proyecto
        │   ├── ObjetosGUI/                  ← inspector de objetos y componentes
        │   │   ├── SettingsObjectInterface.h       ← despachador: crea Settings* según el tipo de componente
        │   │   ├── SettingsComponent.h
        │   │   ├── Transform/SettingsTransform.h/.cpp
        │   │   ├── Color/SettingsColor.h
        │   │   ├── Model/SettingsModel.h
        │   │   ├── Script/SettingsScript.h
        │   │   ├── RigidBody/SettingsRigidBody.h
        │   │   └── Colliders/
        │   │       ├── SettingsColliderEsfera.h
        │   │       ├── SettingsColliderCubo.h
        │   │       └── SettingsColliderMalla.h
        │   │   └── Camera/
        │   │       └── SettingsCamera.h/.cpp ← inspector del componente de cámara (FOV, planos, velocidad, vista previa)
        │   └── SceneGUI/
        │       ├── SceneSelectedInterface.h/.cpp  ← panel de jerarquía, selección de objetos
        │       └── SceneMenuBarInterface.h/.cpp   ← barra de menú de escena (play/stop/save/load)
        ├── GUIManager/
        │   ├── GUIManager.h             ← fábrica y registro de todas las ventanas ImGui
        │   └── GUIManager.cpp
        ├── Herramientas/
        │   └── TypeUtils.h              ← utilidades de tipo (nombre legible de tipo)
        ├── Iluminacion/
        │   ├── Ilumination.h
        │   └── Ilumination.cpp          ← configuración de glLight* (luz OpenGL fija)
        ├── ImGui/                       ← Dear ImGui integrado (v1.x)
        │   ├── imgui.cpp/.h
        │   ├── imgui_impl_glfw.cpp/.h
        │   ├── imgui_impl_opengl3.cpp/.h
        │   ├── imgui_draw/widgets/tables.cpp
        │   └── imstb_*.h
        ├── Matematicas/
        │   ├── StructVec3.h             ← struct vec3 propio
        │   └── Vector.cpp
        ├── Rendering/
        │   └── RenderTarget.h/.cpp      ← render a textura (FBO) para vistas previas de cámara (Fase 2)
        ├── Objetos/
        │   ├── Malla.h                  ← placeholder de malla
        │   ├── GameObject.h/.cpp        ← extend Entity; id, nombre, color, serialización, update
        │   ├── GameObjectFactory.h/.cpp ← factory de GameObjects
        │   ├── Modelos3D.h/.cpp         ← extend GameObject; carga Assimp, dibujo GL, serializa ruta
        │   ├── Materiales/
        │   │   ├── Material.h/.cpp
        │   │   └── materiales.h
        │   └── Componentes/
        │       ├── Component.h          ← interfaz base polimórfica de componentes
        │       ├── CameraComponent.h/.cpp ← cámara como componente: vista desde el Transform del dueño, navegación FPS, flag de vista previa (Fase 2)
        │       ├── Phisics.h            ← typedef/struct auxiliar de física
        │       ├── ComponentFactory.h/.cpp ← factory de componentes (usado por GUI y deserialización)
        │       ├── Transform.h          ← posición, rotación, escala; matriz local/global; descomposición GLM
        │       ├── Color.h              ← componente de color auxiliar (serializable)
        │       ├── Model.h              ← referencia al modelo 3D cargado
        │       ├── Script.h             ← carga/compilación de módulos dinámicos (.so/.dll)
        │       ├── RigidBody/
        │       │   └── RigidBody.h      ← conecta Collider con btRigidBody; sincroniza transform
        │       └── Colliders/
        │           ├── Collider.h       ← base abstracta de colliders, dibuja wireframe
        │           ├── EsfereCollider.h ← btSphereShape
        │           ├── CubeCollider.h   ← btBoxShape
        │           └── MallaCollider.h  ← btConvexHullShape (mesh)
        ├── Scenes/
        │   ├── Ilumination.h            ← forward de Ilumination para escenas
        │   ├── GameScene.h/.cpp         ← coordinador de frame: render, GUI, física, gizmo, scripts
        │   ├── SceneRegistry.h/.cpp     ← ownership único de GameObjects; árbol + vista lineal
        │   ├── EditorController.h/.cpp  ← mutaciones de editor: crear/eliminar/reparentar/componentes
        │   ├── SceneSerializer.h/.cpp   ← save/load binario en preorden con marcadores =>/<=
        └── States/
            ├── ApplicationStateMachine.h
            └── ApplicationStateMachine.cpp  ← enum ApplicationState: MainMenu/Editing/Playing/Exiting
```

---

## 3. Flujo de ejecución

```text
main.cpp
  ├── Ventana (GLFW init)
  ├── ImGui init (glfw + opengl3 backend)
  ├── Camera
  ├── GUIManager
  │   ├── MenuInterface
  │   ├── SceneSelectedInterface
  │   ├── SceneMenuBarInterface
  │   ├── TreeFilesInterface
  │   └── ContentFolderInterface
  ├── GameScene(camera, guiManager)
  │   ├── SceneRegistry            ← ownership de objetos
  │   ├── PhysicsEngine            ← mundo Bullet (con suelo estático)
  │   ├── EditorController         ← mutaciones
  │   ├── SceneSerializer          ← persistencia
  │   └── EventBus                 ← notificaciones
  ├── Ilumination (sun)
  ├── ApplicationStateMachine
  └── bucle principal
      ├── glfwPollEvents
      ├── Time::update (deltaTime)
      ├── ImGui::NewFrame
      ├── aplica estado (MainMenu / Editing / Playing / Exiting)
      ├── si Playing → phisics.stepSimulation(dt)
      ├── dibujarGameObjects (OpenGL inmediato)
      ├── dibujar gizmo ImGuizmo si objeto seleccionado
      ├── GUI() de GameScene (paneles ImGui)
      ├── ImGui::Render + swap buffers
      └── si Exiting → break
```

La clase `MiAPP` en `main.cpp` actúa como composition root y registra los callbacks de teclado (`teclado_callback`) y ratón (`mouse_callback`) sobre GLFW. `GameScene` configura internamente `GUIManager`, `SceneRegistry`, `EditorController`, `SceneSerializer` y `EventBus`.

---

## 4. Relaciones entre los módulos

### Aplicación y escena

- `main.cpp` construye los objetos principales y conecta sus referencias. Es el único composition root.
- `GameScene` recibe `Camera` y `GUIManager` y posee mediante `unique_ptr`:
  `SceneRegistry`, `PhysicsEngine`, `EditorController` y `SceneSerializer`.
- `SceneRegistry` es el único propietario de los `GameObject`, mediante
  `std::vector<std::unique_ptr<GameObject>>`. El árbol y la lista son vistas no propietarias.
- `EditorController` recibe el registro, la física y el EventBus por inyección.
  Centraliza crear, eliminar, reparentar y modificar componentes.
  Publica eventos en `EventBus` después de cada operación exitosa.
- `EventBus` implementa suscripción tipada mediante tokens (`size_t`).
  Soporta `ObjectCreated`, `ObjectDeleted`, `ObjectReparented`, `ComponentChanged`,
  `SceneCleared` y `ObjectSelected`. No es global: vive dentro de `GameScene`.
- `Ilumination` es configurada por `main` (`setSun`) y aplicada durante el render.
- `ApplicationStateMachine` modela los estados `MainMenu`, `Editing`, `Playing` y `Exiting`.
  Las transiciones se realizan desde `main.cpp`; convive con flags de UI legados
  (`menuActivo`, `start`).

### Entidades, objetos y componentes

- `Entity` contiene una `ListaDE<Component*>` y un `Transform` base. Define la interfaz abstracta de serialización (métodos `serialize*`/`deserialize*`).
- `GameObject` extiende `Entity` e implementa `Comparable<GameObject>`. Añade:
  id entero, nombre (`inputName[25]`), color auxiliar (`auxColor`), estado (`bool`),
  ciclo `update(dt)` y la lógica de serialización binaria concreta.
- `Modelos3D` extiende `GameObject`. Carga geometría (vértices, normales, índices)
  mediante Assimp, la dibuja con `glBegin/glEnd` (OpenGL inmediato) y serializa
  adicionalmente la ruta del archivo del modelo.
- Los componentes concretos son: `Transform`, `Color`, `Model`, `Script`,
  `EsfereCollider`, `CubeCollider`, `MallaCollider`, `RigidBody`.
  Todos heredan de `Component` (interfaz base polimórfica).
- `ComponentFactory` centraliza la creación de componentes tanto desde la GUI
  como durante la deserialización, reduciendo el acoplamiento con clases concretas.
- `Transform` provee transformaciones locales y globales usando GLM;
  expone descomposición de matrices para ImGuizmo.
- Los colliders crean la `btCollisionShape` correspondiente y `RigidBody`
  la conecta al `btDiscreteDynamicsWorld` de `PhysicsEngine`.

### Jerarquía y contenedores

- `ArbolEnlazado<GameObject*>` representa la jerarquía padre-hijo dentro de
  `SceneRegistry`. No libera los objetos apuntados (la ownership está en el vector).
- `ListaDE<GameObject*>` es una vista lineal reconstruible de `SceneRegistry`,
  usada por GUI, render y actualización. Tampoco es propietaria.
- `TNodo` contiene sus hijos mediante una `ListaDE<Position<E>*>`;
  `DNodo` implementa los nodos de listas doblemente enlazadas.
- Las estructuras genéricas son header-only para permitir la instanciación de plantillas.

### GUI

- `GeneralUserInterface` define la interfaz común (`initGUI`, `contentGUI`, `printGUI`, `endGUI`).
- `GUIManager` actúa como fábrica y registro centralizado de todas las ventanas
  del editor. Expone métodos tipados (`getMenuGUI`, `getTreeFilesGUI`, etc.).
- `SceneSelectedInterface` observa la escena inyectada y delega las
  operaciones de edición a `EditorController`. Usa `GameObjectFactory` para crear objetos.
- `SettingsObjectInterface` inspecciona un `GameObject` seleccionado y crea/actualiza
  los paneles `Settings*` específicos de cada componente presente.
- `TreeFilesInterface` y `ContentFolderInterface` implementan el explorador de archivos visual.
  Usan íconos desde `Imagenes/`.
- `SceneMenuBarInterface` gestiona la barra de menú de escena (Play/Stop/Save/Load).
  Recibe un `bool*` (`start`) para comunicar el estado Play/Stop al `main`.
- `MenuInterface` y sus sub-interfaces gestionan el menú principal (opciones y proyecto).

### Persistencia

- `SceneSerializer` es el único responsable del archivo `BBDDObjetos.txt`.
  Guarda en preorden: escribe el binario de cada objeto y registra la ruta;
  inserta `=>`/`<=` para la jerarquía.
- La carga reconstruye la escena leyendo `BBDDObjetos.txt` y los `.db` individuales
  de forma recursiva; inserta cada objeto directamente en `SceneRegistry`.
- `Binario` encapsula los streams binarios usados por las entidades.
- `GameObject::saveEntity/loadEntity` coordina la serialización binaria propia
  (atributos globales, locales, componentes).
- Limitación conocida: no hay versionado, validación de tamaño ni abstracción de formato.

### Scripts dinámicos

- `Script` (componente) gestiona la carga de módulos `.so`/`.dll` mediante `dlopen`/`LoadLibrary`.
- `IScriptBehaviour` define la interfaz esperada: `onStart(GameObject*)` y `onUpdate(GameObject*, float)`.
- La compilación en caliente y el `SerializeField` están pendientes de implementación completa.

---

## 5. Dependencias externas

| Dependencia | Uso | Integración |
|---|---|---|
| GLFW | Ventana, contexto OpenGL y eventos de input | `find_package(glfw3)` |
| OpenGL / GLU | Renderizado inmediato y utilidades de cámara | `find_package(OpenGL)` + fallback manual a libGLU |
| Bullet Physics | Física y colisiones (btDiscreteDynamicsWorld) | `find_package(Bullet)` |
| Assimp | Importación de modelos 3D (obj, fbx, etc.) | `find_package(assimp)` |
| GLM | Matrices, vectores, descomposición de transformaciones | Headers en `External/glm` |
| Dear ImGui | Interfaz del editor completa | Integrado en `src/ImGui/` |
| ImGuizmo | Gizmos de transformación (translate/rotate/scale) | Integrado en `ImGuizmo/` |
| ncurses | Dependencia auxiliar de Linux | `find_package(Curses)` |
| X11 / Xrandr / Xi | Dependencias del windowing en Linux | Link directo |

`src/ImGui/` y `ImGuizmo/` contienen código de terceros compilado como parte del ejecutable, no como bibliotecas separadas.

---

## 6. Organización `.h` / `.cpp`

El código propio con lógica no genérica está dividido en headers de interfaz y archivos `.cpp` de implementación. Las excepciones son:

- Estructuras de datos genéricas, que deben ser header-only para la instanciación de plantillas.
- Componentes pequeños y algunas interfaces GUI, parcialmente inline por el acoplamiento actual.
- Headers de terceros, que conservan la organización original.

`CMakeLists.txt` descubre automáticamente los `.cpp` y `.h` bajo `src/` mediante
`GLOB_RECURSE CONFIGURE_DEPENDS`. Los nuevos archivos de implementación se incorporan
al build sin enumerarlos manualmente.

Los archivos de ImGui se recopilan por separado desde `src/ImGui/` y los de ImGuizmo
desde `ImGuizmo/` (fuera de `src/`).

---

## 7. Flujo de datos principal

```
main.cpp
  │
  ├─ input GLFW ──► MiAPP::onKey/onMouse ──► Camera (movimiento)
  │                                       ──► menuActivo toggle (tecla E)
  │                                       ──► sceneRunning = false (Escape)
  │
  ├─ GameScene::GUI()
  │     ├─ SceneMenuBarInterface ──► start flag ──► ApplicationStateMachine
  │     ├─ SceneSelectedInterface ──► EditorController (crear/borrar/reparentar GO)
  │     │                         ──► EventBus.publish(ObjectCreated/Deleted/Selected)
  │     └─ GUIManager paneles
  │           └─ SettingsObjectInterface ──► EditorController (add/removeComponent)
  │
  ├─ GameScene::update(dt)
  │     ├─ si Playing: PhysicsEngine::stepSimulation(dt)
  │     │               └─ btDiscreteDynamicsWorld::stepSimulation
  │     └─ scripts: IScriptBehaviour::onUpdate (si compilados)
  │
  └─ GameScene::dibujarGameObjects()
        ├─ Ilumination::apply() [glLight*]
        ├─ Modelos3D::dibujar(dt) [glBegin/glVertex/glEnd]
        └─ ImGuizmo::Manipulate (gizmo sobre objeto seleccionado)
```

---

## 8. Patrones aplicados y límites actuales

| Patrón | Aplicación actual | Efecto arquitectónico |
|---|---|---|
| RAII / ownership explícito | `unique_ptr` en `GameScene`, `SceneRegistry` y `GUIManager` | Define propietarios claros y reduce liberaciones manuales. |
| Composite | Árbol n-ario de `GameObject` en `ArbolEnlazado` | Modela la jerarquía padre-hijo y las transformaciones anidadas. |
| Registry / Repository | `SceneRegistry` | Centraliza búsqueda, ownership y operaciones de escena. |
| Controller | `EditorController` | Evita que la GUI coordine directamente las mutaciones. |
| Factory | `ComponentFactory`, `GameObjectFactory` | Reduce `new` concretos en GUI y serialización. |
| Facade | `PhysicsEngine` | Oculta el mundo Bullet detrás de operaciones del dominio. |
| Adapter | `BulletPhysicsAdapter` | Permite sustituir el backend de física sin modificar `PhysicsEngine`. |
| Observer | `EventBus` (token-based) | Desacopla notificaciones de escena y selección entre subsistemas. |
| State | `ApplicationStateMachine` | Explicita los modos principales de la aplicación. |
| Strategy (parcial) | `IPhysicsBackend` | Contrato de backend, pero actualmente solo existe el adaptador Bullet. |

No están implementados todavía:
- `Command` para undo/redo.
- `Prototype` para duplicación y prefabs.
- `AssetManager / Flyweight` para compartir recursos de forma eficiente.
- `Visitor` para inspectores extensibles.

---

## 9. Observaciones de arquitectura

- `SceneRegistry` establece ownership único mediante `std::vector<unique_ptr<GameObject>>`.
  GUI, selección, física y vistas lineales solo mantienen referencias raw no propietarias.
- `PhysicsEngine` inicializa directamente el mundo Bullet **en el constructor del header**,
  incluyendo la creación del suelo estático. Esto acopla la física al header y dificulta pruebas.
  La fachada `IPhysicsBackend` / `BulletPhysicsAdapter` existe pero `PhysicsEngine` no la usa todavía de forma consistente.
- `GameScene` continúa siendo una fachada demasiado amplia: mezcla ciclo de frame, render,
  GUI, física, scripts y gizmo. La extracción de `SceneRenderer`, `PhysicsSystem` y
  `ScriptSystem` aliviaría esta responsabilidad.
- `ApplicationStateMachine` explicita los estados, pero las transiciones se realizan
  desde `main.cpp` y conviven con flags de UI legados (`menuActivo`, `start`).
- `EventBus` transporta `GameObject*` crudos. Un consumidor no debe conservar esos
  punteros después de recibir `ObjectDeleted`.
- `EditorController::deleteGameObject` publica el evento de eliminación **antes** de
  destruir el objeto, para que los observadores puedan invalidar sus referencias con seguridad.
- Las rutas de assets se resuelven a partir de `HOME` en Linux y rutas específicas en
  Windows; centralizar esta configuración es necesario para la portabilidad.
- La serialización binaria no tiene versionado ni validación formal de tamaños. Un cambio
  en la estructura de atributos invalida escenas guardadas.
- `SettingsObjectInterface` y algunos componentes todavía incluyen y construyen detalles
  concretos; el siguiente paso de desacoplamiento es completar el uso de `EditorController`
  y descriptors de componentes.
- El renderer usa OpenGL fijo (`glBegin`, `glMatrixMode`, `glLight*`), por lo que
  está fuertemente ligado al contexto de compatibilidad. No es compatible con OpenGL Core.

---

## 10. Capas de la aplicación

```text
Aplicación:     main.cpp, MiAPP, ApplicationStateMachine, GUIManager
Dominio:        GameScene, SceneRegistry, EditorController, GameObject, Component, EventBus
Infraestructura: BulletPhysicsAdapter, OpenGL/GLFW (Ventana), Assimp (Modelos3D),
                 Binario/SceneSerializer, Script (dl*)
Dependencias:   ImGui, ImGuizmo, GLM (incluidos en el repo)
```

Relaciones clave:

```text
GUI → EditorController → SceneRegistry
                      └─ PhysicsEngine → IPhysicsBackend → BulletPhysicsAdapter → Bullet
                      └─ EventBus → suscriptores (SceneSelectedInterface, etc.)
SceneSerializer → SceneRegistry / EditorController
GameScene → coordina todos los subsistemas del frame
```

---

## 11. Pendientes conocidos (desde comentarios de `main.cpp`)

- [ ] Agregar sistema de animaciones.
- [ ] Implementar materiales y texturas.
- [ ] Sistema de scripts dinámicos completo (`onStart`, `onUpdate`, `SerializeField`).
- [ ] `CommandManager` para undo/redo.
- [ ] Cargar el árbol de jerarquía al hacer `Load Scene` (actualmente solo carga objetos planos).
- [ ] Cuadro de log de errores en el editor.
- [ ] Evitar crash al anidar un hijo a su propio ancestro (validar en `reparentGameObject`).
- [ ] Limpiar binarios huérfanos (`.db`) al eliminar objetos.
- [ ] Resolver ID duplicados al crear objetos.
- [ ] Sistema de seguimiento de scripts asociado a git.
- [ ] Agregar clase `Input` independiente (actualmente el input está en callbacks de `main.cpp`).
- [ ] Terminar todos los popups del inspector.
- [ ] Soporte para prefabs y duplicación de objetos.
- [ ] Portabilidad de rutas de assets.
- [ ] Versionado y validación de la serialización binaria.
- [ ] Extraer `SceneRenderer`, `PhysicsSystem` y `ScriptSystem` de `GameScene`.
- [ ] Encapsular las estructuras internas de `SceneRegistry` (eliminar getters raw de compatibilidad).
