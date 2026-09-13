# IDEA: Menú de inicio del motor (patrón MVC) — trabajo en limpio

> Documento de rescate. El código original (`MenuView` + `MenuController`) fue
> eliminado el 2026-09-13 para "partir en limpio", conservando solo la idea.
> Nunca llegó a commitarse (estaba untracked); último commit previo: `283f3c4`.

## 1. Idea general

Menú de inicio a pantalla completa (estilo launcher) separado en dos capas:

- **MenuController** (`src/States/`): lógica **pura**, sin ImGui/GLFW ni
  rendering. Dueño del estado: vista actual + datos de configuración
  (nombre de proyecto, idioma). Independiente de la interfaz y testeable.
- **MenuView** (`src/GUI/MenusGUI/`): capa de presentación. **Solo lee** el
  estado del controller y, ante clics del usuario, **delega la acción de
  vuelta** al controller (patrón MVP). Así la interfaz se puede reemplazar o
  recorrer sin tocar la lógica.

## 2. Contrato de MenuController (lógica pura)

```cpp
class MenuController {
public:
    enum class Vista { Principal, Opciones, ConfigProyecto, Ninguna };

    void mostrarMenu();        // abre el menu (p. ej. desde el editor con Escape)
    void iniciarEstudio();     // cierra el menu y deja el motor listo para editar
    void abrirOpciones();
    void abrirConfigProyecto();
    void volver();             // de Opciones/ConfigProyecto -> Principal

    bool estaVisible() const noexcept { return vista != Vista::Ninguna; }
    Vista getVista() const noexcept { return vista; }

    const std::string& getNombreProyecto() const noexcept;
    void setNombreProyecto(const std::string&);
    const std::string& getIdioma() const noexcept;
    void setIdioma(const std::string&);
    const std::vector<std::string>& getIdiomas() const noexcept;

private:
    Vista vista = Vista::Principal;
    std::string nombreProyecto = "Nuevo Proyecto";
    std::string idioma = "Espanol";
    std::vector<std::string> idiomasDisponibles = {"Espanol", "English"};
};
// Todas las transiciones son setters triviales del enum `vista`.
```

## 3. Vistas del menú

| Vista | Contenido |
|---|---|
| `Principal` | Botones centrados: "Iniciar Estudio", "Config Proyect" (etiqueta con el nombre actual), "Opciones" (etiqueta con el idioma actual), "Exit". |
| `Opciones` | Título "Opciones", selector de idioma (`ImGui::BeginCombo` sobre `getIdiomas()`), "Volver". |
| `ConfigProyecto` | Título "Config Proyect", `InputText` del nombre del proyecto, "Volver". |
| `Ninguna` | Menú oculto → se renderiza el editor normal (equivale a "Iniciar Estudio"). |

## 4. Detalles clave de la vista (ImGui)

- Hereda de `GeneralUserInterface` (mismo contrato `initGUI/contentGUI/endGUI/printGUI` que la `MenuInterface` actual). Constructor: `MenuView(MenuController*, GLFWwindow*)`, nombre "Menu", `state=true`.
- Ventana a pantalla completa sin decoración: `SetNextWindowPos(0,0)` + tamaño `GetIO().DisplaySize`; flags `NoTitleBar|NoResize|NoMove|NoCollapse|NoDocking`; `PushStyleVar(WindowRounding,0)` y `PushStyleColor(WindowBg, (0.1,0.1,0.12,1))` alrededor de `Begin`/`End`.
- `printGUI()` retorna sin dibujar si `!controller->estaVisible()`.
- Botones centrados con helper (`kBotAncho=200`, `kBotAlto=50`):
  ```cpp
  void centrar(ImVec2 d) { // d = desvio vertical desde el centro
      ImVec2 c = ImGui::GetWindowSize();
      c.x *= 0.5f; c.y *= 0.5f;
      ImGui::SetCursorPos({c.x - kBotAncho*0.5f + d.x, c.y + d.y});
  }
  // Botones apilados con desvios: -170, -110, -50, +10
  ```
- Etiquetas (Nombre Proyecto / Idioma) a la derecha del botón con `SameLine()` + `SetCursorScreenPos` alineando verticalmente: `y + (kBotAlto - GetTextLineHeight())*0.5f`.
- Buffer de edición `char nombreProyectoBuffer[128]`: al **entrar** a `ConfigProyecto` se resetea y, la primera vez que está vacío, se rellena con `strncpy` desde `getNombreProyecto()` (evita arrastrar texto de una sesión anterior). Cada cambio del `InputText` llama `setNombreProyecto()`.
- "Exit" usa `glfwSetWindowShouldClose(ventana, GLFW_TRUE)`; la ventana GLFW se recibe por constructor (única dependencia de la vista con GLFW).
- Include condicional de GLFW igual que el resto del proyecto: `<glfw3.h>` en Windows, `<GLFW/glfw3.h>` en Linux.

## 5. Integración prevista

- Abrir el menú desde el editor (p. ej. tecla Escape) vía `mostrarMenu()`.
- "Iniciar Estudio" → `iniciarEstudio()` → `Ninguna` → arranca el editor.
- Pensado para convivir con, y eventualmente reemplazar, la `MenuInterface` actual (`MenuOpcionesInterface` + `MenuProyectoInterface`), que mezcla lógica y UI en la misma clase.
- CMake usa `GLOB_RECURSE`, por lo que no hay que tocar CMakeLists para añadir/quitar estos fuentes.

## 6. Estado al momento del borrado

- 4 archivos sin seguimiento: `MenuView.{h,cpp}` (197 líneas), `MenuController.{h,cpp}` (69 líneas).
- No estaba cableado a `ApplicationStateMachine` ni llamado desde ningún otro punto del código (verificado con grep).
