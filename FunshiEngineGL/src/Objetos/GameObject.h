#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#if defined(_WIN32)
#include <conio.h>
#elif defined(__linux__)
#include <ncurses.h>
#endif

#include <stdlib.h>
#include <glm/glm.hpp>
#include "../Objetos/Componentes/Phisics.h"
#include "../Objetos/Componentes/Transform.h"
#include "../Objetos/Componentes/Color.h"
#include "../GestorDeArchivos/Binario.h"
#include "../Estructuras/Comparable.h"

using namespace std;
class GameObject : public Comparable<GameObject> {

protected:

	bool state = true;
	int id;
	int tam = 1;
	Binario* myBinario;

public:
	GameObject() {
		addComponent(new Transform());
		addComponent(new Color());
	}

	virtual ~GameObject() {

	} // Destructor virtual para que se llame al de las clases derivadas



	bool buttonPress = true;
	bool activeSelectedObject = true;
	bool activeComponentSettingsObject = false;
	//ImGui
	char inputName[25];
	bool dense;
	bool points;
	bool lines;
	color auxColor = { 0.0f, 0.0f, 0.0f, 1.0f };


	//COMPONENT
	std::vector<Component*> components;
	int inputId;
	//COLLIDER

	//GET ONE SPECIFIC COMPONENT M�todo para agregar un componente al GameObject.
	void addComponent(Component* component) {
		component->settingsObjectComponent = true;
		components.push_back(component);
	}

	// Metodo GetComponent.
	template <typename T>
	T* getComponent() {
		for (Component* componente : components) {
			if (dynamic_cast<T*>(componente)) {
				return dynamic_cast<T*>(componente); // Devuelve el componente si se encuentra y es casteable a <T>.
			}
		}
		return nullptr; // Devuelve nullptr si no se encuentra el componente.
	}

	//GET MULTIPLES SPECIFIC COMPONENT
	//GET IN (VARIABLE) OBJECT HERETED COMPONENT
	template <typename C, typename T, typename = std::enable_if_t<std::is_base_of_v<GameObject, T>>>
	C** getComponents(/*ARRAY para guardar Objects de Component , usando polimorfismo*/T* arr[], int tam,
		/*ARRAY para guardar Objects Component*/ C* arrComponent[], int tamComponent) {
		for (int i = 0; i < tamComponent; i++) {
			arrComponent[i] = arr[i]->template getComponent<C>();
		}
		return arrComponent;
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
	static void selectedObject(GameObject*, bool&);

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
		else {
			cout << "Color no es componente" << endl;
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

	Binario* getMyBinario() {
		return myBinario;
	}

public:
	virtual void dibujar() {}//se deja para implementar

	//HACER PARA COLOR Y COMPONENT LO MISMO
protected:

	virtual void serializeObjectComponents() {
		// Guardar el número de componentes
		size_t numComponents = components.size();
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&numComponents), sizeof(size_t));

		// Guardar cada componente
		for (const auto& component : components) {
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
		}
	}


	virtual void deserializeObjectComponents() {
		//numero de componentes
		size_t numComponents = 0;
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&numComponents), sizeof(size_t));

		components.clear();

		//leer cada componente
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
			else {
				std::cerr << "Tipo de componente desconocido: " << typeName << "\n";
			}
		}
	}


	virtual void serializeObject() {
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&state), sizeof(bool));
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&id), sizeof(int));
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&tam), sizeof(int));
		myBinario->getOfBinariFile()->write(inputName, sizeof(char) * 25);
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&dense), sizeof(bool));
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&points), sizeof(bool));
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&lines), sizeof(bool));
		myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&inputId), sizeof(int));

		//guarda atributos de componentes
		serializeObjectComponents();
	}


	virtual void deserializeObject() {
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&state), sizeof(bool));
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&id), sizeof(int));
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&tam), sizeof(int));
		myBinario->getIfBinariFile()->read(inputName, sizeof(inputName));
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&dense), sizeof(bool));
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&points), sizeof(bool));
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&lines), sizeof(bool));
		myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&inputId), sizeof(int));

		//carga atributos de componentes
		deserializeObjectComponents();
	}


public:


	virtual void saveObject(string filename) {
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

		serializeObject();

		myBinario->ofCloseBinary();
	}



	virtual void loadObject(string filename) {
		string path = filename + "/ObjectN" + to_string(getId()) + ".db";
		myBinario = new Binario(path);
		myBinario->ifOpenBinary();

		deserializeObject();

		myBinario->ifCloseBinary();
	}
};
#endif
