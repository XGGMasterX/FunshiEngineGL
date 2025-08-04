#include "../src/Scenes/GameScene.h"
#include "../src/GUIManager/GUIManager.h"
#include <imgui.h>
#include "Time.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include "../src/Objetos/Modelos3D.h"
#include <GLFW/glfw3.h>
#include <iostream>
/////////////////////////////////////////////////////////////////////////////////
/*
                           \\\\COSAS PARA AGREGAR////
*sistema de seguimiento de scripts y asociaciones junto a git
*Mejorar Controles
* arreglar la iluminacion y texturados y de dibujado por priorizacion distancial
* agregar refactor para compatible con linux
* agregar interface imgui reactiva por clicking
* agregar reaccion por clicking
*Mejorar Elejir tipo coposicion que tiene la figura
*Agregar funcion para deletear todos los gameObject
*Interfaz y botones
*Agregar metodo update
*Agregar metodo Start
*Agregar Classe Input
*Crear Colliders
*Agregar contenedores
*Corregir Archivos Header
*Mejorar settings de objetos desde imgui
*Crear el menu
*Agregar en settings zona de scripts , crear un typedef [SerializeField] y etc
*Colocar limite en Y para camara que evite inversion en el input del eje Y
*Agregar otros archivos al gestor de archivos y agregar su edicion
*/
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
/*
                           \\\\COSAS PARA ARREGLAR////
*No se pintan bien los objetos
*/
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
/*
                           \\\\COSAS PARA DEBUGGEAR////
*Observar version de OPENGL
*/
/////////////////////////////////////////////////////////////////////////////////
// 
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
//CAMARA
Camera* camera = new Camera(vec3(1, 1, -50));
//ESCENA
GameScene* scene = new GameScene(camera);
//INPUT
static float lastMousePosX = 0.0;
static float lastMousePosY = 0.0;
static bool firstTimeMouseX = true;
static bool firstTimeMouseY = true;
bool recFilesInit = true;
//imgui
bool my_tool_active;
int inputImGuiID;
char inputImGuiString[128] = "";
bool menuActivo = false;


// Variables de estado
bool showMenu = true;
bool sceneRunning = false;
float aleatorio(float a, float b)
{
    float n = (float)rand() / RAND_MAX;
    float t = b - a;
    float r = a + n * t;
    return r;
}


void interfaceCreateNewObject() {
    ImGui::Begin("CreateObject", &my_tool_active, ImGuiWindowFlags_MenuBar);

    ImGui::Text("Objects");
    if (ImGui::TreeNode("===CreateObject==="))
    {
        ImGui::Text("Modelo3D");
        if (ImGui::Button("CreateModelo3D"))
        {
            Modelos3D* newModelos1 = new Modelos3D("C:/MotorGraficoArchivos/Binarios/BlenderModelos/cubo.obj");//AUTOMATIZAR
            Modelos3D* newModelos2 = new Modelos3D("C:/MotorGraficoArchivos/Binarios/BlenderModelos/monkey.obj");//AUTOMATIZAR
            Modelos3D* newModelos3 = new Modelos3D("C:/MotorGraficoArchivos/Binarios/BlenderModelos/head_fixed.obj");//AUTOMATIZAR
            cout << "Objeto Creado" << endl;
            ImGui::InputText("Path", inputImGuiString, IM_ARRAYSIZE(inputImGuiString), ImGuiInputTextFlags_EnterReturnsTrue);
            scene->createGameObject(newModelos1);
            scene->createGameObject(newModelos2);
            scene->createGameObject(newModelos3);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("===DeleteObject==="))
    {
        ImGui::InputInt("Id", &inputImGuiID);
        if (ImGui::Button("Delete"))
        {
            scene->deleteObjectByID(inputImGuiID);
        }
        ImGui::TreePop();

    }

    ImGui::End();
}



void teclado_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        sceneRunning = false;
    }
    else if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        // Forward
        camera->forward();
        if (key == GLFW_KEY_W && key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            //DERECHA ADELANTE
            camera->forwardRight();
        }

        else if (key == GLFW_KEY_W && key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            //IZQUIERDA ADELANTE
            camera->forwardLeft();

        }
    }
    else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        // Back
        camera->back();
        if (key == GLFW_KEY_S && key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            //DERECHA ATRAS
            camera->backRight();

        }

        else if (key == GLFW_KEY_S && key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            //IZQUIERDA ATRAS
            camera->backLeft();
        }
    }
    else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        // Left
        camera->left();

    }
    else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        // Right
        camera->right();
    }
    if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        // Up
        camera->up();
    }
    else if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        // Down
        camera->down();
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        menuActivo = !menuActivo;
    }


}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

int main(void)
{
    Ventana* auxVentana = new Ventana();
    auxVentana->initVentana();
    GLFWwindow* window = auxVentana->getWindow();
    //MENU
    GUIManager* managerOfGUI = new GUIManager(window);
    MenuInterface* mainMenu = managerOfGUI->getMenuInterface();
    Time::start();

    // -- inicio
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, teclado_callback);
    glClearColor(0.0, 0.15, 0.25, 1.0); //color de fondo


    // Activar iluminación
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

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
        float deltaTime = Time::getDeltaTime();
        
        /* Poll for and process events */
        glfwPollEvents();
       

        //usar delta para updates de scene y gui

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        if (true /*ImGui::Button */) {
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


            if (menuActivo) { //menuActivo = !menuActivo si E es presionada



                //MOSTRAMOS EL GESTOR DE ARCHIVOS
                //MOSTRAMOS INSPECTOR DE SCENE
                //MOSTRAMOS INSPECTOR DE FOLDERS
                //TODAS LAS GUI DE LA SCENE
                interfaceCreateNewObject(); //MOSTRAMOS LA VENTANA DE INSTANCIACION DE OBJETOS
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
