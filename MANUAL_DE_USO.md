<!--
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    SPDX-License-Identifier: Apache-2.0
-->

# Manual de uso de FunshiEngineGL

Guia practica para usuarios del editor: como montar una escena, asignar assets
por arrastre y escribir scripts con la API exacta que expone el motor.

> Documentacion complementaria: [README.md](README.md) (caracteristicas y
> compilacion), [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) (arquitectura),
> [CAMARAS_VISTAS_PREVIAS.md](CAMARAS_VISTAS_PREVIAS.md) (camaras) y
> [FunshiEngineGL/DOCUMENTACION.md](FunshiEngineGL/DOCUMENTACION.md) (notas
> internas de arquitectura).

## Contenido

1. [Requisitos e inicio rapido](#1-requisitos-e-inicio-rapido)
2. [Proyectos y estructura de carpetas](#2-proyectos-y-estructura-de-carpetas)
3. [Recorrido del editor](#3-recorrido-del-editor)
4. [Objetos y componentes](#4-objetos-y-componentes)
5. [Assets por drag & drop](#5-assets-por-drag--drop)
6. [Fisica](#6-fisica)
7. [Audio](#7-audio)
8. [Interfaces de juego (HUD)](#8-interfaces-de-juego-hud)
9. [Camaras](#9-camaras)
10. [Guardar y abrir escenas](#10-guardar-y-abrir-escenas)
11. [Apariencia y configuracion del editor](#11-apariencia-y-configuracion-del-editor)
12. [Scripts: conceptos y ciclo de vida](#12-scripts-conceptos-y-ciclo-de-vida)
13. [Scripting C++: referencia completa](#13-scripting-c-referencia-completa)
14. [Scripting Java (JNI)](#14-scripting-java-jni)
15. [Hot reload y depuracion](#15-hot-reload-y-depuracion)
16. [Problemas frecuentes](#16-problemas-frecuentes)

---

## 1. Requisitos e inicio rapido

**Dependencias:** CMake >= 3.16, compilador con C++17, OpenGL 2.1+/GLU, GLFW,
GLM, Assimp y Bullet. Java es opcional (solo scripting Java): si el build
encuentra un JDK, se compila el soporte `FUNSHI_JAVA=ON` automaticamente.

```bash
cmake -S FunshiEngineGL -B FunshiEngineGL/build
cmake --build FunshiEngineGL/build -j$(nproc)
./FunshiEngineGL/build/FunshiEngineGL
```

**Primeros pasos:**

1. En el menu de inicio elegi idioma, nombre del proyecto y sensibilidad de
   camara. "Iniciar Estudio" crea el proyecto y sus carpetas automaticamente.
2. Navega la escena con `W`/`A`/`S`/`D`, `Espacio`/`Shift` y el mouse (nav FPS).
   `E` oculta la UI; `Escape` vuelve al menu.

---

## 2. Proyectos y estructura de carpetas

Al crear un proyecto, el motor genera la estructura bajo
`{app}/MotorGrafico/<proyecto>/`:

```
<proyecto>/
├── Memory/Binarios/Scene        ← escenas binarias
└── src<proyecto>/               ← assets del proyecto
    ├── modelos/                 ← .obj/.fbx que arrastra el editor
    ├── Sonidos/                 ← clips de audio (.wav/.mp3/...)
    ├── Interfaces/              ← assets JSON del CreadorDeInterfaces
    └── src<proyecto>/           ← scripts del usuario (.cpp/.java)
```

La convencion de assets por nombre usa carpetas `Sonidos/` e `Interfaces/` con
mayuscula inicial. El arbol de archivos del editor lista la **raiz del
proyecto** (`src<nombre>`).

> **Nota de packaging:** en el instalador Windows el explorador apunta a la
> raiz del proyecto; en builds de desarrollo antiguas (Linux) podia apuntar a
> la de scripts. Si un dropdown o drag & drop aparece vacio, verifica que raiz
> lista tu arbol.

---

## 3. Recorrido del editor

| Tecla / accion | Funcion |
|---|---|
| `W` `A` `S` `D` | Mover la camara activa (diagonales normalizadas) |
| `Espacio` / `Shift izq.` | Subir / bajar la camara |
| Mouse | Nav FPS (sensibilidad de Opciones) |
| `E` | Mostrar/ocultar interfaces del editor |
| `Escape` | Volver al menu de inicio |
| `1` / `T` | Gizmo: traslacion |
| `2` / `R` | Gizmo: rotacion |
| `3` / `Y` | Gizmo: escala |
| `G` | Gizmo local / mundo |
| Clic en objeto | Seleccionar en viewport |

El modo Play/Stop se controla desde la barra de menu de la escena; la fisica y
los scripts solo se ejecutan en Play. Clic en un objeto del arbol o del
viewport lo selecciona; el Inspector muestra sus componentes a la derecha.

---

## 4. Objetos y componentes

- **Crear objetos:** "New Object" (vacio/jerarquia) y "New RenderObject"
  (incluye `Model` + `Material` para renderizar).
- **Componentes:** `Transform`, `Color`, `Model`, `Material`, `Light`,
  `CameraComponent`, colliders (esfera / cubo / malla), `RigidBody`,
  `AudioSource`, `InterfaceComponent` y `Script`.
- **Inspector:** boton "Agregar componente" abre el popup de componentes; cada
  uno tiene su panel propio (Transform, Luz con tipo/atenuacion/color, etc.).
- **Jerarquia:** arrastra un objeto sobre otro en el arbol para reparentar; el
  motor rechaza ciclos y la raiz. "Childs Freeze" congela la transformacion de
  los hijos durante la edicion del padre.
- **Gizmos** (ImGuizmo): traslacion/rotacion/escala con `1`/`2`/`3` o
  `T`/`R`/`Y`, local/mundo con `G`; la fisica tiene su gizmo propio para el
  collider activo.

---

## 5. Assets por drag & drop

El explorador de archivos (arbol + grid) emite el payload ImGui
`ARCHIVO_PATH` (path completo del asset) al arrastrar. Receptores del editor:

| Destino del drop | Componente | Que guarda |
|---|---|---|
| Panel **Model** del Inspector | `Model` | path completo (buffer 4096) |
| Panel **Script** del Inspector | `Script` | path completo del fuente (`std::string`) |
| Dropdown **Sonido** de AudioSource | `AudioSource` | nombre del clip |
| Dropdown **Interfaz** de InterfaceComponent | `InterfaceComponent` | nombre del asset JSON |

Todos los receptores almacenan el valor en `std::string` o en buffers de al
menos 4096 bytes, de modo que paths largos no se truncan (el componente
`Model` era el unico con buffer de 100 y fue ampliado a 4096).

**Como usar:**

1. Copia tus assets a las carpetas del proyecto (o crealas con "New Folder" /
   "New File"); el FileSystemWatcher rescanea al detectar cambios externos.
2. Selecciona el objeto y arrastra el asset sobre el control correspondiente
   del panel del componente ("Arrastra modelo", campo Fuente del script,
   dropdown de Sonido, dropdown de Interfaz).
3. La escena guarda el path o nombre; al recargar se resuelve de nuevo.

---

## 6. Fisica

- Colliders de esfera, cubo o malla; con `RigidBody` participan de la
  simulacion Bullet. Solo simula en modo **Play**: en edicion el gizmo mueve
  el objeto y el motor sincroniza collider/cuerpo/objeto con la matriz global
  compuesta del dueño, para que mover un collider no desincronice el cuerpo.
- Mientras se manipula el gizmo, `stepSimulation` se pausa (la gravedad podria
  "eyectar" el objeto); al soltar, la simulacion sigue.
- Gizmo dedicado de fisica para el collider activo.

---

## 7. Audio

- Coloca los clips en `Sonidos/` (wav/mp3/etc.). `AudioClipsManager` los
  descubre y los registra **por nombre** en el `AudioEngine` al escanear.
- Agrega `AudioSource` a un objeto; en su panel elige el clip del dropdown
  (o arrastralo desde `Sonidos/`), ajusta volumen, loop y "reproduccion
  automatica". En Play, el AudioEngine reproduce en su hilo de audio.
- Cambiar de proyecto re-escanea y limpia el registro de clips.

---

## 8. Interfaces de juego (HUD)

- Crea el asset de interfaz con el **CreadorDeInterfaces** (genera un JSON en
  `Interfaces/<nombre>.json`).
- Agrega `InterfaceComponent` a un GameObject; su inspector muestra el nombre
  del asset (dropdown + drop desde `Interfaces/`).
- Al entrar en **Play**, la escena muestra esa interfaz a pantalla completa
  delante de la camara principal (HUD del juego).

---

## 9. Camaras

- `CameraComponent` con **vistas previas en vivo** (render a FBO); detalle
  completo en [CAMARAS_VISTAS_PREVIAS.md](CAMARAS_VISTAS_PREVIAS.md).
- "Usar" en el panel de la camara la marca como activa; el id se persiste en
  `Configuracion.json` (default automatica si el id ya no existe al cargar).

---

## 10. Guardar y abrir escenas

El guardado se hace desde la barra de menu de la escena. Las escenas son
binarias (`Memory/Binarios/Scene`), con serializacion en preorden y marcadores
`=>`/`<=` que recupera la jerarquia completa (padres e hijos) de forma
recursiva. Al iniciar, el editor recupera la escena del proyecto.

> Las escenas no tienen versionado aun (pendiente en el roadmap); al cambiar
> el formato binario de un componente, las escenas viejas pueden leer campos
> truncados con seguridad (los lectores acotan con `std::min`), pero el valor
> largo se pierde.

---

## 11. Apariencia y configuracion del editor

- Tema claro/oscuro, modo blanco y negro (acompaña fondo y grilla del
  viewport), color de acento y color de fondo de la escena, aplicados en vivo
  por `TemaEditor` / `AparienciaUtil`.
- Sensibilidad de camara, ventana de camaras y visibilidad de ventanas se
  guardan en `Configuracion.json` junto al binario
  (`<directorioEjecutable>/MotorGrafico/Configuracion.json`); tolerante a
  archivos ausentes o corruptos.
- Idioma del editor: Espanol / English desde Opciones.

---

## 12. Scripts: conceptos y ciclo de vida

Los comportamientos del juego se escriben como **scripts dinamicos**: archivos
`.cpp` o `.java` dentro del proyecto que el editor compila en caliente y
ejecuta en modo Play.

- Se crean desde el explorador: "New Script" (C++) o "New Script Java"
  (disponible si el motor se compilo con soporte JNI).
- **El nombre del archivo debe ser `<ClassName>.cpp`** (la clase == nombre del
  archivo). El backend compila la clase como `FUNSHI_<ClassName>` mediante
  `-DFUNSHI_NOMBRE_CLASE=<ClassName>`.
- Ciclo de vida en C++ (`IScriptBehaviour`): `onStart(owner)` al entrar en
  Play, `onUpdate(owner, deltaTime)` cada frame en Play, y `onStop(owner)`
  opcional al salir de Play.
- **SerializeField:** los campos declarados con macros `REFLECT_*` aparecen
  editables en el inspector, se guardan con la escena y sobreviven al hot
  reload (se reinyectan por nombre de campo).
- Hot reload por mtime del fuente: en Play, guardar el `.cpp`/`.java`
  recompila y recarga el comportamiento conservando los valores.
- La ventana **Estado** muestra el toolchain (compilador C++, javac, libjvm,
  cache) y el resultado de compilacion/carga de cada script de la escena.

---

## 13. Scripting C++: referencia completa

### 13.1 Plantilla (identica a la que genera el editor)

```cpp
#include "Behaviour/IScriptBehaviour.h"

// El archivo debe llamarse <ClassName>.cpp. La macro FUNSHI_NOMBRE_CLASE
// se define en compilacion con el nombre real de tu clase.
class FUNSHI_NOMBRE_CLASE : public IScriptBehaviour {
public:
    // Campos SerializeField editables en el inspector:
    float velocidad = 5.0f;

    REFLECT_INICIO(FUNSHI_NOMBRE_CLASE)
        REFLECT_CAMPO(velocidad)
    REFLECT_FIN

    void onStart(GameObject* owner) override {
        (void)owner;
    }

    void onUpdate(GameObject* owner, float deltaTime) override {
        (void)owner; (void)deltaTime;
    }

    void onStop(GameObject* owner) override {
        (void)owner;
    }

    std::vector<::ReflejoScripts::DefCampo>
    camposReflejados() const override { return reflexion(); }
};

// Export requerida por el backend del motor; NO renombrar. Va FUERA de la
// clase, al final del archivo.
extern "C" IScriptBehaviour* FUNSHI_CREAR_COMPORTAMIENTO(
    const MotorScript::ApiScriptGameObject* api) {
    (void)api;
    return new FUNSHI_NOMBRE_CLASE();
}
```

### 13.2 Campos SerializeField (macros REFLECT_*)

Declaralos entre `REFLECT_INICIO(<Clase>)` y `REFLECT_FIN` (pueden ir en zona
`public` o `private`; la macro abre `public`). Tipos soportados:

| Macro | Tipo del campo | Ejemplo |
|---|---|---|
| `REFLECT_CAMPO(nombre)` | `int`, `float`, `double`, `bool`, `std::string`, `vec3` | `int vidas = 3;` |
| `REFLECT_CAMPO(objetivo)` | `GameObject*` | referencia por nombre del objeto |
| `REFLECT_ARRAY(puntos)` | `std::vector<T>` de primitivas / `vec3` / `std::vector<GameObject*>` | `std::vector<float> ratios;` |
| `REFLECT_GRUPO(misil)` | struct anidado con su propio bloque `REFLECT_*` | `Misil misil;` |
| `REFLECT_GRUPOS(oleadas)` | `std::vector<S>` de structs reflejados | `std::vector<Oleada> oleadas;` |

Ejemplo con todos los casos:

```cpp
struct Oleada {
    int conteo = 1;
    float espaciado = 0.5f;

    REFLECT_INICIO(Oleada)
        REFLECT_CAMPO(conteo)
        REFLECT_CAMPO(espaciado)
    REFLECT_FIN
};

class FUNSHI_NOMBRE_CLASE : public IScriptBehaviour {
public:
    int vidas = 3;
    float velocidad = 5.0f;
    bool activo = true;
    std::string nombre = "jugador";
    vec3 direccion = { 0.0f, 1.0f, 0.0f };  // Float x3 en el inspector
    std::vector<float> puntos;
    std::vector<std::string> tags;
    Oleada primera;
    std::vector<Oleada> oleadas;
    GameObject* objetivo = nullptr;             // dropdown de objetos
    std::vector<GameObject*> enemigos;          // multi-seleccion

    REFLECT_INICIO(FUNSHI_NOMBRE_CLASE)
        REFLECT_CAMPO(vidas)
        REFLECT_CAMPO(velocidad)
        REFLECT_CAMPO(activo)
        REFLECT_CAMPO(nombre)
        REFLECT_CAMPO(direccion)
        REFLECT_ARRAY(puntos)
        REFLECT_ARRAY(tags)
        REFLECT_GRUPO(primera)
        REFLECT_GRUPOS(oleadas)
        REFLECT_CAMPO(objetivo)
        REFLECT_CAMPO(enemigos)
    REFLECT_FIN

    void onStart(GameObject* owner) override { (void)owner; }
    void onUpdate(GameObject* owner, float d) override { (void)owner; (void)d; }
    void onStop(GameObject* owner) override { (void)owner; }

    std::vector<::ReflejoScripts::DefCampo>
    camposReflejados() const override { return reflexion(); }
};
```

Notas:
- Las referencias `GameObject*` se serializan por el **nombre** del objeto (no
  por puntero), y sobreviven al hot reload.
- El editor muestra cada campo con su widget (InputFloat, Checkbox, InputText,
  ColorEdit para `vec3` cuando aplica, dropdown de objetos para referencias).
- Si el inspector muestra "Sin campos SerializeField", verificá que el bloque
  `REFLECT_*` este dentro de la clase y que `camposReflejados()` devuelva
  `reflexion()`.

### 13.3 Acceso al GameObject: tabla `api`

El motor inyecta en cada instancia la tabla `MotorScript::ApiScriptGameObject`
(via `conectarApi`, que el backend llama al crear el comportamiento). Los
scripts la usan como `this->api->...` y **deben comprobar `if (api)`** antes
de usarla. Firmas reales de la tabla:

| Funcion | Firma en la tabla | Descripcion |
|---|---|---|
| `api->nombre(owner)` | `const char* (const void* objeto)` | nombre del objeto |
| `api->posicionX(owner)` | `float (const void* objeto)` | posicion mundo, eje X |
| `api->posicionY(owner)` | `float (const void* objeto)` | posicion mundo, eje Y |
| `api->posicionZ(owner)` | `float (const void* objeto)` | posicion mundo, eje Z |
| `api->fijarPosicion(owner, x, y, z)` | `void (void*, float, float, float)` | fija la posicion |
| `api->fijarEscala(owner, x, y, z)` | `void (void*, float, float, float)` | fija la escala |
| `api->fijarRotacionEjes(owner, angulo, x, y, z)` | `void (void*, float, float, float, float)` | rota `angulo` radianes sobre el eje `(x,y,z)` |
| `api->imprimirConsola("texto")` | `void (const char* texto)` | escribe en la consola/log del editor |

Ejemplo de uso combinado:

```cpp
void onUpdate(GameObject* owner, float deltaTime) override {
    if (!api) return;
    float x = api->posicionX(owner);
    api->fijarPosicion(owner, x + velocidad * deltaTime,
                       api->posicionY(owner), api->posicionZ(owner));
    if (x > 10.0f) api->imprimirConsola("llego al limite");
}
```

### 13.4 Notas del backend C++

- El fuente se compila a `.so` (Linux), `.dll` (Windows/MSVC) o `.dylib`
  (macOS) con `-std=c++17 -shared -fPIC -O2` y
  `-DFUNSHI_NOMBRE_CLASE=<ClassName>`.
- El `.so` del script **no enlaza contra el motor**: todo el acceso pasa por
  la tabla `api` de punteros a funcion. No incluyas cabeceras del motor mas
  alla de `IScriptBehaviour.h`; en particular evita arrastrar Bullet/Assimp
  (opcional: usa `<cmath>`, `<string>`, `<vector>` y demas STL).
- La clase compilada se llama `FUNSHI_<ClassName>`: el mismo template
  (`FUNSHI_NOMBRE_CLASE`) compila para cualquier `<ClassName>.cpp`.
- El cache de artefactos compilados y la ruta del compilador se muestran en
  la ventana Estado.

---

## 14. Scripting Java (JNI)

Requiere que el motor se haya compilado con el JDK disponible
(`FUNSHI_JAVA=ON`); la ventana Estado muestra `javac`, `libjvm` y si el
soporte esta activo.

### 14.1 Plantilla generada por el editor

```java
// El nombre de la clase debe coincidir con el del archivo (MiScript.java).
public class MiScript implements Comportamiento {
    // Campos publicos = SerializeField editables en el inspector:
    public float velocidad = 5.0f;

    @Override
    public void iniciar(long objeto) {}

    @Override
    public void actualizar(long objeto, double deltaTime) {}

    @Override
    public void detener(long objeto) {}
}
```

### 14.2 Diferencias con C++

- El ciclo es `iniciar` / `actualizar` / `detener` y reciben el objeto como
  `long` (handle nativo), no como `GameObject*`.
- Los campos `public` del comportamiento son SerializeField automaticos; no se
  usan macros.
- Cada carga usa un classloader nuevo (child-first: delega al padre solo
  `Nativo`, `Comportamiento` y clases del JDK), lo que permite hot reload sin
  reiniciar la JVM.
- Utiles para prototipado rapido; para rendimiento, preferi C++.

---

## 15. Hot reload y depuracion

- **C++:** guardar el `.cpp` en Play recompila; el editor compara el mtime del
  fuente con el del artefacto cargado. Los valores SerializeField se extraen
  antes de descargar y se reinyectan por nombre de campo al terminar, de modo
  que reordenar campos en el fuente no pierde valores.
- **Java:** igual, con el classloader child-first; la JVM se reutiliza.
- **Ventana Estado:** para cada script muestra nombre, ok/error y mensaje
  (errores de compilacion incluidos), ademas del toolchain detectado.
- **Errores de compilacion C++** aparecen en el Estado y en consola; corregi
  el fuente y guardalo de nuevo (no hace falta salir de Play).
- Al cerrar la aplicacion los comportamientos se descargan sin disparar
  `onStop`; los backends (incluida la JVM) se apagan despues.

---

## 16. Problemas frecuentes

**El modelo arrastrado no aparece al recargar la escena.**
Versiones anteriores guardaban el path del componente `Model` en un buffer de
100 bytes: los paths largos quedaban truncados al serializar. Reasigna el
modelo arrastrandolo de nuevo con la version actual (buffer de 4096).

**"Sin campos SerializeField" en el inspector del script.**
El bloque `REFLECT_*` no esta dentro de la clase, o `camposReflejados()` no
devuelve `reflexion()`. Revisa la plantilla de la seccion 13.1.

**El dropdown de Sonido / Interfaz esta vacio.**
Los assets se descubren por carpeta: coloca los clips en `Sonidos/` y los
JSON en `Interfaces/` del proyecto. En builds antiguas verifica ademas que
raiz lista el explorador (seccion 2).

**El script no compila: clase no encontrada o export faltante.**
- El archivo debe llamarse `<ClassName>.cpp` y la clase generada es
  `FUNSHI_<ClassName>`; no renombres la export `FUNSHI_CREAR_COMPORTAMIENTO`.
- La export debe quedar fuera de la clase, al final del archivo.

**Fisica: el objeto "sale disparado" al moverlo con el gizmo.**
En Play la simulacion avanza mientras arrastras; usa el gizmo en modo edicion
o detene la simulacion antes de mover objetos con cuerpo dinamico.

**Paths con tildes/espacios fallan al cargar assets.**
El motor usa paths `char`/ANSI en Windows; evita caracteres fuera de ASCII en
las carpetas del proyecto hasta completar la portabilidad de rutas (pendiente
en el roadmap).