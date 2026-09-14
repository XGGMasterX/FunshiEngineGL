# Estructura y relaciones del proyecto

## 1. Descripción general

`FunshiEngineGL` es un editor/motor gráfico 3D en C++17. El ejecutable combina:

- Ventana y contexto OpenGL mediante GLFW.
- Renderizado inmediato con OpenGL/GLU (pipeline de compatibilidad).
- Interfaz de editor con Dear ImGui y gizmos con ImGuizmo.
- Jerarquía de entidades basada en árboles enlazados propios.
- Simulación física mediante Bullet Physics detrás de una fachada desacoplada.
- Carga de modelos 3D mediante Assimp.
- Serialización binaria de escenas en preorden (jerarquía completa).
- EventBus para notificaciones desacopladas entre subsistemas.
- Máquina de estados explícita (`ApplicationStateMachine`).
- Explorador de archivos del proyecto con fachada propia y vigilancia de cambios.
- Configuración del editor persistida en JSON (`EditorConfig`).
- Soporte para scripts dinámicos (`.so`/`.dll`) via `IScriptBehaviour`.
- Estructuras de datos genéricas propias y jerarquía de excepciones.

El punto de entrada es `FunshiEngineGL/src/main.cpp`. La configuración de compilación
está en `FunshiEngineGL/CMakeLists.txt`; también existe una solución de Visual Studio
(`FunshiEngineGL.sln`). Las pruebas headless viven en `tests/` y el CI en
`.github/workflows/ci.yml`.

---

## 2. Árbol de archivos relevante

Se omiten los directorios generados por el build (`build/`, `build_temp/`), los
archivos temporales de editor (`*.swp`, `*.swo`) y el contenido interno de las
dependencias de terceros.

```text
FunshiEngineGL/                          ← raíz del repo
├── README.md                            ← visión general, build, controles y pendientes
├── PROJECT_STRUCTURE.md                 ← este documento
├── CAMARAS_VISTAS_PREVIAS.md            ← Fase 2: cámaras componente + vistas previas
├── FunshiEngineGL.sln                   ← solución Visual Studio (Windows)
├── .github/workflows/ci.yml             ← CI: engine en Ubuntu + pruebas en Linux/Win/macOS
├── .gitignore / .gitattributes
├── tests/
│   ├── FileManagerTests.cpp             ← pruebas headless del explorador de archivos
│   └── EditorConfigTests.cpp            ← pruebas headless de la configuración JSON
└── FunshiEngineGL/                      ← proyecto CMake principal
    ├── CMakeLists.txt                   ← GLOB de fuentes, dependencias, sanitizers,
    │                                      pruebas (CTest) y opción BUILD_ENGINE
    ├── FunshiEngineGL.vcxproj(.filters) ← proyecto de Visual Studio (Windows)
    ├── Imagenes/                        ← íconos del explorador (cpp, cubo, file, folder, hpp)
    ├── ImGuizmo/                        ← dependencia integrada (ImGuizmo.cpp/.h, etc.)
    ├── External/nlohmann/json.hpp       ← nlohmann/json vendoriado (EditorConfig)
    └── src/
        ├── main.cpp                     ← composition root: ventanas, callbacks, bucle, config
        ├── Time.h / Time.cpp            ← delta time y limitador de FPS
        ├── Ventana.h / Ventana.cpp      ← inicialización GLFW
        ├── Behaviour/
        │   └── IScriptBehaviour.h       ← interfaz de scripts dinámicos (onStart, onUpdate)
        ├── Configuracion/
        │   └── EditorConfig.h/.cpp      ← persistencia JSON de la configuración (menú + GUI)
        ├── Entity/
        │   ├── Entity.h                 ← base: lista de componentes, Transform, serialización
        │   └── Entity.cpp
        ├── Estructuras/                 ← contenedores genéricos, header-only
        │   ├── Comparable.h / Ordenadora.h
        │   ├── ListasEnlazadas/
        │   │   ├── ListasDoblementeEnlazada/ListaDE.h
        │   │   ├── ListasConPrioridad/PriorityListaDE.h
        │   │   └── PositionList.h
        │   ├── MetodosDeOrdenamiento/ListMergeSort.h
        │   ├── Nodos/DNodo.h + TNodo.h  ← nodos de listas y de árbol
        │   ├── Position/Position.h
        │   └── Trees/
        │       ├── Tree.h
        │       └── ArbolesEnlazados/
        │           ├── ArbolEnlazado.h  ← árbol n-ario: jerarquía de GameObjects
        │           └── ArbolBinarioEnlazado.h + Heap/ (Heap, MinHeap, MaxHeap)
        ├── Events/
        │   ├── EventBus.h               ← pub/sub tipado con token de suscripción
        │   └── EventBus.cpp
        ├── ExcepcionesCPP/              ← Throwable, RuntimeException, excepciones de
        │                                  contenedores (ExcepcionesEstructuras/)
        ├── Fisicas/
        │   ├── IPhysicsBackend.h        ← contrato Strategy del backend de física
        │   ├── PhysicsEngine.h/.cpp     ← fachada PIMPL; el header no expone Bullet
        │   └── BulletPhysicsAdapter.h/.cpp ← adaptador concreto de Bullet (RAII)
        ├── FileManager/
        │   ├── FileManager.h/.cpp       ← fachada del explorador: modelo + operaciones
        │   ├── FileSelection.h          ← estado de navegación compartido entre vistas
        │   └── FileSystemWatcher.h/.cpp ← vigilancia de cambios externos (inotify)
        ├── GestorDeArchivos/            ← Binario (streams binarios), File, Carpeta,
        │                                  GestorDeArchivos (exploración del filesystem)
        ├── Gizmo/
        │   └── Gizmo.h                  ← placeholder (la manipulación real vive en ImGuizmo)
        ├── GUI/
        │   ├── GeneralUserInterface.h/.cpp ← interfaz base de paneles ImGui
        │   ├── WindowNames.h
        │   ├── DockSpaceGUI/               ← dock principal del editor
        │   ├── FileManagerGUI/             ← TreeFilesInterface + ContentFolderInterface
        │   │                                  (vistas del explorador; conversan con FileManager)
        │   ├── MenusGUI/                   ← paquete del menú de inicio (MVP); ver su README.md
        │   │   ├── MenuModel.h/.cpp        ← lógica pura sin ImGui/GLFW
        │   │   ├── MenuView.h/.cpp         ← dibujo ImGui
        │   │   ├── StartMenuPresenter.h/.cpp ← puente motor ↔ modelo
        │   │   └── MenuGUI.h/.cpp          ← fachada pública del paquete
        │   ├── ObjetosGUI/
        │   │   ├── SettingsObjectInterface.h/.cpp ← inspector: crea Settings* por componente
        │   │   ├── SettingsComponent.h
        │   │   ├── Transform/SettingsTransform.h/.cpp ← transform + checkbox "Gizmo activo"
        │   │   ├── Color/  Model/  Script/
        │   │   ├── Material/SettingsMaterial.*  Light/SettingsLight.*
        │   │   ├── Camera/SettingsCamera.*       ← FOV, planos, velocidad, vista previa
        │   │   ├── RigidBody/SettingsRigidBody.*
        │   │   └── Colliders/ (Esfera, Cubo, Malla) ← sync transform/shape con física
        │   └── SceneGUI/
        │       ├── SceneSelectedInterface.h/.cpp  ← jerarquía y selección; usa EditorController
        │       ├── SceneObjectTree.h/.cpp         ← árbol de objetos (con drag & drop)
        │       └── SceneMenuBarInterface.h/.cpp   ← barra Play/Stop/acciones de escena (flag `start`)
        ├── GUIManager/
        │   └── GUIManager.h/.cpp         ← fábrica y registro de ventanas; posee FileManager
        ├── Herramientas/
        │   ├── TypeUtils.h/.cpp          ← nombres legibles de tipos
        │   ├── PathUtils.h               ← PATH_SEP multiplataforma compartido
        │   ├── MaterialPresets.h         ← presets de Material como datos (reemplaza a materiales.h)
        │   ├── TreeGUI/TreeGUI.h         ← widget de árbol genérico para ImGui
        │   └── IconosGUI/                ← carga de íconos con stb_image
        ├── Iluminacion/
        │   └── LightSystem.h/.cpp        ← dueño del estado GL de luces (GL_LIGHT0..7) por frame
        ├── ImGui/                        ← Dear ImGui v1.x integrado (+ backends glfw/opengl3)
        ├── Matematicas/
        │   ├── StructVec3.h              ← vec3 propio
        │   └── Vector.cpp/.h
        ├── Rendering/
        │   └── RenderTarget.h/.cpp       ← render a textura (FBO) para vistas previas de cámara
        ├── Objetos/
        │   ├── GameObject.h/.cpp         ← id, nombre, estado, update, serialización binaria
        │   ├── GameObjectFactory.h/.cpp
        │   ├── Modelos3D.h/.cpp          ← carga Assimp y dibujo GL inmediato
        │   ├── Malla.h                   ← placeholder de malla
        │   └── Componentes/
        │       ├── Component.h           ← interfaz base polimórfica (serialize/deserialize)
        │       ├── ComponentFactory.h/.cpp ← creación por nombre (GUI y deserialización)
        │       ├── Transform.h/.cpp      ← local/global con GLM; anti-NaN; flag gizmoHabilitado
        │       ├── CameraComponent.h/.cpp ← cámara componente: vista, FPS, flag de vista previa
        │       ├── Material.h/.cpp       ← AMBIENT/DIFFUSE/SPECULAR/EMISSION/SHININESS
        │       ├── Light.h/.cpp          ← luz puntual serializable
        │       ├── Color.h / Model.h / Script.h / Phisics.h
        │       ├── RigidBody/RigidBody.h/.cpp ← cuerpo Bullet sincronizado (RAII)
        │       └── Colliders/
        │           ├── Collider.h/.cpp   ← base abstracta; radio; gizmo del collider (GizmoTarget)
        │           ├── EsfereCollider.*  ← btSphereShape
        │           ├── CubeCollider.*    ← btBoxShape (half extents = radio)
        │           └── MallaCollider.*   ← btConvexHullShape a partir de la malla
        └── Scenes/
            ├── GameScene.h/.cpp          ← coordinador del frame: render, GUI, física, gizmo, previews
            ├── SceneRegistry.h/.cpp      ← ownership único (unique_ptr) + árbol + vista lineal
            ├── EditorController.h/.cpp   ← mutaciones + GizmoTarget + registro de física
            ├── SceneSerializer.h/.cpp    ← save/load binario preorden con marcadores =>/<=
```

---

## 3. Flujo de ejecución

```text
main.cpp
  ├── Ventana (GLFW init)
  ├── ImGui init (backends glfw + opengl3; imgui.ini junto al proyecto)
  ├── GUIManager (crea el menú y las ventanas; posee FileManager)
  ├── GameScene(guiManager)
  │   ├── SceneRegistry            ← ownership de objetos
  │   ├── PhysicsEngine (PIMPL)    ← mundo Bullet vía BulletPhysicsAdapter
  │   ├── EditorController         ← mutaciones, gizmo, registro de física
  │   ├── SceneSerializer          ← persistencia
  │   └── EventBus                 ← notificaciones
  ├── EditorConfig                 ← carga JSON y aplica a menú/GUI/escena
  ├── ApplicationStateMachine      ← MainMenu / Editing / Playing / Exiting
  └── bucle principal
      ├── glfwPollEvents
      ├── Time::update (deltaTime)
      ├── ImGui::NewFrame
      ├── refleja el estado del menú en la fachada MenuGUI (guardia de cambio)
      ├── si Playing → phisics.stepSimulation(dt) (solo con start==true)
      ├── dibujarGameObjects (OpenGL inmediato)
      ├── gizmo ImGuizmo sobre el objetivo activo (objeto o collider)
      ├── GUI() de GameScene (paneles) + vistas previas de cámaras (FBO)
      ├── ImGui::Render + swap buffers
      └── al salir: saveScene + guardar EditorConfig
```

`main.cpp` es el composition root y registra los callbacks de teclado y ratón sobre
GLFW (`MiAPP`). El estado del menú lo gobierna el modelo del paquete `MenuGUI`,
sincronizado por frame desde `ApplicationStateMachine`. `GameScene` configura
internamente `GUIManager`, `SceneRegistry`, `EditorController`, `SceneSerializer` y
`EventBus`.

---

## 4. Relaciones entre los módulos

### Aplicación y escena

- `main.cpp` construye los objetos principales y conecta sus referencias. Es el único composition root.
- `GameScene` posee mediante `unique_ptr`: `SceneRegistry`, `PhysicsEngine`,
  `EditorController` y `SceneSerializer`. Recibe el `GUIManager` por inyección.
- `SceneRegistry` es el único propietario de los `GameObject`, mediante
  `std::vector<std::unique_ptr<GameObject>>`. El árbol y la lista son vistas no propietarias.
- `EditorController` recibe el registro, la física y el EventBus por inyección.
  Centraliza crear, eliminar, reparentar y modificar componentes; también es el
  **registro central de física** (alta/baja de `RigidBody`) y el dueño del
  `GizmoTarget` (objeto u offset de collider manipulado por el gizmo).
  Publica eventos en `EventBus` después de cada operación exitosa.
- `SceneRegistry::reparent` valida el movimiento: rechaza la raíz, el auto-reparento
  y cualquier ancestro del nuevo padre (no se pueden crear ciclos).
- `EventBus` implementa suscripción tipada mediante tokens (`size_t`). Soporta
  `ObjectCreated`, `ObjectDeleted`, `ObjectReparented`, `ComponentChanged`,
  `SceneCleared` y `ObjectSelected`. No es global: vive dentro de `GameScene`.
- `LightSystem` es el dueño del estado GL de luces: cada frame escanea los objetos,
  toma los componentes `Light` y parametriza los slots `GL_LIGHT0..7`. No queda
  lógica de luz en el bucle ni en los componentes.
- `ApplicationStateMachine` modela los estados `MainMenu`, `Editing`, `Playing` y
  `Exiting`. Las transiciones se realizan desde `main.cpp`; conviven con flags de
  UI legados (`menuActivo`, `start`) con roles documentados.

### Entidades, objetos y componentes

- `Entity` contiene una `ListaDE<Component*>` y un `Transform` base. Define la interfaz abstracta de serialización (métodos `serialize*`/`deserialize*`).
- `GameObject` extiende `Entity` e implementa `Comparable<GameObject>`. Añade:
  id entero, nombre (`inputName[25]`), color auxiliar (`auxColor`), estado (`bool`),
  ciclo `update(dt)` y la lógica de serialización binaria concreta.
- `Modelos3D` extiende `GameObject`. Carga geometría (vértices, normales, índices)
  mediante Assimp, la dibuja con `glBegin/glEnd` (OpenGL inmediato) y serializa
  adicionalmente la ruta del archivo del modelo.
- Los componentes concretos son: `Transform`, `Color`, `Model`, `Material`, `Light`,
  `CameraComponent`, `Script`, `EsfereCollider`, `CubeCollider`, `MallaCollider` y
  `RigidBody`. Todos heredan de `Component` y serializan sus datos binarios.
- `ComponentFactory` centraliza la creación por nombre de tipo tanto desde la GUI
  como durante la deserialización (`"CameraComponent"` acepta el alias `"Camera"`).
- `Transform` provee transformaciones locales y globales usando GLM; expone
  descomposición de matrices para ImGuizmo y blinda su cadena contra NaN
  (valores no finitos no entran al estado ni al render).
- La cadena física: los colliders crean la `btCollisionShape` (reconstruida si
  cambia el radio), `RigidBody` la conecta al mundo de `PhysicsEngine`, y el sync
  de transformaciones entre objeto, collider y cuerpo se hace con matrices
  compuestas (global T del dueño + offset del collider), no con deltas sueltos.

### Jerarquía y contenedores

- `ArbolEnlazado<GameObject*>` representa la jerarquía padre-hijo dentro de
  `SceneRegistry`. No libera los objetos apuntados (la ownership está en el vector).
- `ListaDE<GameObject*>` es una vista lineal reconstruible de `SceneRegistry`,
  usada por GUI, render, iluminación y actualización. Tampoco es propietaria.
- `TNodo` contiene sus hijos mediante una `ListaDE<Position<E>*>`;
  `DNodo` implementa los nodos de listas doblemente enlazadas.
- Las estructuras genéricas son header-only para permitir la instanciación de plantillas.

### GUI

- `GeneralUserInterface` define la interfaz común (`initGUI`, `contentGUI`, `printGUI`, `endGUI`).
- `GUIManager` actúa como fábrica y registro centralizado de todas las ventanas
  del editor. Expone métodos tipados (`getMenuGUI`, `getTreeFilesGUI`, etc.),
  guarda/restaura el estado abierto/cerrado de las ventanas y **posee el
  `FileManager`** del proyecto.
- El explorador de archivos es una arquitectura de tres piezas: `FileManager`
  (fachada dueña del modelo `GestorDeArchivos` y de las operaciones de dominio),
  `FileSelection` (estado de navegación compartido) y las vistas
  `TreeFilesInterface`/`ContentFolderInterface`, que solo conversan con la fachada.
  `FileSystemWatcher` avisa de cambios externos (inotify) para re-escanear.
- `SceneSelectedInterface` observa la escena inyectada y delega las
  operaciones de edición a `EditorController`. Usa `GameObjectFactory` para crear objetos.
- `SceneObjectTree` dibuja el árbol de objetos (con drag & drop para reparentar).
- `SettingsObjectInterface` inspecciona un `GameObject` seleccionado y crea/actualiza
  los paneles `Settings*` específicos de cada componente presente. El checkbox
  **"Gizmo activo"** de `SettingsTransform` enciende/apaga el gizmo de ese
  transform (del objeto o del offset del collider) sin deseleccionar.
- `SceneMenuBarInterface` gestiona la barra de menú de escena y comunica el
  estado Play/Stop mediante un `bool*` (`start`) que consume `GameScene`.
- `MenuGUI` es la fachada del paquete `MenusGUI` (menú principal): `MenuModel`
  (lógica pura) + `MenuView` (ImGui) + `StartMenuPresenter` (puente motor).
  Ver `src/GUI/MenusGUI/README.md`.

### Persistencia

- `SceneSerializer` es el único responsable del archivo `BBDDObjetos.txt` y de los
  `.db` individuales. Guarda en preorden: escribe el binario de cada objeto y
  registra la ruta; inserta `=>` para abrir un bloque de hijos y `<=` para cerrarlo.
- La carga reconstruye la escena **de forma recursiva** (`loadPreOrder` con
  look-ahead de marcadores), restaurando padres e hijos; inserta cada objeto
  directamente en `SceneRegistry` vía `EditorController`.
- `Binario` encapsula los streams binarios usados por las entidades.
- `GameObject::saveEntity/loadEntity` coordina la serialización binaria propia
  (atributos globales, locales, componentes).
- `EditorConfig` (JSON via nlohmann) persiste la configuración del editor:
  menú (proyecto, idioma, sensibilidad de cámara), gizmo, ventana de cámaras y
  ventanas (estado abierto/cerrado de GUIManager), en `~/MotorGrafico/Configuracion.json`
  (Linux) / `C:/MotorGraficoArchivos/Configuracion.json` (Windows). Tolerante a
  archivos ausentes o corruptos: los defaults quedan en `EditorConfig.h`.
  El layout `imgui.ini` también se guarda junto al proyecto (no en el CWD).
- Limitación conocida: la serialización binaria no tiene versionado ni validación
  de tamaños; un cambio en la estructura de atributos invalida escenas guardadas.

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
| Bullet Physics | Física y colisiones (btDiscreteDynamicsWorld) | `find_package(Bullet)` con fallback a targets clásicos |
| Assimp | Carga de modelos 3D (opcional via `USE_ASSIMP`) | `find_package(assimp)` |
| GLM | Matemáticas de Transform y cámaras (`GLM_ENABLE_EXPERIMENTAL`) | Usa `External/glm` si existe; si no, `find_package(glm)` del sistema |
| nlohmann/json | `EditorConfig` (persistencia de configuración) | Vendoriado en `External/nlohmann` |
| Dear ImGui | Interfaz del editor | Integrada en `src/ImGui/` |
| ImGuizmo | Gizmos de transformación | Integrada en `FunshiEngineGL/ImGuizmo/` |
| ncurses / X11 | Enlace en Linux | `find_package(Curses)` + X11/Xrandr/Xi |

En Windows se enlaza además `opengl32`, `glu32` y `dbghelp` (stack traces de
`RuntimeException`).

---

## 6. Organización del código

La convención general es un par `.h`/`.cpp` por clase. Las excepciones son:

- Estructuras de datos genéricas, que deben ser header-only para la instanciación de plantillas.
- Componentes pequeños y algunas interfaces GUI, parcialmente inline por el acoplamiento actual.
- Headers de terceros, que conservan la organización original.

`CMakeLists.txt` descubre automáticamente los `.cpp` y `.h` bajo `src/` mediante
`GLOB_RECURSE CONFIGURE_DEPENDS`. Los nuevos archivos de implementación se incorporan
al build sin enumerarlos manualmente. Los archivos de ImGui se recopilan por separado
desde `src/ImGui/` y los de ImGuizmo desde `ImGuizmo/` (fuera de `src/`).

Además del ejecutable, el proyecto define dos **targets de prueba headless**
registrados en CTest:

- `filemanager-tests`: ejercita `GestorDeArchivos`/`FileManager`/`FileSystemWatcher`
  contra un proyecto temporal, sin ventanas ni pila gráfica.
- `configuracion-tests`: round-trip del JSON de `EditorConfig` y carga tolerante
  ante archivos ausentes o corruptos.

La opción `BUILD_ENGINE=OFF` compila solo las pruebas (útil en CI y plataformas
sin las librerías gráficas), y `ENABLE_ASAN` (ON por defecto en Debug) activa
ASan+UBSan en GCC/Clang.

---

## 7. Flujo de datos principal

```text
main.cpp
  │
  ├─ input GLFW ──► MiAPP::onKey/onMouse ──► CameraComponent activa (movimiento)
  │                                       ──► tecla E: toggleEditorInterfaces()
  │                                       ──► 1/T, 2/R, 3/Y: operación del gizmo
  │                                       ──► Escape: volver al menú (máquina de estados)
  │
  ├─ GameScene::GUI()
  │     ├─ SceneMenuBarInterface ──► flag start (Play/Stop) ──► GameScene::update
  │     ├─ SceneSelectedInterface ──► EditorController (crear/borrar/reparentar GO)
  │     │                         ──► EventBus.publish(ObjectCreated/Deleted/Selected)
  │     ├─ SceneObjectTree ──► selección y reparentado por drag & drop
  │     └─ GUIManager paneles
  │           └─ SettingsObjectInterface ──► EditorController (add/removeComponent,
  │                                          sync colliders, "Gizmo activo")
  │
  ├─ GameScene::update(dt)
  │     ├─ transición editor→play: empuja la pose visual a los cuerpos Bullet
  │     ├─ si start y gizmo libre: PhysicsEngine::stepSimulation(dt)
  │     │               └─ btDiscreteDynamicsWorld::stepSimulation
  │     └─ scripts: IScriptBehaviour::onUpdate (si compilados)
  │
  └─ GameScene::gameScene()
        ├─ LightSystem::beginFrame() [glLight*]
        ├─ dibujarGameObjects (Modelos3D con glBegin/glEnd)
        ├─ marcadores de luz y cámara (wireframes auxiliares)
        ├─ ImGuizmo::Manipulate sobre el GizmoTarget activo (objeto o collider)
        ├─ dibujarViewportsPrevios (FBO de cámaras) + paneles ImGui
        └─ pickObject con el mouse para seleccionar en el viewport
```

---

## 8. Patrones aplicados y límites actuales

| Patrón | Aplicación actual | Efecto arquitectónico |
|---|---|---|
| RAII / ownership explícito | `unique_ptr` en `GameScene`, `SceneRegistry`, `GUIManager`, `FileManager`; colliders y cuerpos Bullet gestionados por `Collider`/`RigidBody` | Propietarios claros y liberación automática. |
| Composite | Árbol n-ario de `GameObject` en `ArbolEnlazado` | Modela la jerarquía padre-hijo y las transformaciones anidadas. |
| Registry / Repository | `SceneRegistry` | Centraliza búsqueda, ownership y operaciones de escena. |
| Controller | `EditorController` | La GUI no coordina mutaciones directamente; la física se registra aquí. |
| Factory | `ComponentFactory`, `GameObjectFactory` | Reduce `new` concretos en GUI y serialización. |
| Facade | `PhysicsEngine` (PIMPL) y `FileManager` | Ocultan Bullet y el filesystem detrás de operaciones del dominio. |
| Adapter | `BulletPhysicsAdapter` | Permite sustituir el backend de física sin modificar `PhysicsEngine`. |
| Strategy | `IPhysicsBackend` | Contrato de backend inyectado por constructor (con nullptr se puede montar la fachada sin mundo). |
| MVP | Paquete `MenusGUI` (`MenuModel`/`MenuView`/`StartMenuPresenter`) | Lógica del menú testeable, independiente de ImGui/GLFW. |
| Observer | `EventBus` (token-based) | Desacopla notificaciones de escena y selección entre subsistemas. |
| State | `ApplicationStateMachine` | Explicita los modos principales de la aplicación. |

No están implementados todavía:
- `Command` para undo/redo.
- `Prototype` para duplicación y prefabs.
- `AssetManager / Flyweight` para compartir recursos de forma eficiente.
- `Visitor` para inspectores extensibles.

---

## 9. Observaciones de arquitectura

- `SceneRegistry` establece ownership único mediante `std::vector<unique_ptr<GameObject>>`.
  GUI, selección, física y vistas lineales solo mantienen referencias raw no propietarias.
- `PhysicsEngine` es una fachada PIMPL con el backend inyectado (`IPhysicsBackend`):
  el header no expone Bullet y la fachada puede existir sin mundo (útil para pruebas
  y para el editor sin simulación).
- `GameScene` sigue siendo un coordinador amplio: mezcla ciclo de frame, render,
  GUI, física, gizmo y vistas previas. La extracción de `SceneRenderer`,
  `PhysicsSystem` y `ScriptSystem` aliviaría esa responsabilidad.
- `ApplicationStateMachine` explicita los estados, pero las transiciones se realizan
  desde `main.cpp` y conviven con flags de UI legados (`menuActivo`, `start`).
- `EventBus` transporta `GameObject*` crudos. Un consumidor no debe conservar esos
  punteros después de recibir `ObjectDeleted`. `EditorController::deleteGameObject`
  publica el evento **antes** de destruir el objeto, para que los observadores
  invaliden sus referencias a tiempo (fix de un use-after-free histórico).
- La manipulación del gizmo pausa `stepSimulation` mientras el usuario arrastra y
  la física solo corre en Play; el sync collider↔rigidbody↔objeto usa la matriz
  global compuesta del dueño, de modo que mover un collider no desincroniza el cuerpo.
- Las rutas de usuario (`~/MotorGrafico`, `C:/MotorGraficoArchivos`) están
  centralizadas en `EditorConfig` para la configuración y el layout, pero los assets
  del proyecto todavía se resuelven a mano; `PathUtils.h` solo comparte el separador.
- La serialización binaria no tiene versionado ni validación formal de tamaños. Un
  cambio en la estructura de atributos invalida escenas guardadas.
- `SettingsObjectInterface` y algunos componentes todavía incluyen y construyen
  detalles concretos; el siguiente paso de desacoplamiento es completar el uso de
  `EditorController` y descriptors de componentes.
- El renderer usa OpenGL de compatibilidad (`glBegin`, `glMatrixMode`, `glLight*`);
  no es compatible con un contexto OpenGL Core.

---

## 10. Capas de la aplicación

```text
Aplicación:      main.cpp, MiAPP, ApplicationStateMachine, GUIManager, EditorConfig
Dominio:         GameScene, SceneRegistry, EditorController, GameObject, Component,
                 EventBus, FileManager, LightSystem
Infraestructura: BulletPhysicsAdapter, OpenGL/GLFW (Ventana), Assimp (Modelos3D),
                 Binario/SceneSerializer, Script (dlopen/LoadLibrary), FileSystemWatcher
Dependencias:    ImGui, ImGuizmo, nlohmann/json, GLM (integradas o vendoriadas)
```

Relaciones clave:

```text
GUI → EditorController → SceneRegistry
                       └─ PhysicsEngine → IPhysicsBackend → BulletPhysicsAdapter → Bullet
                       └─ EventBus → suscriptores (SceneSelectedInterface, etc.)
Vistas del explorador → FileManager → GestorDeArchivos / FileSystemWatcher
SceneSerializer → SceneRegistry / EditorController
GameScene → coordina todos los subsistemas del frame
```

---

## 11. Pruebas y CI

- `tests/FileManagerTests.cpp`: construcción y re-resolución del árbol de archivos,
  operaciones de dominio (crear, renombrar, copiar, eliminar, búsqueda) y
  `FileSystemWatcher` (detección de cambios externos, en Linux via inotify).
- `tests/EditorConfigTests.cpp`: round-trip del JSON y tolerancia a archivos
  ausentes o corruptos.
- Ambos targets compilan en cualquier plataforma y se ejecutan con `ctest`.
- `.github/workflows/ci.yml` compila el engine completo en Ubuntu (Release, sin
  ASan) y ejecuta las pruebas; además ejecuta las pruebas headless en
  Linux/Windows/macOS con `BUILD_ENGINE=OFF`.

Los bugs de la Fase 2 (cámaras/vistas previas) y sus fixes están documentados en
[CAMARAS_VISTAS_PREVIAS.md](CAMARAS_VISTAS_PREVIAS.md).

---

## 12. Pendientes conocidos

- [ ] Sistema de animaciones.
- [ ] Texturas y asset manager compartido.
- [ ] Scripts dinámicos completos: compilación en caliente, `onStart`/`onUpdate`, `SerializeField`.
- [ ] `CommandManager` para undo/redo.
- [ ] Cuadro de log de errores en el editor.
- [ ] Resolver IDs duplicados al crear objetos; limpiar binarios huérfanos al eliminar.
- [ ] Clase `Input` independiente (hoy el input vive en callbacks de `main.cpp`).
- [ ] Terminar los popups del inspector.
- [ ] Prefabs y duplicación de objetos.
- [ ] Portabilidad de rutas de assets (centralizar `HOME` / rutas de Windows).
- [ ] Versionado y validación de la serialización binaria.
- [ ] Extraer `SceneRenderer`, `PhysicsSystem` y `ScriptSystem` de `GameScene`.
- [ ] Encapsular las estructuras internas de `SceneRegistry` (eliminar getters raw de compatibilidad).
- [ ] Vistas previas de cámara seleccionables con clic (hoy son pasivas).
- [ ] Limpiar el `glEndList()` huérfano en `GameScene::mallaScene`.
