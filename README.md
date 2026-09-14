# FunshiEngineGL

Motor y editor 3D en tiempo real escrito en C++17, con interfaz ImGui y renderizado OpenGL (modo fijo de compatibilidad). El proyecto está orientado a aprendizaje, experimentación y desarrollo incremental de un editor tipo Unity/Godot construido desde cero.

---

## Características actuales

- Ventana y contexto OpenGL a través de **GLFW**.
- Renderizado con **OpenGL / GLU** (modo inmediato).
- Interfaz de editor completa con **Dear ImGui**.
- Gizmos de transformación en el viewport con **ImGuizmo** (`W` traslación, `E` rotación, `R` escala).
- Sistema de **entidades y componentes** (Entity–Component).
- **Jerarquía de objetos** representada con un árbol enlazado propio (`ArbolEnlazado<GameObject*>`).
- Carga de modelos 3D con **Assimp** (`.obj`, `.fbx` y otros formatos soportados por Assimp).
- **Simulación física** con Bullet Physics (RigidBody, SphereCollider, BoxCollider, MeshCollider).
- **Serialización binaria** de escenas: guarda y carga jerarquía completa con preorden + marcadores `=>`/`<=`.
- **EventBus** para notificaciones desacopladas entre subsistemas (creación, eliminación, selección de objetos).
- **Máquina de estados** de la aplicación: `MainMenu`, `Editing`, `Playing`, `Exiting`.
- Explorador de archivos integrado (`TreeFilesInterface` / `ContentFolderInterface`).
- Soporte para **scripts dinámicos** (`.so` / `.dll`) via `IScriptBehaviour`.
- Iluminación básica OpenGL (`Ilumination`, `glLight*`).
- Cámara FPS navegable desde el editor.
- Cámaras como componente y **vistas previas en vivo** por cámara (render a FBO + ventanas ImGui) — ver [CAMARAS_VISTAS_PREVIAS.md](CAMARAS_VISTAS_PREVIAS.md).
- **Configuración del editor persistida en JSON** (`EditorConfig`, nlohmann/json): proyecto, idioma, sensibilidad de cámara y estado de las ventanas ImGui. Archivo junto al proyecto: `~/MotorGrafico/Configuracion.json` (Linux) / `C:/MotorGraficoArchivos/Configuracion.json` (Windows).
- Estructuras de datos propias: listas doblemente enlazadas, árboles enlazados binarios, listas con prioridad, métodos de ordenamiento (mergesort).
- Jerarquía de excepciones propia (herencia de `Throwable`).
- Build reproducible en **Linux** y **Windows** con **CMake**.
- AddressSanitizer y UBSan habilitados por defecto en builds de Debug.

---

## Requisitos

| Dependencia   | Versión mínima | Notas                              |
|---------------|----------------|------------------------------------|
| CMake         | 3.15           |                                    |
| GCC / Clang   | C++17          |                                    |
| GLFW          | 3.x            | `libglfw3-dev`                     |
| OpenGL / GLU  | cualquiera     | `libglu1-mesa-dev`                 |
| Bullet Physics| 3.x            | `libbullet-dev`                    |
| Assimp        | 5.x            | `libassimp-dev`                    |
| GLM           | 0.9.9+         | Incluido en `FunshiEngineGL/External/glm` |
| ncurses       | cualquiera     | `libncurses-dev` (solo Linux)      |
| X11           | —              | `libx11-dev`, `libxrandr-dev`, `libxi-dev` |

### Instalar dependencias en Ubuntu/Debian

```bash
sudo apt install cmake build-essential \
    libglfw3-dev libglu1-mesa-dev \
    libbullet-dev libassimp-dev \
    libncurses-dev libx11-dev \
    libxrandr-dev libxi-dev
```

---

## Compilar y ejecutar

```bash
# Clonar el repositorio
git clone <url-del-repo>
cd FunshiEngineGL

# Configurar con CMake (Debug con ASan activado por defecto)
cd FunshiEngineGL          # directorio del proyecto CMake
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Compilar
make -j$(nproc)

# Ejecutar
./FunshiEngineGL
```

Para desactivar AddressSanitizer (build de Release más rápido):

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_ASAN=OFF
```

> El binario compilado se genera como `build/FunshiEngineGL`. También existe `build_temp/` con un build previo funcional de referencia.

---

## Estructura del repositorio

```
FunshiEngineGL/            ← raíz del repo
├── README.md
├── PROJECT_STRUCTURE.md   ← arquitectura detallada
├── CAMARAS_VISTAS_PREVIAS.md ← Fase 2: cámaras componente + vistas previas (bugs y fixes)
├── FunshiEngineGL.sln     ← solución Visual Studio (Windows)
├── FunshiEngineGL/        ← proyecto principal
│   ├── CMakeLists.txt
│   ├── ImGuizmo/          ← dependencia externa integrada
│   ├── Imagenes/          ← íconos del editor
│   ├── build_temp/        ← build de referencia
│   └── src/               ← todo el código fuente
│       ├── main.cpp
│       ├── Behaviour/
│       ├── Entity/
│       ├── Estructuras/
│       ├── Events/
│       ├── ExcepcionesCPP/
│       ├── Fisicas/
│       ├── GestorDeArchivos/
│       ├── Gizmo/
│       ├── GUI/
│       ├── GUIManager/
│       ├── Herramientas/
│       ├── Iluminacion/
│       ├── ImGui/
│       ├── Matematicas/
│       ├── Objetos/
│       ├── Scenes/
│       └── States/
└── FunshiEngineGL_BACKUP/ ← snapshot de respaldo
```

Ver **PROJECT_STRUCTURE.md** para la descripción completa de cada módulo, las relaciones entre clases, el flujo de ejecución y la evaluación arquitectónica.

---

## Controles del editor

| Tecla / Acción       | Función                                        |
|----------------------|------------------------------------------------|
| `W` / `A` / `S` / `D` | Mover cámara adelante / izquierda / atrás / derecha |
| `Espacio`            | Subir cámara                                   |
| `Shift izquierdo`    | Bajar cámara                                   |
| `E`                  | Activar / desactivar menú principal            |
| `Escape`             | Detener la escena (volver a edición)           |
| Gizmo `W`            | Modo traslación                                |
| Gizmo `E`            | Modo rotación                                  |
| Gizmo `R`            | Modo escala                                    |
| Clic en objeto       | Seleccionar objeto en el viewport              |

---

## Roadmap / Pendientes conocidos

Extraído de los comentarios del código fuente (`main.cpp`):

- [ ] Agregar sistema de animaciones.
- [ ] Implementar materiales y texturas.
- [ ] Sistema de scripts dinámicos completo (compilación, `onStart`, `onUpdate`, `SerializeField`).
- [ ] `CommandManager` para undo/redo.
- [ ] Cargar el árbol de jerarquía al hacer `Load Scene`.
- [ ] Cuadro de log de errores en el editor.
- [ ] Evitar crash al anidar un hijo a su propio ancestro.
- [ ] Limpiar binarios huérfanos al eliminar objetos.
- [ ] Resolver ID duplicados al crear objetos.
- [ ] Sistema de seguimiento de scripts asociado a git.
- [ ] Agregar clase `Input` independiente.
- [ ] Terminar todos los popups del inspector.
- [ ] Soporte para prefabs y duplicación de objetos.
- [ ] Portabilidad de rutas de assets (centralizar resolución de `HOME` / Windows paths).
- [ ] Versionado y validación de la serialización binaria.

---

## Licencia

Proyecto personal / educativo. Sin licencia formal por el momento.
