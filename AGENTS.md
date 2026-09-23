# AGENTS.md — Convenciones para agentes de código

Guía rápida para agentes (y humanos) que trabajen en este repositorio.

## Comandos esenciales

```bash
# Configurar y compilar (el build soportado es CMake; el .vcxproj es legacy)
cmake -S FunshiEngineGL -B FunshiEngineGL/build
cmake --build FunshiEngineGL/build -j$(nproc)

# Ejecutar la suite de tests (12 targets headless)
cd FunshiEngineGL/build && ctest --output-on-failure
```

## Convenciones del código

- **Idioma**: C++17. Identificadores, comentarios y logs en español
  (`fijarPosicion`, `AudioEngine::reproducir`). Nombres de tipo en PascalCase,
  métodos en camelCase.
- **Copyright**: todo archivo fuente nuevo del motor debe llevar el header
  Apache 2.0 (ver cabecera de cualquier `.h`/`.cpp` existente como modelo) o
  al menos `SPDX-License-Identifier: Apache-2.0`.
- **Estilo**: 4 espacios, llaves en la línea siguiente para clases/funciones.
- **Documentación**: los cambios de API visibles para usuarios se reflejan en
  `MANUAL_DE_USO.md` (especialmente la sección 13, scripting). Cambios de
  arquitectura en `PROJECT_STRUCTURE.md`.

## Reglas de arquitectura

- **Análisis previo obligatorio**: antes de crear o modificar código, analizar
  las excepciones, estructuras de datos, patrones de diseño y arquitecturas ya
  presentes en el proyecto y reutilizarlas; si el cambio requiere algo nuevo
  o distinto, recomendarlo y justificarlo antes de implementarlo.
- **Tablas de API para scripts (`ApiScriptGameObject`, `ScriptServices`)**:
  estrictamente APPEND-ONLY. Los campos nuevos van SIEMPRE al final, antes del
  campo `int version`; nunca reordenar ni borrar entradas. El campo `version`
  se incrementa al agregar funcionalidad.
- **Modularidad**: los módulos se comunican por tablas de punteros a función
  (patrón de `ScriptGameObject.cpp`, implementado en una TU del ejecutable) o
  por inyección explícita (`GameScene::inyectarServiciosScript`). No acoplar
  el módulo de scripts con Audio/Scenes/Input por includes directos.
- **Backends de scripts**: C++ (`BackendCpp`) y Java JNI (`BackendJava`);
  cualquier servicio nuevo debe documentarse si queda C++-only.
- **Serialización binaria**: sin versionado aún; al ampliar un componente,
  mantener compatibilidad de prefijos y acotar lecturas con `std::min`.
- **Física** detrás de la fachada `PhysicsEngine` → `IPhysicsBackend`; no usar
  Bullet fuera de `Fisicas/`.

## Flujo de trabajo

- **Una tarea, una rama**: al comenzar una tarea de un tema distinto al actual,
  primero commitear los cambios pendientes de la rama actual y luego cambiar de
  rama. Nunca mezclar temas distintos en una misma rama.
- **Ramas remotas primero**: si la tarea requiere una rama nueva, verificar
  antes si en el remoto ya existe una que cumpla ese rol; en ese caso traerla
  al local (`git fetch` + `git checkout -b <rama> origin/<rama>` o
  `git switch`), actualizarla con su base y trabajar sobre ella. Solo crearla
  localmente si no existe.
- **Base correcta de la rama**: al crear una rama, verificar contra la
  documentación del flujo (ver [FLUJO_DE_RAMAS.md](FLUJO_DE_RAMAS.md)) desde
  cuál rama debe partir — `develop`, `release`, `staging` o `test` — para
  mantener el orden de desarrollo; solo usar `master` (o la rama que
  corresponda según ese documento) como base cuando el flujo lo indique.
- **Commits atómicos por tarea**: cada tarea terminada cierra con su commit
  (o los que sean necesarios si la tarea es grande), con mensaje descriptivo
  y convencionales (`feat:`, `fix:`, `docs:`, `refactor:`, `chore:`).
- **Sin atribuciones ajenas al cambio**: los mensajes de commit (y cualquier
  metadato asociado) deben describir únicamente la implementación o los
  cambios realizados. No se permite adjudicar coautoría, autoría, firmas ni
  menciones a herramientas, asistentes o terceros que no correspondan al
  trabajo concreto sobre el código.
- Validar antes de commitear: build completo + `ctest` en verde.
- El CI (`.github/workflows/ci.yml`) compila el engine en Ubuntu y corre la
  suite en Linux/Windows/macOS; no pushear sin pasar los tests localmente.
