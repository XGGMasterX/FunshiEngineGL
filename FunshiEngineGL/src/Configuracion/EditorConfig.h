/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

#include <map>
#include <string>

#include "Apariencia.h"

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
        int version = 2;
        // Seccion "menu": MenuModel (vista Opciones).
        std::string nombreProyecto = "Nuevo Proyecto";
        std::string idioma = "Espanol";
        float sensibilidadCamara = 0.15f;
        // Sensibilidad de movimiento (WASD) de la camara del editor. Es global
        // (vista Opciones del menu), como sensibilidadCamara; se aplica a la
        // escena por SensibilidadMovimientoCambio.
        float sensibilidadMovimientoCamara = 1.0f;
        // Seccion "editor": estado de interfaz (GameScene).
        bool ventanaCamarasAbierta = true;
        int gizmoOperacion = 7;
        // sistema de coordenadas del gizmo: false = LOCAL, true = GLOBAL/WORLD.
        bool gizmoGlobal = false;
        // Id del GameObject elegido como camara activa ("Usar"), -1 = automatico
        // (GameScene usa la primera camara). Se persiste por id porque los
        // archivos de escena ya usan ese id estable.
        int camaraActivaId = -1;
        // stateGUI de cada ventana por su WindowName.
        std::map<std::string, bool> estadoVentanas;
        // Apariencia del editor (tema, modo B/N, acento de la UI y fondo 3D).
        // En la version 2 del archivo para permitir migracion tolerante.
        Apariencia apariencia;
    };

    // Ruta del archivo por plataforma, junto al binario del motor:
    //   Linux y Windows: <directorioEjecutable>/MotorGrafico/Configuracion.json
    // Directorio base de MotorGrafico donde viven todos los proyectos:
    //   Linux y Windows: <directorioEjecutable>/MotorGrafico
    static std::string directorioBaseMotorGrafico();

    // Directorio raiz de un proyecto especifico: <directorioBase>/<nombreProyecto>
    static std::string directorioProyecto(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Directorio Memory del proyecto (contiene lo que antes se guardaba en MotorGrafico):
    // <directorioProyecto>/Memory
    static std::string directorioMemory(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Directorio src del proyecto (raiz del explorador de archivos, hermano de Memory):
    // <directorioProyecto>/src<nombreProyecto>
    static std::string directorioSrc(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Nombre de la raiz del explorador de archivos: "src" + nombreProyecto
    static std::string nombreRaizSrc(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Ruta de la configuracion general en la raiz de MotorGrafico (hermana de los proyectos):
    // <directorioBase>/Configuracion.json
    static std::string rutaConfiguracionGeneral();

    // Ruta de la configuracion especifica del proyecto (dentro de Memory):
    // <directorioProyecto>/Memory/ConfiguracionProyecto.json
    static std::string rutaConfiguracionProyecto(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Ruta de compatibilidad: alias de rutaConfiguracionProyecto
    static std::string rutaConfiguracion(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Directorio de binarios de la escena: <directorioMemory>/Binarios
    static std::string directorioBinarios(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Prefijo para guardar la escena (saveScene): <directorioMemory>/Binarios/Scene
    static std::string rutaScenePrefijo(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Ruta del descriptor de escena: <directorioMemory>/Binarios/SceneBBDDObjetos.txt
    static std::string rutaSceneBBDD(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Directorio de archivos binarios individuales: <directorioMemory>/Binarios/Scene/
    static std::string rutaSceneDir(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Ruta del layout de ventanas de ImGui: <directorioMemory>/imgui.ini
    static std::string rutaImguiIni(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Crea en disco la estructura de carpetas requerida para el proyecto:
    //   <directorioBase>/<nombreProyecto>/Memory/Binarios/Scene
    //   <directorioBase>/<nombreProyecto>/src<nombreProyecto>
    // Y migra archivos previos si existian en la raiz de MotorGrafico.
    static void asegurarEstructuraProyecto(const std::string& nombreProyecto = "Nuevo Proyecto");

    // Ruta por defecto: apunta a Configuracion.json en la raiz de MotorGrafico
    static std::string rutaPorDefecto();

    // Directorio base por defecto (compatibilidad): directorio Memory de Nuevo Proyecto
    static std::string directorioProyectoPorDefecto();

    // Carga general y del proyecto activo si corresponde
    void cargar(const std::string& ruta);
    // Guarda general y del proyecto activo si corresponde
    void guardar(const std::string& ruta);

    // Guardado y carga independientes para no mezclar configuraciones generales y de proyecto:
    void guardarGeneral(const std::string& ruta = "");
    void cargarGeneral(const std::string& ruta = "");
    void guardarProyecto(const std::string& nombreProyecto, const std::string& ruta = "");
    void cargarProyecto(const std::string& nombreProyecto, const std::string& ruta = "");

    const Datos& datos() const noexcept { return datos_; }
    Datos& datos() noexcept { return datos_; }

    // Vuelve los datos a los defaults de fabrica (accion "Restablecer
    // configuracion" del menu). El llamador (main) conserva luego nombreProyecto
    // para no cambiar de proyecto/carpeta, y reapica escena/ventanas.
    void restablecer() noexcept { datos_ = Datos{}; }

private:
    Datos datos_;
};

#endif