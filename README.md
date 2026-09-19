# FunshiEngineGL

Motor y editor 3D en tiempo real escrito en C++17, con interfaz ImGui y renderizado OpenGL (pipeline de compatibilidad) construido desde cero.

---

## Características actuales

- Ventana y contexto OpenGL con **GLFW**; renderizado con **OpenGL / GLU** (pipeline inmediato).
- Interfaz de editor con **Dear ImGui** (docking) y gizmos con **ImGuizmo**.
- Sistema **Entity–Component**: `Transform`, `Color`, `Model`, `Material`, `Light`, `CameraComponent`, colliders (esfera / cubo / malla), `RigidBody` y `Script`.
- **Scripts dinámicos** (`Script` + `IScriptBehaviour`): reflexión por macros con campos `SerializeField` (escalares, arrays y grupos anidados) editables en el inspector; compilación en caliente de C++ a `.so`/`.dll` (`BackendCpp`) y soporte opcional de **Java vía JNI** (`BackendJava`, se activa con `-DFUNSHI_JAVA=ON`). Hot reload por fecha de modificación que reinyecta los valores serializados, y ciclo `onStart`/`onUpdate`/`onStop`.
- **Jerarquía de objetos** con árbol enlazado propio (`ArbolEnlazado<GameObject*>`) y reparentado seguro (rechaza ciclos y la raíz).
- Carga de modelos 3D con **Assimp** (`.obj`, `.fbx` y formatos soportados por Assimp).
- **Física con Bullet** detrás de una fachada desacoplada (`PhysicsEngine` → `IPhysicsBackend` → `BulletPhysicsAdapter`): solo simula en modo Play, sincroniza transformaciones entre objeto, collider y cuerpo, y admite un gizmo dedicado para el collider activo.
- **Serialización binaria de escenas** en preorden con marcadores `=>`/`<=`: guarda y recupera la jerarquía completa (padres e hijos) de forma recursiva.
- **EventBus** con suscripción tipada (creación, eliminación, reparentado, selección y cambios de componentes).
- **Máquina de estados** de la aplicación: `MainMenu`, `Editing`, `Playing`, `Exiting`.
- Explorador de archivos del proyecto con fachada propia (`FileManager`), estado de navegación compartido (`FileSelection`) y vigilancia de cambios externos (`FileSystemWatcher`).
- **Apariencia del editor configurable** (perfil persisto en `Configuracion.json`):
  tema claro/oscuro, **modo blanco y negro** que acompana al fondo y la grilla
  del viewport, color de acento de la interfaz y color de fondo de la escena,
  aplicados en vivo por `TemaEditor`/`AparienciaUtil`.
- La **cámara activa** elegida con "Usar" se persiste por id en la configuración
  (default automática si el id ya no existe al cargar).
- **Cámaras como componente** con vistas previas en vivo (render a FBO) — ver [CAMARAS_VISTAS_PREVIAS.md](CAMARAS_VISTAS_PREVIAS.md).
- **Iluminación** gestionada por `LightSystem` (slots `GL_LIGHT0..7`, marcadores de luz y cámara en escena) y **materiales** con presets (`MaterialPresets`).
- Menú de inicio modular (paquete `MenusGUI`, patrón MVP): idioma, nombre del proyecto y sensibilidad de cámara.
- **Configuración del editor persistida en JSON** (`EditorConfig`, nlohmann/json): proyecto, idioma, sensibilidad, gizmo activo, ventana de cámaras y estado de las ventanas. Se guarda junto al proyecto (`~/MotorGrafico/Configuracion.json` en Linux / `C:/MotorGraficoArchivos/Configuracion.json` en Windows); tolerante a archivos ausentes o corruptos.
- **Caché de assets compartida** (Flyweight): `AssetManager` (meshes CPU) y `TextureManager` (imágenes), con rutas normalizadas (`AssetPath`) y loader inyectable.
- Estructuras de datos propias (listas, árboles, heaps, mergesort) y jerarquía de excepciones propia: las usadas por el motor (`ListaDE`, `ArbolEnlazado`, `PriorityListaDE`) están corregidas y verificadas, y las implementadas para el motor (`MinHeap`, `MaxHeap`, `ListMergeSort`, `ArbolBinarioEnlazado`) cuentan con pruebas headless.
- **Pruebas headless** (`tests/`, CTest) y **CI multiplataforma** en GitHub Actions.
- Build reproducible en **Linux** y **Windows** (y pruebas en macOS) con **CMake**; ASan+UBSan por defecto en Debug.

---

## Requisitos

| Dependencia    | Versión mínima | Notas                                                        |
|----------------|----------------|--------------------------------------------------------------|
| CMake          | 3.15           |                                                              |
| Compilador     | C++17          | GCC / Clang / MSVC                                           |
| GLFW           | 3.x            | `libglfw3-dev`                                               |
| OpenGL / GLU   | cualquiera     | `libglu1-mesa-dev`                                           |
| Bullet Physics | 3.x            | `libbullet-dev`                                              |
| Assimp         | 5.x            | `libassimp-dev`                                              |
| GLM            | 0.9.9+         | Del sistema (`libglm-dev`); el CMake usa `External/glm` si existe y cae al sistema si no |
| nlohmann/json  | —              | Vendoriado en `FunshiEngineGL/External/nlohmann`             |
| ncurses        | cualquiera     | `libncurses-dev` (solo Linux)                                |
| X11            | —              | `libx11-dev`, `libxrandr-dev`, `libxi-dev`                   |
| JDK            | 17+            | Solo para scripts **Java** (`-DFUNSHI_JAVA=ON`); opcional en runtime via `JAVA_HOME` |

### Instalar dependencias en Ubuntu/Debian

```bash
sudo apt install cmake build-essential ninja-build \
    libglfw3-dev libglu1-mesa-dev libglm-dev \
    libbullet-dev libassimp-dev \
    libncurses-dev libx11-dev \
    libxrandr-dev libxi-dev
```

(Es el mismo conjunto que instala el job de CI de Ubuntu.)

---

## Compilar y ejecutar

```bash
# Clonar el repositorio
git clone <url-del-repo>
cd FunshiEngineGL

# Configurar (Debug con ASan/UBSan por defecto)
cmake -B build -S FunshiEngineGL

# Compilar
cmake --build build -j$(nproc)

# Ejecutar (Linux)
./build/FunshiEngineGL
```

Build de Release más rápido (sin sanitizers):

```bash
cmake -B build -S FunshiEngineGL -DCMAKE_BUILD_TYPE=Release -DENABLE_ASAN=OFF
```

En Windows la misma receta funciona con el generador de Visual Studio; también existe la solución `FunshiEngineGL.sln`.

> El primer arranque crea su configuración en `~/MotorGrafico/` (Linux) o `C:/MotorGraficoArchivos/` (Windows): ahí viven la escena serializada, `Configuracion.json` y el layout `imgui.ini` del editor.


---

## Pruebas y CI

Las pruebas son headless (sin pila gráfica) y corren con CTest:

```bash
cmake --build build --target filemanager-tests configuracion-tests assetmanager-tests texturemanager-tests estructuras-tests
ctest --test-dir build --output-on-failure
```

- `filemanager-tests`: explorador de archivos (`GestorDeArchivos`/`FileManager`/`FileSystemWatcher`).
- `configuracion-tests`: `EditorConfig` (JSON tolerante + round-trip).
- `assetmanager-tests` y `texturemanager-tests`: caches Flyweight de meshes e imágenes.
- `estructuras-tests`: listas, árboles, heaps y ordenamiento propios (87 verificaciones).

Con `-DBUILD_ENGINE=OFF` se compilan **solo** las pruebas: no se requieren GLFW/OpenGL/Bullet/Assimp y funcionan en cualquier plataforma. `.github/workflows/ci.yml` hace exactamente eso en Linux, Windows y macOS, además de un build completo del engine en Ubuntu.

---

## Estructura del repositorio

```text
FunshiEngineGL/            ← raíz del repo
├── README.md
├── PROJECT_STRUCTURE.md   ← arquitectura detallada
├── CAMARAS_VISTAS_PREVIAS.md ← cámaras componente + vistas previas (Fase 2)
├── FunshiEngineGL.sln     ← solución Visual Studio (Windows)
├── .github/workflows/     ← CI (build del engine + pruebas multiplataforma)
├── tests/                 ← pruebas headless (FileManager, EditorConfig, Assets, Estructuras)
└── FunshiEngineGL/        ← proyecto principal
    ├── CMakeLists.txt
    ├── ImGuizmo/          ← dependencia externa integrada
    ├── External/          ← nlohmann/json vendoriado
    ├── Imagenes/          ← íconos del editor
    └── src/               ← todo el código fuente
        ├── main.cpp       ← composition root y bucle principal
        ├── Assets/        Behaviour/  Entity/  Estructuras/  Events/
        ├── ExcepcionesCPP/  Fisicas/  FileManager/  GestorDeArchivos/
        ├── Configuracion/ GUI/  GUIManager/  Herramientas/  Iluminacion/
        ├── ImGui/  Matematicas/  Rendering/  Objetos/
        ├── Scenes/  States/  Ventana.*  EngineTime.*
```

Ver **PROJECT_STRUCTURE.md** para la descripción completa de cada módulo, las relaciones entre clases, el flujo de ejecución, las pruebas y los pendientes.

---

## Controles del editor

| Tecla / Acción          | Función                                                       |
|-------------------------|---------------------------------------------------------------|
| `W` / `A` / `S` / `D`   | Mover la cámara activa (con diagonales)                        |
| `Espacio` / `Shift izq` | Subir / bajar la cámara                                        |
| Mouse (sin UI capturada)| Navegación FPS de la cámara activa (sensibilidad de Opciones)  |
| `E`                     | Mostrar / ocultar las interfaces del editor                    |
| `Escape`                | Volver al menú de inicio                                       |
| `1` o `T`               | Gizmo: traslación                                              |
| `2` o `R`               | Gizmo: rotación                                                |
| `3` o `Y`               | Gizmo: escala                                                  |
| Clic en objeto          | Seleccionar objeto en el viewport                              |

> El modo Play/Stop se controla desde la barra de menú de la escena; la física solo simula en Play.

---

## Roadmap / Pendientes conocidos

- [ ] Sistema de animaciones.
- [x] Scripts dinámicos: `SerializeField` con reflexión, compilación en caliente de C++ (`BackendCpp`) y Java opcional vía JNI (`BackendJava`, `-DFUNSHI_JAVA=ON` + JDK en build), ciclo `onStart`/`onUpdate`/`onStop` y hot reload.
- [ ] `CommandManager` para undo/redo.
- [ ] Cuadro de log de errores en el editor.
- [ ] Resolver IDs duplicados al crear objetos; limpiar binarios huérfanos al eliminar.
- [ ] Clase `Input` independiente (hoy el input vive en callbacks de `main.cpp`).
- [ ] Terminar los popups del inspector; prefabs y duplicación de objetos.
- [ ] Portabilidad de rutas de assets (centralizar `HOME` / rutas de Windows).
- [ ] Versionado y validación de la serialización binaria.
- [ ] Extraer `SceneRenderer`/`PhysicsSystem`/`ScriptSystem` de `GameScene`; vistas previas seleccionables con clic.

---

## Licencia

Copyright 2026 Gianfranco Ivan Enrique

El código propio del motor se distribuye bajo la **Apache License 2.0** (ver
[`LICENSE`](LICENSE) y [`NOTICE`](NOTICE)).

Las bibliotecas de terceros integradas (Dear ImGui, ImGuizmo, nlohmann/json,
stb_image, GLM) conservan sus licencias originales —principalmente MIT— y no
están cubiertas por la Apache License 2.0. Consulta
[`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md) para el detalle completo de
licencias, titulares y ubicaciones.
