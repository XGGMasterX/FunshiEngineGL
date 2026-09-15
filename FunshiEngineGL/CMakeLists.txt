cmake_minimum_required(VERSION 3.15)
project(FunshiEngineGL LANGUAGES CXX)

# Configuración básica
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Configuración de rutas
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Búsqueda de archivos fuente
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp ${SRC_DIR}/*.c)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS ${SRC_DIR}/*.h ${SRC_DIR}/*.hpp)

# El engine es opcional: CI puede compilar SOLO las pruebas headless del
# subsistema FileManager (sin la pila grafica) en cualquier plataforma, con
# BUILD_ENGINE=OFF. Por defecto se construye igual que siempre.
option(BUILD_ENGINE "Build engine executable" ON)

if(BUILD_ENGINE)
    # Ejecutable principal
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_compile_definitions(${PROJECT_NAME} PRIVATE GLM_ENABLE_EXPERIMENTAL)

# 1. Configuración de GLFW
find_package(glfw3 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# 2. Configuración de OpenGL y GLU (VERSIÓN CORREGIDA)
find_package(OpenGL REQUIRED)

# Primero enlazamos OpenGL básico
if(TARGET OpenGL::OpenGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::OpenGL)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
endif()

# Configuración ESPECÍFICA para GLU (requerido en tu caso)
if(UNIX AND NOT APPLE)
    # Método 1: Buscar GLU como componente de OpenGL
    find_package(OpenGL QUIET COMPONENTS GLU)

    if(TARGET OpenGL::GLU)
        message(STATUS "Found GLU via OpenGL components")
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GLU)
    else()
        # Método 2: Búsqueda manual de GLU
        find_library(GLU_LIBRARY
            NAMES GLU MesaGLU
            PATHS /usr/lib /usr/local/lib /usr/lib/x86_64-linux-gnu
        )

        if(GLU_LIBRARY)
            message(STATUS "Found GLU library: ${GLU_LIBRARY}")
            target_link_libraries(${PROJECT_NAME} PRIVATE ${GLU_LIBRARY})
        else()
            # Método 3: Enlazar directamente si todo falla
            message(STATUS "Trying direct GLU linking")
            target_link_libraries(${PROJECT_NAME} PRIVATE GLU)
        endif()
    endif()
endif()

# Bullet Physics
find_package(Bullet REQUIRED)

# Si Bullet define targets modernos
if(TARGET Bullet::BulletDynamics)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        Bullet::BulletDynamics
        Bullet::BulletCollision
        Bullet::LinearMath
    )
else()
    # Método clásico: uso de variables
    target_include_directories(${PROJECT_NAME} PRIVATE ${BULLET_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${BULLET_LIBRARIES}
    )
endif()

# --- Sanitizers --------------------------------------------------------------
option(ENABLE_ASAN "Enable AddressSanitizer for Debug builds" ON)

# Asegurate de generar en Debug para mejores trazas:
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

if(ENABLE_ASAN AND (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang"))
  message(STATUS "Building with AddressSanitizer (ASan)")
  # Compilación: ASan + mejores stack traces + símbolos + optimización ligera
  target_compile_options(${PROJECT_NAME} PRIVATE
    -fsanitize=address
    -fno-omit-frame-pointer
    -O1
    -g
  )
  # Enlace: ASan (importante agregar también al linkeo)
  target_link_options(${PROJECT_NAME} PRIVATE
    -fsanitize=address
  )

  # Opcional pero útil: UndefinedBehaviorSanitizer (UBSan)
  # (si molesta en 3rd-party libs, desactivalo)
  target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
  target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
endif()
# ---------------------------------------------------------------------------


# Configuración para Windows
if(WIN32)
    # dbghelp: CapturaBackTrace/SymFromAddr/SymInitialize (RuntimeException).
    # En MinGW los #pragma comment(lib) de MSVC se ignoran, hay que enlazarla.
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 glu32 dbghelp)
endif()

# Dependencias específicas de Linux
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        dl
        X11
        pthread
        Xrandr
        Xi
    )

    # Ncurses
    find_package(Curses REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CURSES_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CURSES_LIBRARIES})
endif()

# ImGui
file(GLOB IMGUI_SRC ${SRC_DIR}/ImGui/*.cpp ${SRC_DIR}/ImGui/backends/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${SRC_DIR}/ImGui ${SRC_DIR}/ImGui/backends)

# ImGuizmo (fuera de src)
file(GLOB IMGUZIMO_SRC ${CMAKE_SOURCE_DIR}/ImGuizmo/*.cpp)
target_sources(${PROJECT_NAME} PRIVATE ${IMGUZIMO_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/ImGuizmo)

# nlohmann/json (vendoriado en External/nlohmann) para la configuracion del
# editor (EditorConfig). Mismamente el nombre .hpp cuelga de External/.
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/External)

# Assimp
option(USE_ASSIMP "Enable Assimp model loading" ON)
if(USE_ASSIMP)
    find_package(assimp REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${ASSIMP_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)
endif()

# GLM: se usa el directorio embebido si existe en el repo; si no (p. ej. en CI
# sin External/), se busca la copia del sistema (libglm-dev en Ubuntu).
set(GLM_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/External/glm")
if(EXISTS "${GLM_INCLUDE_DIR}/glm/glm.hpp")
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLM_INCLUDE_DIR})
else()
    find_package(glm REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)
endif()

endif() # BUILD_ENGINE

# --- Pruebas headless del subsistema FileManager (sin pila gráfica) ---------
# Compilan en cualquier plataforma (solo std C++17 + los headers de ImGui para
# TreeGUI.h) y ejercitan GestorDeArchivos/FileManager/FileSystemWatcher contra
# un proyecto temporal, sin ventanas. Registradas en ctest.
enable_testing()
add_executable(filemanager-tests
    ${SRC_DIR}/GestorDeArchivos/GestorDeArchivos.cpp
    ${SRC_DIR}/GestorDeArchivos/File.cpp
    ${SRC_DIR}/GestorDeArchivos/Carpeta.cpp
    ${SRC_DIR}/FileManager/FileManager.cpp
    ${SRC_DIR}/FileManager/FileSystemWatcher.cpp
    ${SRC_DIR}/ExcepcionesCPP/RuntimeException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/FileManagerTests.cpp
)
target_include_directories(filemanager-tests PRIVATE ${SRC_DIR}/ImGui)
set_target_properties(filemanager-tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
add_test(NAME filemanager-tests COMMAND filemanager-tests)

# EditorConfig (JSON tolerante) tambien se prueba headless y en las tres
# plataformas: verifica round-trip y la carga ante ausencia/corrupcion.
add_executable(configuracion-tests
    ${SRC_DIR}/Configuracion/EditorConfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/EditorConfigTests.cpp
)
target_include_directories(configuracion-tests PRIVATE ${CMAKE_SOURCE_DIR}/External)
set_target_properties(configuracion-tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
add_test(NAME configuracion-tests COMMAND configuracion-tests)

# AssetManager (cache Flyweight de meshes CPU) tambien headless y portatil:
# rutas (AssetPath), geometria (Mesh) y el registro compartido con un loader
# artificial, sin Assimp ni pila grafica.
add_executable(assetmanager-tests
    ${SRC_DIR}/Assets/AssetManager.cpp
    ${SRC_DIR}/Assets/Mesh.cpp
    ${SRC_DIR}/Matematicas/Vector.cpp
    ${SRC_DIR}/ExcepcionesCPP/RuntimeException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/AssetManagerTests.cpp
)
target_include_directories(assetmanager-tests PRIVATE ${SRC_DIR})
set_target_properties(assetmanager-tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
add_test(NAME assetmanager-tests COMMAND assetmanager-tests)

# TextureManager (cache Flyweight de imagenes CPU) headless y portatil al
# igual que assetmanager-tests: registro compartido + loader artificial, sin
# stb_image (StbImageLoader es del engine, aqui no entra la pila grafica).
add_executable(texturemanager-tests
    ${SRC_DIR}/Assets/TextureManager.cpp
    ${SRC_DIR}/ExcepcionesCPP/RuntimeException.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/TextureManagerTests.cpp
)
target_include_directories(texturemanager-tests PRIVATE ${SRC_DIR})
set_target_properties(texturemanager-tests PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
add_test(NAME texturemanager-tests COMMAND texturemanager-tests)

if(WIN32)
    # dbghelp: CapturaBackTrace/SymFromAddr/SymInitialize (RuntimeException).
    # En MinGW los #pragma comment(lib) de MSVC se ignoran, hay que enlazarla.
    target_link_libraries(filemanager-tests PRIVATE dbghelp)
    target_link_libraries(assetmanager-tests PRIVATE dbghelp)
    target_link_libraries(texturemanager-tests PRIVATE dbghelp)
endif()
