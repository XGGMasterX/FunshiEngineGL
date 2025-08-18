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


#include "../Herramientas/TypeUtils.h"
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
#include "../Objetos/Componentes/Model.h"
#include "../Entity/Entity.h"

using namespace std;

class GameObject : public Comparable<GameObject> , public Entity{

protected:

	bool state = true;
	int id;
	int tam = 1;

public:
	GameObject(Entity* origin) : Entity(origin){
	}
 GameObject() : Entity(){}

	virtual ~GameObject() {

	} // Destructor virtual para que se llame al de las clases derivadas

	char inputName[25] = "";
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

 //GUARDADOS
 virtual void serializeGlobalAtributes() override{
  serializeExternalAtributes();
  serializeLocalAtributes();
 }
 virtual void serializeLocalAtributes() override{
  //guarda atributos declarados
   myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&state), sizeof(bool));
   myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&id), sizeof(int));
   myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&tam), sizeof(int));
   myBinario->getOfBinariFile()->write(inputName, sizeof(char) * 25);

   serializeEntityComponents();
 }
 virtual void serializeExternalAtributes() override{
  //guarda atributos heredados
  serializeTransformOrigin();
 }

 //SERIALIACION EXTERNAL ATRIBUTES
virtual void serializeTransformOrigin() override {
    bool hasTransform = (transformOrigin != nullptr);
    myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&hasTransform), sizeof(bool));

    if (hasTransform) {
        // guardamos nombre fijo para identificar
        std::string typeName = "TransformOrigin";
        size_t typeNameLength = typeName.size();
        myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&typeNameLength), sizeof(size_t));
        myBinario->getOfBinariFile()->write(typeName.c_str(), typeNameLength);

        // guardamos los datos
        transformOrigin->saveComponent(myBinario->getOfBinariFile());
    }
}
 

 //CARGAS
 virtual void deserializeGlobalAtributes() override{
  deserializeExternalAtributes();
  deserializeLocalAtributes();
 }
 virtual void deserializeLocalAtributes() override{
  //carga atributos declarados
   myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&state), sizeof(bool));
   myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&id), sizeof(int));
   myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&tam), sizeof(int));
   myBinario->getIfBinariFile()->read(inputName, sizeof(inputName));
   
   deserializeEntityComponents();
 }
 virtual void deserializeExternalAtributes() override{
  //carga atributos heredados
  deserializeTransformOrigin();
 }

 //SERIALIZACION EXTERNAL ATRIBUTES
virtual void deserializeTransformOrigin() override {
    bool hasTransform = false;
    myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&hasTransform), sizeof(bool));

    if (hasTransform) {
        size_t typeNameLength = 0;
        myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&typeNameLength), sizeof(size_t));
        std::string typeName(typeNameLength, '\0');
        myBinario->getIfBinariFile()->read(&typeName[0], typeNameLength);

        if (typeName == "TransformOrigin") {
            if (!transformOrigin) {
                transformOrigin = new Transform();
            }
            transformOrigin->loadComponent(myBinario->getIfBinariFile());
        }
        else {
            std::cerr << "Tipo de atributo heredado desconocido: " << typeName << "\n";
        }
    }
}

 //OTROS

virtual void serializeEntityComponents() override {
    size_t numComponents = components->tam();
    myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&numComponents), sizeof(size_t));

    if (!components->isEmpty()) {
        Position<Component*>* position = components->first();
        while (position != nullptr) {
            Component* component = position->getElement();

            // Obtener nombre limpio multiplataforma
            std::string cleanTypeName = demangle(typeid(*component).name());

#if defined(_MSC_VER) // Compilador MSVC
            const std::string classPrefix = "class ";
            if (cleanTypeName.compare(0, classPrefix.size(), classPrefix) == 0) {
                cleanTypeName = cleanTypeName.substr(classPrefix.size());
            }
#endif

            // Guardar nombre del tipo
            size_t typeNameLength = cleanTypeName.size();
            myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&typeNameLength), sizeof(size_t));
            myBinario->getOfBinariFile()->write(cleanTypeName.c_str(), typeNameLength);

            // Guardar datos del componente
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
             else if(typeName == "Model"){
                component = new Model();
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
  //guarda los atributos
  serializeGlobalAtributes();
	}


	virtual void deserializeEntity() override {
  //carga los atributos
  deserializeGlobalAtributes();
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

virtual Transform* getGlobalTransform(){
 Transform* resultado = getComponent<Transform>();
 if(transformOrigin != nullptr){
 float dx = 0.0f;
 float dy = 0.0f;
 float dz = 0.0f;
 //TRANSLACION
 float* myPos = resultado->getTranslatef();
 float* myOrigin = transformOrigin->getTranslatef();
 dx = myPos[0] + myOrigin[0];
 dy = myPos[1] + myOrigin[1];
 dz = myPos[2] + myOrigin[2];
 //OTRAS IFLUENCIAS TEMPORALES
 float* myRot = resultado->getRotatef();
 float* myScale = resultado->getScalef();
 resultado = new Transform();
 resultado->setTranslatef(dx,dy,dz);
 //ROTACION
 resultado->setRotatef(myRot[0],myRot[1],myRot[2],myRot[3]);
 //SLALACION
 resultado->setScalef(myScale[0],myScale[1],myScale[2]);
 }
 return resultado;
 }
};
#endif
