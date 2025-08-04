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

//ACTIVAR GUARDADO Y DIBUJO DE OBJETOS
using namespace std;

class GameScene : Time {
private:
    // PRE FABRICADOS PARA TESTING
    ListaDE<GameObject*>* nodosScene;
    PriorityListaDE<GameObject*>* gameObjectsPorDistancia;
    Camera* camera;
public:
    GameScene(Camera* camera) {
        this->camera = camera;
        nodosScene = new ListaDE<GameObject*>();
        gameObjectsPorDistancia = new PriorityListaDE<GameObject*>(camera);
    }

    ~GameScene() {
        gameObjectsPorDistancia->clear();
    }



    //------LISTA IMGUI------
    ListaDE<GameObject*>* getNodosScene() {
        return nodosScene;
    }

    void setNodosScene(ListaDE<GameObject*>* nodosScene) {
        this->nodosScene = nodosScene;
    }


public:
    //------------Scene------------
    void saveScene(const std::string& filename) {
        if (!gameObjectsPorDistancia->isEmpty()) {
            Position<GameObject*>* pos = gameObjectsPorDistancia->first();
            while (pos != nullptr && pos->getElement() != nullptr) {
                pos->getElement()->saveObject(filename);
                cout << "Guardado: " << pos->getElement()->getId() << endl;
                pos = (pos != gameObjectsPorDistancia->last()) ? gameObjectsPorDistancia->next(pos) : nullptr;
            }
        }

    }

    bool deleteObjectByID(int id) {
        bool encontre = false;
        if (!gameObjectsPorDistancia->isEmpty()) {
            Position<GameObject*>* pos = gameObjectsPorDistancia->first();
            while (pos != nullptr && !encontre) {
                if (pos->getElement()->getId() == id) {
                    gameObjectsPorDistancia->remove(pos);
                    cout << "Eliminado: " << pos->getElement()->getId() << endl;
                    encontre = true;
                }
                else {
                    pos = (pos != gameObjectsPorDistancia->last()) ? gameObjectsPorDistancia->next(pos) : nullptr;
                }
            }
        }
        return encontre;
    }

    void loadScene(const std::string& pathTxt, const std::string& semiPath) {
        gameObjectsPorDistancia->clear(); // Evitar duplicados al cargar
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
            obj->loadObject(semiPath);
            idStr = idStr.substr(0, idStr.find(".db"));          // eliminar ".db" si es necesario
            gameObjectsPorDistancia->insertarOrdenado(obj);
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
        if (!gameObjectsPorDistancia->isEmpty()) {
            Position<GameObject*>* pos = gameObjectsPorDistancia->first();
            while (pos != nullptr && pos->getElement() != nullptr) {
                dibujarObject(pos->getElement());
                cout << "Dibujando: " << pos->getElement()->getId() << endl;
                pos = (pos != gameObjectsPorDistancia->last()) ? gameObjectsPorDistancia->next(pos) : nullptr;
            }
        }
    }

    void reSorting() {
        if (!gameObjectsPorDistancia->isEmpty()) {
            PriorityListaDE<GameObject*>* aux = new PriorityListaDE<GameObject*>(camera);
            //al lista ordenada ya no tiene orden de peso porque me movi
            Position<GameObject*>* pos = gameObjectsPorDistancia->last();
            while (pos != nullptr) {
                aux->insertarOrdenado(pos->getElement());//le doy peso
                pos = (pos != gameObjectsPorDistancia->first()) ? gameObjectsPorDistancia->prev(pos) : nullptr;
            }
            gameObjectsPorDistancia = aux;
        }
    }

    void createGameObject(GameObject* newGameObject) {
        gameObjectsPorDistancia->insertarOrdenado(newGameObject);
    }

    void dibujarObject(GameObject* obj) {
        //GameObject::selectedObject(obj, obj->buttonPress);
        obj->setTam(10);
        obj->setColor(obj->auxColor);
        obj->dibujar();
        //mejorar esto
       // setListObjectByID(getNodosScene()->listID, obj);
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

    void gameScene() {
        glLoadIdentity();
        camera->activar();

        //Autiomatizar Creacion De Objetos
        glPushMatrix();
        mallaScene(70.0);
        glPopMatrix();

        reSorting();
        dibujarGameObjects();
    }

};
#endif
