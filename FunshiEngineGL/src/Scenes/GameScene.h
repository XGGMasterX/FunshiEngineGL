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


public:
    //------------Scene------------
    void saveScene(const std::string& filename) {
        if (!gameObjects->isEmpty()) {
            Position<GameObject*>* pos = gameObjects->first();
            while (pos != nullptr && pos->getElement() != nullptr) {
                pos->getElement()->saveEntity(filename);
                cout << "Guardado: " << pos->getElement()->getId() << endl;
                pos = (pos != gameObjects->last()) ? gameObjects->next(pos) : nullptr;
            }
        }

    }

    bool isStart() {
        return start;
    }

    void loadScene(const std::string& pathTxt, const std::string& semiPath) {
        gameObjects->clear(); // Evitar duplicados al cargar
        std::ifstream file(pathTxt);
        if (!file.is_open()) {
            std::cerr << "No se pudo abrir el archivo de paths: " << pathTxt << std::endl;
            return;
        }

        std::string rutaDb;
        while (std::getline(file, rutaDb)) {
            if (rutaDb.empty()) continue;

            //problema con el tipo dinamico y estatico
            GameObject* obj = new Modelos3D();

            // Extraer el id desde el nombre del archivo: ObjectN4.db => 4 y carga el archivo de lectura
            std::string nombreArchivo = rutaDb.substr(rutaDb.find_last_of("/\\") + 1);
            size_t posN = nombreArchivo.find("ObjectN");
            if (posN == std::string::npos) {
                std::cerr << "Formato de nombre incorrecto: " << nombreArchivo << std::endl;
                continue;
            }

            std::string idStr = nombreArchivo.substr(posN + 7); // 7 = length de "ObjectN"


            obj->setId(std::stoi(idStr));
            obj->loadEntity(semiPath);
            idStr = idStr.substr(0, idStr.find(".db"));          // eliminar ".db" si es necesario
            gameObjects->addLast(obj);
        }

        file.close();

        // Limpiar el archivo después de cargar
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
