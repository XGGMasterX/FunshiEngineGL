# Arquitectura: estados, GUI exterior (menú) e interiores del editor

Documento de análisis y propuesta de diseño para la rama `feature/estados-gui`.
Objetivo: **conectar las GUI internas del editor entre sí y con la GUI exterior
(el menú de inicio)**, de forma que un mismo mecanismo (máquina de estados)
gobierne ambas superficies. Se basa en lo observado en el código real, en las
buenas prácticas para proyectos grandes y en patrones de editores consolidados
(revisados en internet, ver §3).

Nada de lo propuesto está implementado todavía: es la hoja de ruta que se
ejecutará por fases, cada una con su propia revisión en `main`.

---

## 1. Estado actual (lo que el código ya resuelve)

- `ApplicationStateMachine` (`src/Estados/ApplicationStateMachine.{h,cpp}`) ya
  existe como fuente de verdad del alto nivel: `MainMenu`, `Editing`, `Playing`,
  `Exiting`. Su uso hoy es un simple `transitionTo` + consulta `is`, sin
  submáquina, sin eventos de transición.
- `main.cpp` es quien **decide qué se dibuja** a mano: si el estado es
  `MainMenu` dibuja la fachada del paquete `MenusGUI` (menú MVP, `MenuModel`/
  `MenuView`/`MenuGUI`), y si es `Editing` dibuja la fachada interna
  (`GUIManager` + `DockSpaceGUI`). Ese empalme vive en `main`, no en la máquina.
- La persistencias de ventanas internas ya está resuelta y centralizada:
  `EditorConfig` (`estadoVentanas`) con `obtenerEstadosVentanas` /
  `restaurarEstadosVentanas`, más apariencia, cámara activa, gizmo, idioma y
  sensibilidad. `GUIManager` es la fachada de esas ventanas (su paquete interno
  es `GUI/`).
- Ya se comunican "hacia dentro": el editor (opciones → escena/grilla/fondo;
  `editorConfig` → `GUIManager`), y "hacia fuera": el menú (opciones del menú →
  `EditorConfig.apariencia`/`idioma`/`sensibilidad`).
- Hay un `EventBus` (`src/Events/EventBus.{h,cpp}`) con `subscribe`/
  `publish`/`unsubscribe` que hoy se usa para eventos de escena.

## 2. Los dos huecos que hay que cerrar

1. **`main` es el pegamento**: la transición menú↔editor y qué ventanas se
   dibujan se toman en `main.cpp` (`ConsultarCierre`, `Escape`, `menuReflejado`).
   Ese conocimiento debería vivir en la máquina de estados (o en un controlador
   que la use), no en el bucle de aplicación.
2. **Las GUI internas no conversan entre sí de forma declarativa**: la ventana
   de Estado y el overlay de carga ya son independientes (señalado en sesión
   previa), pero el resto se pasa punteros / datos directos (p. ej. "cámara
   activa" se propaga de editor→previews, "apariencia" de menú→editor) sin un
   canal común. Al crecer, esto vuelve a acoplar paquetes.

## 3. Opciones investigadas (internet) y evaluación

### 3.1 Stock/falacia de "un estado por ventana"
El error típico es convertir cada ventana en un estado de la máquina (una
ventana abierta = un estado). Problemas: explosión de combinaciones, transiciones
no significativas, y persistencia difícil (¿cómo serializas "estado = {Inspector
abierto, Estado cerrado, ...}"?). La comunidad de editores ImGui (p. ej. el
segundo motor de referencia, *gkNextEditor*: `EditorInterface` orquesta paneles,
`EditorUiState` guarda qué está visible) muestra lo contrario: **la visibilidad
de ventanas NO son estados, son configuración de interfaz**.

### 3.2 Patrón que se repite en todos los editores serios (ImGui 1.91 y editores
de referencia)
- Un **estado global de UI** (ventana activa / fachada visible) + **estado de
  configuración por ventana** (abierta/cerrada, dock, tamaño). ImGui 1.91
  introduce `GetStateStorage()` por ventana/panel y perfila "panel state" para
  exactamente esto: estado por panel en `ImGuiContext`, ajeno a la lógica de
  negocio.
- Un **único punto de entrada de render**: el estado de la app determina qué
  fachada invoca `main` (menú vs editor), y dentro del editor cada panel es una
  función/objeto que se dibuja si `EditorUiState` lo dice.
- **Mediator/EventBus** para que los paquetes no se conozcan por puntero: el
  editor de apariencia no necesita saber quién es el manager de escenas.

### 3.3 Lo que YA encaja con el proyecto (no hay que re-implementar)
- `ApplicationStateMachine` = semilla del orquestador (estado de faé).
- `GUIManager` = fachada de GUI internas (ya tiene persistencia de ventanas).
- `MenusGUI` (MVP) = fachada exterior ya desacoplada por MVP.
- `EventBus` = canal de comunicación ya existente.
- `EditorConfig` = fuente única de persistencia.

## 4. Propuesta: una máquina de estados GUI + un canal de eventos

### 4.1 Modelo (qué NO cambia)
- `MainMenu` y `Editing` (y `Playing`/`Exiting`) siguen siendo los estados de
  aplicación. NO se crean estados por ventana.

### 4.2 Dos piezas nuevas (pequeñas, coherentes con lo existente)

**A) `OrquestadorEstadoGUI` se vuelve el único que toca la máquina y decide la
fachada GUI por frame.** Vive en el paquete `States` (puro: sin ImGui/GLFW, por
lo que es headless-testable igual que `EditorConfig`). Tiene la
responsabilidad de:
- escuchar eventos de menú (`manejarTeclaEscape` = "VolverAlMenú" desde el
  editor, `iniciarEstudio` = "Iniciar Estudio" desde el menú de inicio) vía la
  API de la fachada y el `ApplicationStateMachine`,
- reflejar en `ApplicationStateMachine.transitionTo(...)`,
- exponer `menuDebeEstarVisible()` / `escenaDebeCorrer()` que `main` consulta
  en el bucle para dibujar.

**B) Un `EditorEventBus` ínterno** (reusa el `EventBus` existente, o un
`EditorConfig`-detached bus tipado) con mensajes declarativos:
- `VentanaEstadoCambio(open)` — la ventana Estado actualiza su propia visibilidad
  (ya independiente) y la publica.
- `AparienciaCambio(perfil)` — menú ↔ editor comparten perfil sin punteros.
- `CamaraActivaCambio(id)` — inspector/previews se enteran.
- `IdiomaCambio(locale)` — todas las etiquetas se refrescan vía un `onLanguageChanged`.

Cada ventana **se subscribe a lo que necesita** y **publica lo que produce**;
ninguna se pasa un puntero a otra. Es el patrón Mediator/Observer que el
proyecto ya usa de forma puntual, pero ahora **declarado para todas las GUI**.

### 4.3 Por qué es la mejor opción para este proyecto
- **Mínimo cambio, máximo desacople**: no toca el motor, ni la escena, ni la
  serialización. Se apoya en fachadas y en el `EventBus` que ya existen.
- **Escalable**: añadir un estado nuevo (p. ej. `Settings` global) = un valor al
  enum + una rama en `EditorController`; añadir un panel = subscribe/publish.
- **Persistencia sin ambigüedad**: el estado de ventanas sigue siendo un mapa de
  configuración (`EditorConfig.estadoVentanas`), no estados de la FSM. Habilidad
  de restaurar sesión (ya implementada) intacta.
- **Testeable**: `EditorController` y el bus son puros (sin ImGui), se pueden
  probar headless igual que los tests de `EditorConfig`.
- Es lo que hacen editores grandes referenciados en la investigación (UI state
  global + per-window state + MessageBus), y coincide con la dirección que ya
  tomó ImGui 1.91 (stale storage por ventana).

## 5. Plan por fases (cada una sin romper nada)

1. **Fase 0 (rama actual, ya hecha)**: rama `feature/estados-gui` sobre
   `master`, bind del menú y configuración al día.
2. **Fase 1 — Orquestador de estado GUI (implementado en `feature/estados-gui`)**:
   - Clase pura **`OrquestadorEstadoGUI`** (paquete `src/States/`, headless, sin
     ImGui/GLFW) que centraliza las REGLAS que antes vivían sueltas:
     - `manejarTeclaEscape()` — transición `Editing -> MainMenu` (antes colgaba
       en el callback de teclado de `main`); guardia: inofensiva si ya estamos
       en el menú.
     - `iniciarEstudio()` — transición `MainMenu -> Editing` cuando el menú
       pidió cierre (`ConsultarCierre` de la fachada); con consumo único de la
       solicitud por frame (nueva guardia _double-consume_).
     - `menuDebeEstarVisible()`, `escenaDebeCorrer()` — reflejos por frame que
       `main` consulta en el bucle; sustituyen los `appStateMachine.is(...)`
       sueltos (la maquina ya no se filtra al paquete `MenuGUI`).
   - `main` queda como conversador de fachadas: pregunta a `MenuGUI` por el
     cierre, llama al orquestador (fuente de verdad de la decisión) y refleja su
     veredicto en `SetMenuActivo(...)` con la misma guardia de cambio que ya
     tenía (evita sacar al usuario de las sub-vistas cada frame).
   - Verificación: compilar, `ctest` (tests headless del orquestador junto a
     `configuracion-tests`), arrancar y probar menú→Iniciar Estudio→Escape→menú.
3. **Fase 2 — GUI internas conectadas por `EditorEventBus`**:
   - Definir los 4-5 mensajes clave del §4.2B.
   - Migrar vínculos directos conocidos (apariencia→grilla/fondo; cámara
     activa→previews; idioma→etiquetas; ventana Estado) al bus.
   - Verificación: cambiar apariencia/idioma/cámara en vivo, persistir, reiniciar.
4. **Fase 3 — Pulir opciones del menú**:
   - Revisar y completar las opciones presentes en `MenusGUI` (el menú ya expone
     apariencia, idioma, sensibilidad): unificar con lo que calladamente
     gestiona el editor para que toda opción tenga su efecto en vivo y su
     persistencia. Documentar cada opción (¿efecto inmediato? ¿persistencia?).
   - Añadir cualquier opción "global" nueva (p. ej. reset de configuración) como
     estado/acción con su test.
5. **Fase 4 — (futuro) estados superiores**: si hace falta `Settings`/`About`
   como pantallas, lo resuelve la misma máquina + un overlay, sin tocar las
   fachadas.

## 6. Riesgos y respuestas

- **Riesgo: tocar demasiado `main` desestabiliza el arranque.** → Fase 1 mantiene
  la misma secuencia de render frame a frame; solo agrupa decisiones.
- **Riesgo: muchos eventos = flujo ilegible.** → Mantener el catálogo de mensajes
  pequeño (los del §4.2B) y documentarlos; el bus es tipado, no un `std::any`
  libre.
- **Riesgo: perder persistencia de ventanas.** → Se conserva `EditorConfig`
  como única fuente de estado de ventanas; prohibido derivar visibilidad de la FSM.

## 7. Lecturas / referencias de la investigación
- Dear ImGui 1.91: *per-window / per-viewport state storage* (`GetStateStorage`),
  dirección oficial para estado de panel.
- Editores de referencia (patrón `EditorInterface` + `EditorUiState`,
  paneles como funciones mediante fachadas) — guían la separación "visibilidad =
  config, no estado".
- Patrones clásicos: State, Mediator, Observer/EventBus, Fachada (GOF) — todos ya
  presentes de forma parcial en el proyecto; la propuesta los completa sin
  introducir infraestructura ajena.
