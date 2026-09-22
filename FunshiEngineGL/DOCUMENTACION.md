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