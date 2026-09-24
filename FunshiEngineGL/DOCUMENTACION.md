# FunshiEngineGL - Documentacion

Motor de juegos 3D con OpenGL e ImGui. Este archivo resume los controles,
la navegacion del editor y la configuracion relacionados con las funciones
agregadas mas recientes (camara, gizmo, callbacks, logs).

## 1. Como lanzar (sin terminal atras de la ventana)

El ejecutable NO necesita una terminal: al arrancar redirige toda la salida de
consola (stdout/stderr, cout/cerr, printf) a un archivo de logs, asi no se
escupe texto a ninguna consola.

- **Doble clic** sobre el binario (o abrirlo con tu gestor de archivos): sin
  terminal.
- **`./ejecutar.sh`**: compila si hace falta y lanza el editor desacoplado
  (en segundo plano) escribiendo al log.
- **Escritorio Linux** (`.desktop` en `~/.local/share/applications/`):

  ```
  [Desktop Entry]
  Name=FunshiEngineGL
  Exec=/ruta/a/FunshiEngineGL/build/FunshiEngineGL
  Type=Application
  ```

Solo el lanzamiento **manual desde una terminal** (`./FunshiEngineGL`)
mantiene esa terminal como padre del proceso; incluso ahi la salida ya va al
log y la terminal no muestra basura.

## 2. Logs

- Carpeta: `logs/` **junto al ejecutable** (se crea sola).
- Archivo: `FunshiEngineGL_AAAAMMDD_HHMMSS.log` (uno por arranque, con
  timestamp UTC), en modo append si ya existia.
- Contiene el diagnostico de GPU, los mensajes `[diag]` del render y cualquier
  error (`[MeshRenderer]`, `[IconosGUI]`, etc.).

En Windows el binario se construye como GUI (`WIN32_EXECUTABLE`): no abre
consola; la salida tambien va al log.

## 3. Controles del editor

| Tecla | Accion |
|-------|--------|
| `E` | Alterna interfaces del editor ON/OFF. En navegacion libre (OFF) el cursor se fija al centro y se oculta |
| `Clic derecho` sostenido sobre la escena | Navegar/mirar desde el editor con interfaces visibles (cursor capturado mientras se sostiene; soltar lo restaura) |
| `W` `A` `S` `D` | Adelante / izquierda / atras / derecha (diagonales permitidas y normalizadas) |
| `Espacio` / `Shift izq.` | Arriba / abajo |
| `G` | Alterna el gizmo entre **Local** (ejes del objeto) y **Global** (ejes del mundo, no rota con el objeto) |
| `1` o `T` | Gizmo: mover (Translate) |
| `2` o `R` | Gizmo: rotar (Rotate) |
| `3` o `Y` | Gizmo: escalar (Scale) |
| `Escape` | Volver al menu principal desde el editor |

El modo del gizmo (Local/Global) tambien se elige en el menu **"Gizmo"** de la
barra superior del editor, y se persiste por proyecto.

## 4. Configuracion (persistida en JSON)

- **Sensibilidad del mouse-look (camara)**: 0.15 por defecto. Se ajusta en
  **Opciones** del menu de inicio (configuracion general).
- **Sensibilidad de movimiento (WASD)**: 1.0 por defecto. Se ajusta en
  **Opciones** del menu y en la ventana **"Camaras"** de la escena.
- **Gizmo**: operacion (`gizmoOperacion`) y sistema de coordenadas
  (`gizmoGlobal`) se guardan por proyecto (seccion `editor` de
  ConfiguracionProyecto.json).

## 5. Movimiento por maquina de estado (Input/EditorInput)

- Las callbacks GLFW (teclado/mouse) viven en `src/Input/EditorInput.{h,cpp}`,
  fuera de `main.cpp`.
- `onKey` SOLO registra el estado de las teclas (PRESS/RELEASE). Cada frame el
  bucle llama a `EditorInput::aplicarMovimiento(dt)`, que combina las
  direcciones activas en un vector `[derecha, arriba, adelante]` y lo
  normaliza: las diagonales (`W+A`, `W+D`, ...) se mueven a la misma velocidad
  que un solo eje.
- El movimiento solo aplica en estado **Editing** y cuando ImGui no captura el
  teclado (escribir en un campo de texto no desplaza la camara).
- El pitch de la camara esta limitado a +-89 grados (`kPitchMaxGrados` en
  `CameraComponent.cpp`) para que no se "dé vuelta".

## 6. GameObject "Scene" (raiz de la escena)

Cada escena tiene ahora un objeto raiz llamado **"Scene"** (tipo
`ObjetoEscena`, derivado de `SimpleObject` sin geometria). Este objeto
es el padre estructural de **todas las entidades** de la escena:
objetos 3D, luces, camaras, etc. Se crea automaticamente al abrir o
crear una escena (`SceneRegistry::createDefaultRoot`) y se serializa
como cualquier GameObject (id 0, sin malla). La jerarquia visible en el
explorador/inspector refleja esta estructura: al crear objetos sin
padre, cuelgan del "Scene". Es compatible con escenas anteriores:
al cargar un binario raiz antiguo (guardado con malla), los bytes
sobrantes quedan sin leer (EOF) y no rompen el formato.

## 7. Sistema de audio (AudioEngine + AudioSource)

- **AudioEngine** (`src/Audio/`): motor de audio concurrente con hilo
  dedicado (`bucleAudio`). Recibe comandos por cola protegida con
  mutex (`reproducir`, `detener`, `detenerTodo`). Usa **MiniAudio**
  como backend; si no hay dispositivo de audio al iniciar, el motor
  queda en modo mudo (todas las llamadas devuelven -1) sin romper nada.
- **AudioSource** (Component): adjunto a un GameObject. Guarda el
  nombre del clip (string, coincide con un archivo en `Sonidos/` del
  proyecto), volumen (0-1), bucle y reproduccion automatica. Al entrar
  en modo **Play** (`GameScene::sincronizarAudioPlay`), la escena
  inyecta el `AudioEngine` en cada `AudioSource` y dispara los de
  reproduccion automatica. Al salir de play, detiene todos.
- **Inspector**: panel `SettingsAudioSource` permite editar clip,
  volumen, bucle, auto-play y probar reproduccion/detencion en editor.
- **Persistencia**: se serializa length-prefixed (string + floats + bools)
  junto al GameObject; compatible con escenas viejas (campos nuevos
  leen 0/false al final).

## 8. Creador de Interfaces (CreadorDeInterfaces)

Ventana del editor (accesible desde menu **Ventanas > Creador UI**)
para crear assets de UI en `Memory/Interfaces/<nombre>.json` del
proyecto actual. Funciones:

- **Lista de interfaces**: muestra todas las del proyecto; click para
  cargar en el area de edicion.
- **Edicion de widgets**: 5 tipos — Etiqueta, Boton, Checkbox, Slider,
  EntradaTexto. Cada widget puede tener un **Sonido** (dropdown con
  clips de `Sonidos/`).
- **Guardar**: escribe el JSON en disco.
- **Activar en canvas**: la interfaz del borrador se vuelve la "activa"
  y se refleja en el `CanvasInterface` (ventana dockable para probar).
- **Uso en juego**: ver seccion 9 (InterfaceComponent).

Formato JSON: nombre, titulo, ancho/alto (px), array de widgets
(tipo, etiqueta, valores, sonido). El `CanvasInterface` pinta en vivo
el estado de checkbox/slider/texto.

## 9. InterfaceComponent (UI del jugador / HUD)

Componente `InterfaceComponent` que se adjunta a CUALQUIER GameObject.
Tiene un campo: **Nombre de interfaz** (asset en `Memory/Interfaces/`).
Al entrar en **modo Play** (`start == true`), `GameScene::GUI` busca el
primer objeto con `InterfaceComponent` cuyo nombre no este vacio,
llama a `CreadorDeInterfaces::activarInterfaz(nombre)` (carga el JSON
si no esta activo) y pasa esa `UserInterfaceCustom` al
`CanvasInterface`. Como el canvas en modo play es un **overlay a
pantalla completa** (`setModoPlay(true)`), la interfaz se dibuja
**directamente frente a la camara principal** (HUD del juego).

El estado editable (checkbox, slider, texto) se conserva entre frames
mientras la misma interfaz siga activa (la instancia `interfazActiva_`
del creador persiste).

Para agregar: click derecho en inspector > **Agregar Interfaz** > edita
el nombre del asset.

## 10. Exportar juego

Desde el editor: menu **Archivo > Exportar juego** (barra superior).
Copia recursivamente la carpeta completa del proyecto actual
(`MotorGrafico/<proyecto>`) a
`MotorGrafico/Exportaciones/<proyecto>`. El resultado es una carpeta
autocontenida con:
- `Memory/` (escenas, interfaces, imgui.ini, ConfiguracionProyecto.json)
- `Sonidos/` (clips de audio)
- `src<proyecto>/` (mallas, texturas, scripts fuente)
- `ConfiguracionProyecto.json`

Para distribuir: entrega el ejecutable del motor junto a la carpeta
`Exportaciones/<proyecto>`. El usuario (o un launcher) arranca:
```
FunshiEngineGL --proyecto <nombre>
```
El flag `--proyecto` salta el menu de inicio y abre directo el
proyecto en modo editor (con paneles visibles). En un launcher final
se cambiaria a modo play automatico.

Feedback: el resultado (exito/ruta o error) aparece 4 segundos en la
**Barra de Estado** (ventana "Estado") y en la consola/log.