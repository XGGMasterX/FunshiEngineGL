#include "../src/Scenes/GameScene.h"
#include "../src/GUIManager/GUIManager.h"
#include <imgui.h>
#include "Time.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "../src/Objetos/Modelos3D.h"
#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif
#include <iostream>
//testing
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
/////////////////////////////////////////////////////////////////////////////////
/*
                           \\\\COSAS PARA AGREGAR///

*revisar error en el root del treefile
*arreglar lo del id que no deje meter si ya existen (que no haga nada)

*Agregar animaciones
*Agregar Materiales
*Agregar texturas

//AL ELIMINAR UN OBJETO SE QUEDA EL BINARIO , ARMAR LISTA DE OBJETOS ELIMINADOS Y QUITAR Y LIMPIAR
//Cuadro de log donde se indique lo que se esta haciendo mal
//ERROR NO DEJA LEER ARCHIVOS RAROS , DEBO QUITAR FILTROS
//ES ERROR QUE MANTEGA EL RADIO AL ELIMINAR COLLIDERS ? : SI
//TERMINAR TODOS LOS POPUP
//HACER METODO PARA LOS POPUP EN CADA GUI
//UN SETTING SE ABRE AL ARRASTRAR EL OTRO
//seria ideal crear un metodo que recorra todos los objetos
//obtenga sus collider y verifique si se chocan con el mio
//si es true entonces devolver quien es el objeto
//SE DIBUJAN MAL LOS COLORES DEL COLLIDER
//AL CREA APARECE UNA BARRA / CONTRARIA QUE ES \

*sistema de seguimiento de scripts y asociaciones junto a git
*arreglar la iluminacion y independizar mediantee clases y entidades , gizmos
*Crear sistema de Entitys e incluir sitema de Arboles de dependencia , soy DependencyNode
*Agregar imagenes en botones del contentFolder y para gizmos
*Que no se mueva la camara si estoy en una GUI , ???quitar lo de E y escape , poner boton para salir al menu???
*agregar interface imgui reactiva por clicking
*agregar reaccion por clicking
*Agregar funcion para deletear todos los gameObject y para los gamesObjects de selecteable

*AGREGAR EL SISTEMA DE PROGRAMACION DINAMICA
*Agregar metodo update
*Agregar metodo Start
*Agregar en settings zona de scripts
*crear un typedef [SerializeField] es una extencion define para atributos privados mutables

*Agregar contenedores
*Corregir Archivos Header
*Agregar Classe Input
*/
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
/*
                           \\\\COSAS PARA HACER////
*Comentar Codigo
*Documentar Proyecto
*Exception en carga de files del master
*/
/////////////////////////////////////////////////////////////////////////////////



//VENTANA
int ventanaHeightEjeY, ventanaWidthEjeX;

//INPUT
static float lastMousePosX = 0.0;
static float lastMousePosY = 0.0;
static bool firstTimeMouseX = true;
static bool firstTimeMouseY = true;
bool recFilesInit = true;
bool menuActivo = false;

// Variables de estado
bool showMenu = true;
bool sceneRunning = false;
float deltaTime = 0;


class MiAPP {
private:
    Camera* camera;
    GameScene* scene;


public:
    static MiAPP* instancia;
    

    MiAPP(Camera* camera, GameScene* scene) {
        this->camera = camera;
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

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            sceneRunning = false;
        }
        else if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            camera->forward(deltaTime);
            if (key == GLFW_KEY_W && key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
            {
                
                camera->forwardRight(deltaTime);
            }

            else if (key == GLFW_KEY_W && key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
            {
                
                camera->forwardLeft(deltaTime);

            }
        }
        else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            camera->back(deltaTime);
            if (key == GLFW_KEY_S && key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
            {
                
                camera->backRight(deltaTime);

            }

            else if (key == GLFW_KEY_S && key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
            {
                
                camera->backLeft(deltaTime);
            }
        }
        else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            camera->left(deltaTime);

        }
        else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            camera->right(deltaTime);
        }
        if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
           
            camera->up(deltaTime);
        }
        else if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            
            camera->down(deltaTime);
        }

        if (key == GLFW_KEY_E && action == GLFW_PRESS) {
            menuActivo = !menuActivo;
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
        if (!menuActivo) {
            camera->updateYaw(dx, dy);
            camera->update();
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
    Camera* camera = new Camera(vec3(1, 1, -50));
    GameScene* scene = new GameScene(camera, managerOfGUI);
    MenuInterface* mainMenu = managerOfGUI->getMenuGUI();
    TreeFilesInterface* treeFilesInterface = managerOfGUI->getTreeFilesGUI();
    ContentFolderInterface* contentFolderInterface = managerOfGUI->getContentFolderGUI();
    MiAPP* app = new MiAPP(camera, scene);
    Time::start();

    
    glfwSetKeyCallback(window, MiAPP::teclado_callback);
    glfwSetCursorPosCallback(window, MiAPP::mouse_callback);




    glClearColor(0.1, 0.1, 0.1, 1.0); //color de fondo


    // Configuración de iluminación fija
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_NORMALIZE);  // Normaliza automáticamente las normales

    // Configuración de iluminación
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
 



    // Configuración de materiales
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    float FPS = 60.0;    //LIMITE DE FPS

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 440"); //460 PARA PC , 440 PARA NOTEBOOK

 #if defined(_WIN32)
     scene->loadScene("C:/MotorGraficoArchivos/Binarios/SceneBBDDObjetos.txt", "C:/MotorGraficoArchivos/Binarios/Scene/");
 #elif defined(__linux__)
//scene->loadScene("/home/ggmaster/MotorGraficoArchivos/Binarios/SceneBBDDObjetos.txt", 
//                "/home/ggmaster/MotorGraficoArchivos/Binarios/Scene/");
 #endif


    while (!glfwWindowShouldClose(window)) //BUCLE PRINCIPAL
    {

        Time::limitFPS(FPS);
        deltaTime = Time::getDeltaTime();
        
        
        glfwPollEvents();
        
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
            if (showMenu) {
                mainMenu->printGUI();
                mainMenu->getOpcionesGUI()->printGUI();
                mainMenu->getConfigProyectGUI()->printGUI();
                showMenu = mainMenu->getMenusState();
                sceneRunning = !showMenu;
            }

            if (sceneRunning) {
                scene->gameScene();
            }
            showMenu = !sceneRunning;
            mainMenu->setStateGui(showMenu);

            if (menuActivo) { //menuActivo = !menuActivo si E es presionada (CAMBIAR)
                treeFilesInterface->printGUI();
                if (treeFilesInterface->getFolderContent() != nullptr){
                    managerOfGUI->setContentFolderGUI();
                    contentFolderInterface->printGUI();
                }
            }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);    
    }

#if defined(_WIN32)
    scene->saveScene("C:/MotorGraficoArchivos/Binarios/Scene");
#elif defined(__linux__)
// Guardar escena
//scene->saveScene("/home/ggmaster/MotorGraficoArchivos/Binarios/Scene");
#endif

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    glfwTerminate();
    return 0;
}
