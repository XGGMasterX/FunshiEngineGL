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
#include "../src/Scenes/GameScene.h"
#include "../src/GUIManager/GUIManager.h"
#include "../src/Configuracion/EditorConfig.h"
#include "../src/GUI/Tema/TemaEditor.h"
#include <imgui.h>
#include "EngineTime.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "../src/Objetos/Modelos3D.h"
#include "../src/Objetos/Componentes/CameraComponent.h"
#include "../src/States/ApplicationStateMachine.h"
#include "../src/States/OrquestadorEstadoGUI.h"
#include "../src/Behaviour/ScriptRuntime.h"
#include "../src/Events/EditorEventBus.h"
#include "../src/Ventana.h"
#include "ImGuizmo.h"
#include "GLCompat.h"
#include <iostream>
#include <string>

//VENTANA
static int ventanaHeightEjeY, ventanaWidthEjeX;

//INPUT
static float lastMousePosX = 0.0;
static float lastMousePosY = 0.0;
static bool firstTimeMouseX = true;
static bool firstTimeMouseY = true;
static bool recFilesInit = true;

// Variables de estado
// El estado abierto/cerrado del menu de inicio lo gobierna el modelo del
// paquete MenuGUI (MenuModel), sincronizado por frame desde la maquina de
// estados (fuente de verdad); no un bool suelto de main.
// Fuente de verdad del estado de la aplicacion (MainMenu / Editing). El menu
// de inicio (paquete MenuGUI) informa su decision y aqui se refleja.
static ApplicationStateMachine appStateMachine;
static OrquestadorEstadoGUI orquestadorDeGUI(&appStateMachine);
static float deltaTime = 0.0f;
// Ruta del imgui.ini (layout de docks/geometria de ventanas): el puntero que
// guarda ImGui debe vivir toda la app, por eso es una global.
static std::string g_imguiIniRuta;
class MiAPP {
private:
    GameScene* scene;

public:
    static MiAPP* instancia;
    

    MiAPP(GameScene* scene) {
        this->scene = scene;
        instancia = this;
    }

    static void teclado_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (instancia) instancia->onKey(window, key, scancode, action, mods);
    }

    static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
        if (instancia) instancia->onMouse(window, xpos, ypos);
    }


    void onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
    {

        CameraComponent* camara = scene ? scene->getActiveCamera() : nullptr;
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            // Volver al menu de inicio desde el editor: la maquina de estados
            // es la fuente de verdad; el bucle principal refleja su decision
            // en la fachada del paquete MenuGUI (MainMenu = menu visible).
            if (ImGui::GetIO().WantCaptureKeyboard)
            {
                // Un InputText de ImGui esta activo: Escape revierte el texto
                // en edicion y no corta la edicion de datos del editor.
            }
            else if (appStateMachine.is(ApplicationState::Editing))
            {
                // Escape en el editor: la regla vive en el orquestador de
                // estados de GUI, no suelta en el callback de main.
                orquestadorDeGUI.manejarTeclaEscape();
            }
        }
        else if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            if (camara) camara->forward(deltaTime);
            if (key == GLFW_KEY_W && key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
            {
                
                if (camara) camara->forwardRight(deltaTime);
            }

            else if (key == GLFW_KEY_W && key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
            {
                
                if (camara) camara->forwardLeft(deltaTime);

            }
        }
        else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            if (camara) camara->back(deltaTime);
            if (key == GLFW_KEY_S && key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
            {
                
                if (camara) camara->backRight(deltaTime);

            }

            else if (key == GLFW_KEY_S && key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
            {
                
                if (camara) camara->backLeft(deltaTime);
            }
        }
        else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            if (camara) camara->left(deltaTime);

        }
        else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            if (camara) camara->right(deltaTime);
        }
        if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
           
            if (camara) camara->up(deltaTime);
        }
        else if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            if (camara) camara->down(deltaTime);
        }

        if (key == GLFW_KEY_E && action == GLFW_PRESS) {
            if (scene) scene->toggleEditorInterfaces();
        }

        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_1 || key == GLFW_KEY_T) {
                if (scene) scene->setGizmoOperation(ImGuizmo::TRANSLATE);
            }
            else if (key == GLFW_KEY_2 || key == GLFW_KEY_R) {
                if (scene) scene->setGizmoOperation(ImGuizmo::ROTATE);
            }
            else if (key == GLFW_KEY_3 || key == GLFW_KEY_Y) {
                if (scene) scene->setGizmoOperation(ImGuizmo::SCALE);
            }
        }
    }

    void onMouse(GLFWwindow* window, double xpos, double ypos)
    {
        float dx;
        float dy;
        if (firstTimeMouseX)
        {
            dx = 0;
            dy = 0;
            lastMousePosX = xpos;
            firstTimeMouseX = false;
        }if (firstTimeMouseY)
        {
            dx = 0;
            dy = 0;
            lastMousePosY = ypos;
            firstTimeMouseY = false;
        }

        dx = xpos - lastMousePosX;
        dy = ypos - lastMousePosY;

        lastMousePosX = xpos;
        lastMousePosY = ypos;
        
        ImGuiIO& io = ImGui::GetIO();
        bool gizmoCapturing = scene && scene->isGizmoCapturingInput();
        bool editorActivo = scene && scene->isEditorActivo();
        if (!editorActivo && !io.WantCaptureMouse && !gizmoCapturing) {
            if (CameraComponent* camara = scene ? scene->getActiveCamera() : nullptr) {
                // Sensibilidad global configurada en Opciones (menuGUI).
                const float sensibilidad =
                    scene ? scene->getSensibilidadCamara() : 1.0f;
                camara->updateYaw(dx * sensibilidad, dy * sensibilidad);
            }
        }

    }

    float aleatorio(float a, float b)
    {
        float n = (float)rand() / RAND_MAX;
        float t = b - a;
        float r = a + n * t;
        return r;
    }
};
MiAPP* MiAPP::instancia = nullptr;
int main(void)
{

    Ventana* auxVentana = new Ventana();
    auxVentana->initVentana();
    GLFWwindow* window = auxVentana->getWindow();
    GUIManager* managerOfGUI = new GUIManager(window);
    GameScene* scene = new GameScene(managerOfGUI);
    MenuGUI* mainMenu = managerOfGUI->getMenuGUI();
    TreeFilesInterface* treeFilesInterface = managerOfGUI->getTreeFilesGUI();
    ContentFolderInterface* contentFolderInterface = managerOfGUI->getContentFolderGUI();
    MiAPP* app = new MiAPP(scene);
    Time::start();
    // Configuration del editor (interfaz + menu) persistida en JSON en Memory
    // del proyecto del usuario. Al arrancar se carga y se aplica a cada capa; al
    // salir se recogen los valores actuales y se guarda (ver fin de main).
    // La escena es independiente: sigue en sus binarios (SceneSerializer).
    EditorConfig editorConfig;
    editorConfig.cargar(EditorConfig::rutaPorDefecto());
    std::string proyectoActual = editorConfig.datos().nombreProyecto;
    if (proyectoActual.empty()) proyectoActual = "Nuevo Proyecto";

    EditorConfig::asegurarEstructuraProyecto(proyectoActual);
    managerOfGUI->configurarProyecto(proyectoActual);

    mainMenu->setNombreProyecto(editorConfig.datos().nombreProyecto);
    mainMenu->setIdioma(editorConfig.datos().idioma);
    mainMenu->setSensibilidadCamara(editorConfig.datos().sensibilidadCamara);
    mainMenu->setApariencia(editorConfig.datos().apariencia);
    scene->setVentanaCamarasAbierta(editorConfig.datos().ventanaCamarasAbierta);
    scene->setGizmoOperation(editorConfig.datos().gizmoOperacion);
    scene->setApariencia(editorConfig.datos().apariencia);
    managerOfGUI->restaurarEstadosVentanas(editorConfig.datos().estadoVentanas);

    // Ultimo estado de la maquina reflejado en la fachada del paquete MenuGUI
    // (guardia de cambio; ver el bucle principal).
    bool menuReflejadoEnFachada = appStateMachine.is(ApplicationState::MainMenu);

    
    glfwSetKeyCallback(window, MiAPP::teclado_callback);
    glfwSetCursorPosCallback(window, MiAPP::mouse_callback);




    // Fondo del viewport 3D segun el perfil de apariencia; el bucle principal
    // lo refresca por frame para reflejar cambios en vivo desde Opciones.
    {
        float fondoInicial[3];
        AparienciaUtil::fondoEfectivo(mainMenu->getApariencia(), fondoInicial);
        glClearColor(fondoInicial[0], fondoInicial[1], fondoInicial[2], 1.0f);
    }


    // Configuración de iluminación fija
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_NORMALIZE);  // Normaliza automáticamente las normales

    // El estado de luz (GL_LIGHTING y GL_LIGHT0..7) lo gestiona LightSystem
    // cada frame; no se enciende nada a mano acá.

    // Configuración de materiales
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    float FPS = 60.0;    //LIMITE DE FPS
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // El imgui.ini (layout de docks y geometria de ventanas) se guarda en Memory
    // del proyecto del usuario, no en el directorio actual de lanzamiento.
    g_imguiIniRuta = EditorConfig::rutaImguiIni(proyectoActual);
    io.IniFilename = g_imguiIniRuta.c_str();
    // Tema global de ImGui a partir del perfil de apariencia cargado. Los
    // cambios en vivo llegan por el bus de GUI (EditorEventBus/AparienciaCambio),
    // ya no por relectura por frame del menu.
    TemaEditor::aplicarEstilo(mainMenu->getApariencia());
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 440"); //460 PARA PC , 440 PARA NOTEBOOK

    // Suscriptores del canal de GUI interna (Fase 2): main reacciona a los
    // cambios que publican el menu (apariencia/idioma) y la ventana Estado;
    // la escena y las ventanas no se pasan punteros entre si. El bus lo posee
    // GUIManager y ya esta cableado a los publicadores.
    EditorEventBus* eventosGUI = managerOfGUI->getEditorEventBus();
    if (eventosGUI) {
        // Apariencia: aplica el estilo ImGui, el fondo del viewport y la
        // apariencia de la escena (grilla/vistas previas), y persiste en la
        // config GENERAL al instante (no depende del proyecto activo).
        eventosGUI->subscribe([scene, &editorConfig, &proyectoActual](const EditorEvent& ev) {
            if (ev.type != EditorEventType::AparienciaCambio) return;
            scene->setApariencia(ev.apariencia);
            TemaEditor::aplicarEstilo(ev.apariencia);
            float fondo[3];
            AparienciaUtil::fondoEfectivo(ev.apariencia, fondo);
            glClearColor(fondo[0], fondo[1], fondo[2], 1.0f);
            auto& cfg = editorConfig.datos();
            cfg.apariencia = ev.apariencia;
            editorConfig.guardarGeneral();
        });
        // Idioma: se persiste en la config general al instante.
        eventosGUI->subscribe([&editorConfig](const EditorEvent& ev) {
            if (ev.type != EditorEventType::IdiomaCambio) return;
            editorConfig.datos().idioma = ev.idioma;
            editorConfig.guardarGeneral();
        });
        // Ventana Estado: al cerrarla con la 'X' se persiste en la config
        // del proyecto activo, no en la general.
        eventosGUI->subscribe([&editorConfig, &proyectoActual](const EditorEvent& ev) {
            if (ev.type != EditorEventType::VentanaEstadoCambio) return;
            if (!ev.nombreVentana) return;
            editorConfig.datos().estadoVentanas[ev.nombreVentana] = ev.abierta;
            editorConfig.guardarProyecto(proyectoActual);
        });
    }

    const std::string sceneBBDD = EditorConfig::rutaSceneBBDD(proyectoActual);
    const std::string sceneDir = EditorConfig::rutaSceneDir(proyectoActual);
    scene->loadScene(sceneBBDD, sceneDir);

     // Restaura la camara activa elegida con "Usar" (persistida por id). Si el
     // id ya no existe, GameScene se queda en modo automatico.
     scene->setActiveCameraById(editorConfig.datos().camaraActivaId);


    while (!glfwWindowShouldClose(window)) //BUCLE PRINCIPAL
    {

        Time::limitFPS(FPS);
        deltaTime = Time::getDeltaTime();
        
        
        glfwPollEvents();

        // La apariencia se aplica por eventos (EditorEventBus/AparienciaCambio)
        // cuando el usuario la cambia en Opciones; ya no se relee el menu y se
        // reaplica estilo/fondo cada frame.

        if (scene->isStart()) { //MODIFICAR , si se activa comenzar normal , si se quita volver todo al comienzo.
            //loadNewComponents(); // Buscar y cargar componentes nuevas
            scene->update(deltaTime);
        }

        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            //Obtenemos El tam del Frame De La ventana en X y Y
            glfwGetFramebufferSize(window, &ventanaWidthEjeX, &ventanaHeightEjeY);     

            auxVentana->redimension(ventanaWidthEjeX, ventanaHeightEjeY);                
            // Interfaz de inicio (paquete MenuGUI): main solo conversa con la
            // fachada. ApplicationStateMachine es la fuente de verdad del
            // estado (MainMenu = menu visible, Editing = editor) y los dos
            // flujos que la cambian son: Escape (callback, Editing -> MainMenu)
            // y el boton "Iniciar Estudio" (ConsultarCierre, MainMenu ->
            // Editing). Por frame: se consulta el cierre (consumo unico), se
            // transiciona y se refleja la decision de la maquina en la fachada.
            if (mainMenu->ConsultarCierre()) {  // "Iniciar Estudio"
                // main solo conversa con la fachada (pregunta si el menu pidio
                // cerrarse); la DECISION de transicion MainMenu -> Editing vive
                // en el orquestador, que ademas guarda el resultado por frame.
                orquestadorDeGUI.iniciarEstudio();
            }

            // Sincronizacion con guardia de cambio: SetMenuActivo(true) reinicia
            // la vista del menu a Principal, asi que solo se aplica cuando la
            // maquina efectivamente cambio de estado (evita sacar al usuario de
            // las sub-vistas Opciones/ConfigProyecto en cada frame). La decision
            // la da el orquestador de GUI (unica fuente de verdad), no un
            // appStateMachine.is() suelto aqui.
            // Reflejo por frame: main pregunta al ORQUESTADOR (no a la maquina
            // a pelo) que GUI este frame es la correcta. Esta es la unica
            // fuente de verdad para la fachada; la maquina interna no se filtra
            // al paquete MenuGUI.
            const bool menuDebeEstarAbierto =
                orquestadorDeGUI.menuDebeEstarVisible();
            if (menuDebeEstarAbierto != menuReflejadoEnFachada) {
                mainMenu->SetMenuActivo(menuDebeEstarAbierto);
                menuReflejadoEnFachada = menuDebeEstarAbierto;
            }

            // La sensibilidad del mouse look se configura en Opciones (menu) y
            // se propaga a la escena; el menu esta pausado mientras es visible,
            // asi que la lectura por frame no tiene costo apreciable.
            scene->setSensibilidadCamara(mainMenu->getSensibilidadCamara());

            // Sincroniza cambio de nombre de proyecto si se edito en Config Proyect
            const std::string nombreMenu = mainMenu->getNombreProyecto();
            if (!nombreMenu.empty() && nombreMenu != proyectoActual) {
                proyectoActual = nombreMenu;
                EditorConfig::asegurarEstructuraProyecto(proyectoActual);
                managerOfGUI->configurarProyecto(proyectoActual);
                editorConfig.datos().nombreProyecto = proyectoActual;
            }

            if (mainMenu->ConsultarMenu()) {
                mainMenu->Renderizar();             // la vista dibuja la vista activa del modelo
            }

            // La escena corre en cualquier estado que no sea el menu (Editing,
            // y futuramente Playing); el menu visible la detiene y dibuja solo.
            const bool sceneRunning =
                orquestadorDeGUI.escenaDebeCorrer();

            if (sceneRunning) {
                if (scene->isEditorActivo())
                    managerOfGUI->getDockSpaceGUI()->printGUI();
                scene->gameScene();
            }
            // El estado abierto/cerrado del menu de inicio lo gobierna el
            // modelo del paquete MenuGUI (MenuModel), no un bool suelto de main.

            if (scene->isEditorActivo()) {
                treeFilesInterface->printGUI();
                // R3: el panel de contenido se gobierna solo (lee la seleccion
                // compartida del FileManager) y ya no depende de que main le
                // sincronice la carpeta con setContentFolderGUI().
                contentFolderInterface->printGUI();
            }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);    
    }

    scene->saveScene(EditorConfig::rutaScenePrefijo(proyectoActual));

    // Apagado ordenado de los scripts antes de salir: primero se liberan las
    // referencias globales JNI de las instancias y luego se apaga el JVM
    // (DestroyJavaVM). Sin esto LeakSanitizer reporta cientos de bloques
    // internos del JVM (nmethods, oopmaps, metaspace) como fugas al cerrar.
    scene->descargarScripts();
    ScriptRuntime::apagarScripts();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Guardado de la configuracion al salir (mismo punto donde se salva la
    // escena). Se recogen los valores actuales ya que pudieron cambiar en la
    // sesion (menu, ventanas, gizmo).
    EditorConfig::Datos& cfg = editorConfig.datos();
    cfg.nombreProyecto = mainMenu->getNombreProyecto();
    cfg.idioma = mainMenu->getIdioma();
    cfg.sensibilidadCamara = mainMenu->getSensibilidadCamara();
    cfg.ventanaCamarasAbierta = scene->getVentanaCamarasAbierta();
    cfg.gizmoOperacion = scene->getGizmoOperation();
    cfg.camaraActivaId = scene->getActiveCameraId();
    cfg.estadoVentanas = managerOfGUI->obtenerEstadosVentanas();
    cfg.apariencia = mainMenu->getApariencia();
    // Configuracion general: apariencia, idioma, sensibilidad y ultimo proyecto.
    editorConfig.guardarGeneral();
    // Configuracion del proyecto: estado de ventanas, gizmo, camara activa.
    editorConfig.guardarProyecto(cfg.nombreProyecto);

    glfwTerminate();
    return 0;
}
