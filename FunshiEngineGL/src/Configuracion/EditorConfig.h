#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

#include <map>
#include <string>

// Configuracion global del editor (interfaz + menu), persistida en JSON junto
// al proyecto del usuario. Los datos de escena siguen guardandose en binarios
// (SceneSerializer); el texto estructurado es solo para configuracion:
// legible, editable a mano, con "version" para migrar y tolerante a agregar o
// quitar campos sin romper la carga (un campo ausente conserva el default).
//
// Flujo (main.cpp):
//   1. Al arrancar: cargar() -> aplicar a MenuModel, GameScene y GUIManager.
//   2. Al salir: recoger los valores actuales en datos() y guardar().
class EditorConfig {
public:
    struct Datos {
        int version = 1;
        // Seccion "menu": MenuModel (vista Opciones).
        std::string nombreProyecto = "Nuevo Proyecto";
        std::string idioma = "Espanol";
        float sensibilidadCamara = 1.0f;
        // Seccion "editor": estado de interfaz (GameScene).
        bool ventanaCamarasAbierta = true;
        int gizmoOperacion = 7;
        // stateGUI de cada ventana por su WindowName.
        std::map<std::string, bool> estadoVentanas;
    };

    // Ruta del archivo por plataforma, junto al proyecto del usuario:
    //   Linux:   <HOME>/MotorGrafico/Configuracion.json
    //   Windows: C:/MotorGraficoArchivos/Configuracion.json
    static std::string rutaPorDefecto();

    // Directorio base del proyecto por plataforma (mismo patron que la escena).
    static std::string directorioProyectoPorDefecto();

    void cargar(const std::string& ruta);
    void guardar(const std::string& ruta);

    const Datos& datos() const noexcept { return datos_; }
    Datos& datos() noexcept { return datos_; }

private:
    Datos datos_;
};

#endif