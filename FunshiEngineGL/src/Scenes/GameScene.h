#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "../Ventana.h"

#if defined(_WIN32)
#include <conio.h>
#elif defined(__linux__)
#include <ncurses.h>
#endif
#include <iostream>
#include <fstream>
#include "..//Time.h"
#include "../Gizmo/Camera.h"
#include "../Objetos/GameObject.h"
#include "../Objetos/Materiales/materiales.h"
#include "../Estructuras/ListasEnlazadas/ListasConPrioridad/PriorityListaDE.h"
#include "../Objetos/Modelos3D.h"
#include "../Objetos/Malla.h"
#include "../Objetos/Componentes/Color.h"
#include "../GUIManager/GUIManager.h"
#include "../Fisicas/PhysicsEngine.h"

//ACTIVAR GUARDADO Y DIBUJO DE OBJETOS
using namespace std;

class GameScene {
private:
    ArbolEnlazado<GameObject*>* entitys;
    ListaDE<GameObject*>* gameObjects;
    Camera* camera;
    GUIManager* managerGUI;
    SceneSelectedInterface* selecteableGUI;
    PhysicsEngine* phisics;
    SceneMenuBarInterface* menuBarGUI;
    float deltaTime;
    bool start = false;
public:
    GameScene(Camera* camera,GUIManager* managerGUI) {
        this->camera = camera;
        this->managerGUI = managerGUI;
        this->selecteableGUI = managerGUI->getSelecteableGUI();
        this->menuBarGUI = managerGUI->getMenuBarGUI(&start);
        entitys = selecteableGUI->getEntitysTree();
        gameObjects = selecteableGUI->getGameObjects();
        phisics = new PhysicsEngine();
    }

    ~GameScene() {
        gameObjects->clear();
    }

    //------LISTA IMGUI------
    ListaDE<GameObject*>* getGameObjectsScene() {
        return gameObjects;
    }

private:

    void saveInPreOrden(Position<GameObject*>* root,const std::string& filename){
     if(root != nullptr){
       root->getElement()->saveEntity(filename);
       if(entitys->isInternal(root)){
          ofstream archivo(filename+"BBDDObjetos.txt",std::ios::app);
          archivo << "=>" << endl;
          ListaDE<Position<GameObject*>*>* hijosDeRoot = entitys->childsOf(root);
          Position<Position<GameObject*>*>* position = hijosDeRoot->first();
           while(position != nullptr){
            saveInPreOrden(position->getElement(),filename);
            position = (position != hijosDeRoot->last()) ? hijosDeRoot->next(position) : nullptr;
           }
          archivo << "<=" << endl;
         }
       }
    }

void loadInPreOrdenRec(std::ifstream& file, Position<GameObject*>* parent,
                       const std::string& semiPath)
{
    std::string line;
    while (std::getline(file, line)) {
        if (line == "<=") {
            //RETROCEDO
            return;
        }
        else if (line == "=>") {
            //EL ULTIMO HIJO TIENE HIJOS
            if (parent != nullptr) {
                Position<GameObject*>* lastChild = entitys->childsOf(parent)->last()->getElement();
                loadInPreOrdenRec(file, lastChild, semiPath);
            } else {
                //SI PARENT ES NULL ES PQ EL HIJO ES LA RAIZ
                Position<GameObject*>* rootPos = entitys->rootOfTree();
                loadInPreOrdenRec(file, rootPos, semiPath);
            }
        }
        else if (!line.empty()) {
            //EN CUALQUIER OTRO CASO LEEO EL BINARIO
            GameObject* obj = new Modelos3D();

            // Extraer ID del nombre de archivo
            std::string nombreArchivo = line.substr(line.find_last_of("/\\") + 1);
            size_t posN = nombreArchivo.find("ObjectN");
            if (posN != std::string::npos) {
                std::string idStr = nombreArchivo.substr(posN + 7);
                idStr = idStr.substr(0, idStr.find(".db"));
                obj->setId(std::stoi(idStr));
            }

            obj->loadEntity(semiPath);
            //AGREGO EL ROOT AL ARBOL
            if (parent == nullptr) {
                entitys->deleteRoot();
                entitys->createRoot(obj);
            } else {
                //AGREGO LOS QUE NO SON ROOT A LISTA Y ARBOL
                gameObjects->addLast(obj);
                entitys->addNodeChildOf(parent, obj);
                obj->setOriginTransform(parent->getElement()->getComponent<Transform>());
            }
        }
    }
}


public:
    //GUARDADO EN PRE ORDEN DE ENTIDADES
    void saveScene(const std::string& filename) {
        if (!entitys->isEmpty()) {
            Position<GameObject*>* root = entitys->rootOfTree();
            saveInPreOrden(root,filename);
        }
   }

    bool isStart() {
        return start;
    }

    //CARGADO EN PRE ORDEN DE ENTIDADES
    void loadScene(const std::string& pathTxt, const std::string& semiPath) {
    gameObjects->clear();
    std::ifstream file(pathTxt);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo de paths: " << pathTxt << std::endl;
        return;
    }

    loadInPreOrdenRec(file, nullptr, semiPath);

    file.close();

    // Limpiar archivo después de cargar (opcional)
    std::ofstream cleanFile(pathTxt, std::ios::trunc);
    if (!cleanFile.is_open()) {
        std::cerr << "No se pudo limpiar el archivo de paths." << std::endl;
    }
    }

    //recorro la lista
    //revisar de que punta a que punta
    void dibujarGameObjects() {
        if (!gameObjects->isEmpty()) {
            Position<GameObject*>* pos = gameObjects->first();
            while (pos != nullptr && pos->getElement() != nullptr) {
                dibujarObject(pos->getElement());
                pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
            }
        }
    }

    void dibujarObject(GameObject* obj) {
        obj->setTam(10);
        obj->setColor(obj->auxColor);
        if (obj->getComponent<Transform>() != nullptr) {
            obj->dibujar(deltaTime);
        }
    }

    void mallaScene(float tam) {
        float L = tam;
        float incr = 1.0;
        float y = -0.5;
        glColor3fv(branco_gelo);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, branco_gelo);
        glBegin(GL_LINES);
        for (float i = -L; i <= L; i += incr)
        {
            // Verticais
            glVertex3f(i, y, -L);
            glVertex3f(i, y, L);

            // Horizontais
            glVertex3f(-L, y, i);
            glVertex3f(L, y, i);
        }
        glEnd();
        glEndList();
    }

    void GUI() {
        //le paso los ENTITY => refactorizar todo eso
        selecteableGUI->printGUI();
        
        //Obtengo el objeto que fue seleccionado en la selecteableGUI si se apreto un boton
        if (gameObjects->isElement(selecteableGUI->getReturnableEntity()) && this->phisics != nullptr) {
            managerGUI->setPhysics(this->phisics);
            managerGUI->getSettingGUI(selecteableGUI->getReturnableEntity())->printGUI();
        }
        menuBarGUI->printGUI();
	    
    }

    void update(float deltaTime) {
        //float aux = this->deltaTime;
        this->deltaTime = deltaTime;// +aux;
        phisics->stepSimulation(deltaTime);
        //cout << this->deltaTime << endl;
    }

    void gameScene() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity(); 
        camera->activate();

        //Autiomatizar Creacion De Objetos
        glPushMatrix();
        mallaScene(70.0);
        glPopMatrix();

        //TESTEAR
        GLfloat lightPos[] = { 5.0f, 5.0f, 5.0f, 1.0f }; // Posición mundial
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

        dibujarGameObjects();
        GUI();
    }

};
#endif
