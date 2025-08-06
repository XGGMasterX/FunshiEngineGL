#ifndef TNODO_H
#define TNODO_H
#include "../Position/Position.h"
#include <iostream>
#include "../ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"

using namespace std;
template<typename E>
//La relacion con Position es puramente polimorfica
//No uso Position pq la generacion de instancias TNodo esta encapsulada
//Dicho esto siempre operaremos con TNodo por conveniencia tecnica
class TNodo : public Position<E> {
protected:
	E element;
	TNodo<E>* rootDad;
	ListaDE<TNodo<E>*>* childs;
public:

	TNodo(E element, TNodo<E>* rootDad) {
		this->element = element;
		this->rootDad = rootDad;
		childs = new ListaDE<TNodo<E>*>();
	}

	TNodo(E element) : TNodo(element, nullptr) {
	}

	void setRootDad(TNodo<E>* rootDad) { this->rootDad = rootDad; }
	void setElement(E element) { this->element = element; }
	
	//los modifiadores setters para childs los tiene la estructura
	ListaDE<TNodo<E>*>* getChilds() { return childs; }
	TNodo<E>* getRootDad() { return rootDad; }
	E getElement() override { return element; }
};
#endif