#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#if defined(_WIN32)
#include <conio.h>
#elif defined(__linux__)
#include <ncurses.h>
#endif

#include <stdlib.h>
#if defined(_WIN32)
#include <glm.hpp>
#elif defined(__linux__)
#include <glm/glm.hpp>
#endif

#include "../Objetos/Componentes/Phisics.h"
#include "../Objetos/Componentes/Transform.h"
#include "../Objetos/Componentes/Color.h"
#include "../GestorDeArchivos/Binario.h"
#include "../Estructuras/Comparable.h"
#include "../Estructuras/ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"
#include "../Objetos/Componentes/Colliders/EsfereCollider.h"
#include "../Objetos/Componentes/Colliders/CubeCollider.h"
#include "../Objetos/Componentes/Colliders/MallaCollider.h"
#include "../Objetos/Componentes/RigidBody/RigidBody.h"
#include "../Objetos/Componentes/Script.h"
#include "../Entity/Entity.h"

using namespace std;

class GameObject : public Comparable<GameObject> , public Entity{

protected:

	bool state = true;
	int id;
	int tam = 1;

public:
	GameObject() {
	}

	virtual ~GameObject() {

	} // Destructor virtual para que se llame al de las clases derivadas

	char inputName[25];
	color auxColor = { 0.0f, 0.0f, 0.0f, 1.0f };

public:

	void addComponent(Component* component) override {
	   if (component != nullptr) {
	      	component->settingsObjectComponent = true;
	    	components->addLast(component);
	   }
	}

	//ASUMO QUE NO SE REPITEN
	void deleteComponent(Component* component) override {
		if (component != nullptr && !components->isEmpty()) {
			components->deleteByElement(component);
		}
	}

	ListaDE<Component*>* getComponents() override {
		return components;
	}

	int compareTo(GameObject* other) override {
		Transform* myTransform = getComponent<Transform>();
		Transform* otherTransform = other->getComponent<Transform>();

		if (!myTransform || !otherTransform) return 0;

		// Accede a las posiciones directamente
		float* myPos = myTransform->getTranslatef();
		float* otherPos = otherTransform->getTranslatef();

		// Calcula distancias al cuadrado (más eficiente)
		float myDist = myPos[0] * myPos[0] + myPos[1] * myPos[1] + myPos[2] * myPos[2];
		float otherDist = otherPos[0] * otherPos[0] + otherPos[1] * otherPos[1] + otherPos[2] * otherPos[2];

		return (myDist < otherDist) ? -1 : (myDist > otherDist) ? 1 : 0;
	}

	//quiero que devuelva la distancia de yo (this) a other
	// Devuelve la distancia euclidiana entre this y other
	float distanciaA(GameObject* other) {
		Transform* myTransform = this->getComponent<Transform>();
		Transform* otherTransform = other->getComponent<Transform>();

		if (!myTransform || !otherTransform) return FLT_MAX;

		float* myPos = myTransform->getTranslatef();
		float* otherPos = otherTransform->getTranslatef();

		float dx = myPos[0] - otherPos[0];
		float dy = myPos[1] - otherPos[1];
		float dz = myPos[2] - otherPos[2];

		return sqrt(dx * dx + dy * dy + dz * dz);
	}

	void setId(int id) {
		this->id = id;
	}

	void setState(bool state) {
		this->state = state;
	}

	void setTam(int tam) {
		this->tam = tam;
	}

	void setColor(color cor) {
		Color* color = getComponent<Color>();
		if (color != nullptr) {
			color->setColor(cor);
		}
	}

	const float* getColor(float*& arr, int tam) {
		return getComponent<Color>()->getColor();
	}

	Color* getColor() {
		return getComponent<Color>();
	}

	int getId() {
		return id;
	}

	bool getState() {
		return state;
	}

	int getTam() {
		return tam;
	}

public:
	virtual void dibujar(float deltaTime) = 0; //se deja para implementar

	virtual void update(float deltaTime) {
		if (getComponent<RigidBody>() != nullptr) {
			getComponent<RigidBody>()->syncPhysicsToGameObject();
		}
	}
protected:

	virtual void serializeEntityComponents() override {
		// Guardar el número de componentes
		size_t numComponents = components->tam();
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&numComponents), sizeof(size_t));

		if (!components->isEmpty()) {
			Position<Component*>* position = components->first();
			// Guardar cada componente
			while (position != nullptr) {
				Component* component = position->getElement();
				// Obtener el nombre del tipo y limpiar "class " si está presente (MSVC)
				std::string cleanTypeName = typeid(*component).name();
				const std::string classPrefix = "class ";
				if (cleanTypeName.compare(0, classPrefix.size(), classPrefix) == 0) {
					cleanTypeName = cleanTypeName.substr(classPrefix.size());
				}

				// Guardar el nombre del tipo limpio
				size_t typeNameLength = cleanTypeName.size();
				myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&typeNameLength), sizeof(size_t));
				myBinario->getOfBinariFile()->write(cleanTypeName.c_str(), typeNameLength);

				// Guardar los datos del componente
				component->saveComponent(myBinario->getOfBinariFile());
				position = (position != components->last()) ? components->next(position) : nullptr;
			}
		}
	}


	virtual void deserializeEntityComponents() override {
		//numero de componentes
		size_t numComponents = 0;
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&numComponents), sizeof(size_t));

		if (components->tam() > 0) {
			components->clear();
		}
		

		//leer cada componente , GENERALIZAR
		for (size_t i = 0; i < numComponents; ++i) {
			//leer el nombre del tipo
			size_t typeNameLength = 0;
			myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&typeNameLength), sizeof(size_t));
			std::string typeName(typeNameLength, '\0');
			myBinario->getIfBinariFile()->read(&typeName[0], typeNameLength);

			Component* component = nullptr;

			//cargo los tipos
			if (typeName == "Transform") {
				component = new Transform();
				component->loadComponent(myBinario->getIfBinariFile());
				addComponent(component);
			}
			else if (typeName == "Color") {
				component = new Color();
				component->loadComponent(myBinario->getIfBinariFile());
				addComponent(component);

				//los colores auxiliares
				Color* color = getComponent<Color>();
				if (color) {
					float const* c = color->getColor();
					for (int i = 0; i < 4; ++i) {
						auxColor[i] = c[i];
					}
				}
			}
			else if (typeName == "EsfereCollider") {
				component = new EsfereCollider(5.0f, getComponent<Transform>());
				component->loadComponent(myBinario->getIfBinariFile());
				addComponent(component);
			}
			else if (typeName == "CubeCollider" && getComponent<Transform>() != nullptr) {
				component = new CubeCollider(5.0f, getComponent<Transform>());
				component->loadComponent(myBinario->getIfBinariFile());
				addComponent(component);
			}
			else if (typeName == "MallaCollider" && getComponent<Transform>() != nullptr) {
				component = new MallaCollider(5.0f, getComponent<Transform>());
				component->loadComponent(myBinario->getIfBinariFile());
				addComponent(component);
			}
			else if (typeName == "RigidBody" && getComponent<Collider>() != nullptr) {
				component = new RigidBody(getComponent<Collider>(),1.0f);
				component->loadComponent(myBinario->getIfBinariFile());
				addComponent(component);
			}
			else if (typeName == "Script") {
				component = new Script();
				component->loadComponent(myBinario->getIfBinariFile());
				addComponent(component);
			}
			//SOPORTE PARA COMPONENTES SCRIPTS

			else {
				std::cerr << "Tipo de componente desconocido: " << typeName << "\n";
			}
		}
	}


	virtual void serializeEntity() override {
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&state), sizeof(bool));
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&id), sizeof(int));
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&tam), sizeof(int));
		myBinario->getOfBinariFile()->write(inputName, sizeof(char) * 25);

		//guarda atributos de componentes
		serializeEntityComponents();
	}


	virtual void deserializeEntity() override {
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&state), sizeof(bool));
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&id), sizeof(int));
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&tam), sizeof(int));
		myBinario->getIfBinariFile()->read(inputName, sizeof(inputName));

		//carga atributos de componentes
		deserializeEntityComponents();
	}


public:


	virtual void saveEntity(string filename) override {
		//crear binario y si existe lo limpia internamente el objeto Binario
		string path = filename + "/ObjectN" + to_string(getId()) + ".db";
		myBinario = new Binario(path);
		myBinario->ofOpenBinary();

		//almacena el binario en una base de datos
		std::ofstream archivo(filename + "BBDDObjetos.txt", std::ios::app);
		if (!archivo.is_open()) {
			std::cerr << "No se pudo abrir BBDDObjetos.txt" << std::endl;
			return;
		}
		archivo << path << std::endl;

		serializeEntity();

		myBinario->ofCloseBinary();
	}



	virtual void loadEntity(string filename) override {
		string path = filename + "/ObjectN" + to_string(getId()) + ".db";
		myBinario = new Binario(path);
		myBinario->ifOpenBinary();

		deserializeEntity();

		myBinario->ifCloseBinary();
	}
};
#endif
