#ifndef DNODO_H
#define DNODO_H
#include "../Position/Position.h"
#include <iostream>
using namespace std;

template<typename E>
class DNodo : public Position<E> {
private:
    DNodo<E>* left;
    DNodo<E>* right;
    E element;

public:
    // Constructor correcto
    DNodo(E elem){
        element = elem;
    }

    // Implementación del método virtual puro
    E getElement() override {
        return element;
    }

    void setElement(E element) {
        this->element = element;
    }

    // Métodos para acceder a los nodos hijos
    DNodo<E>* getLeft() { return left; }
    DNodo<E>* getRight() { return right; }

    // Métodos para modificar los nodos hijos
    void setLeft(DNodo<E>* l) { left = l; }
    void setRight(DNodo<E>* r) { right = r; }

    // Destructor
    ~DNodo() {
        // Aquí normalmente se manejaría la liberación de memoria
        // si los nodos hijos son propiedad de este nodo
    }
};


#endif