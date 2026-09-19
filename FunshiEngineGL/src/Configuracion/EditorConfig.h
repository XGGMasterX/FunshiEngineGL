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
        float sensibilidadCamara = 1.0f;
        // Seccion "editor": estado de interfaz (GameScene).
        bool ventanaCamarasAbierta = true;
        int gizmoOperacion = 7;
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

    // Ruta del archivo por plataforma, junto al proyecto del usuario:
    //   Linux:   <HOME>/MotorGrafico/Configuracion.json
    //   Windows: C:/MotorGraficoArchivos/Configuracion.json
    // Directorio base de MotorGrafico donde viven todos los proyectos:
    //   Linux:   <HOME>/MotorGrafico
    //   Windows: C:/MotorGraficoArchivos
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

    // Ruta de Configuracion.json dentro de Memory del proyecto:
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

    // Ruta por defecto (compatibilidad): apunta a la configuracion en Memory de Nuevo Proyecto
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