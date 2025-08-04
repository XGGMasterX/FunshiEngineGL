#ifndef PRIORITYLISTADE_H
#define PRIORITYLISTADE_H
#include "../ListasDoblementeEnlazada/ListaDE.h"
#include "../../Ordenadora.h"

using namespace std;

template <typename E>
class PriorityListaDE : public ListaDE<E>, public Ordenadora<E> {
private:
    E referenciaDeComparacion;
public:
    PriorityListaDE(E ref) {
        referenciaDeComparacion = ref;
    }

    virtual void insertarOrdenado(E e) {
        if (ListaDE<E>::isEmpty()) {
            ListaDE<E>::addLast(e);
        }
        else {
            E aux = e;
            Position<E>* position = ListaDE<E>::first();
            while (position != nullptr) {
                if (ordenBy(aux, position->getElement())) {
                    //burbujea el peso hacia arriba dado que insertas siempre
                    //de forma ordenada nunca se mezclan pesos y es de menor a mayor
                    aux = ListaDE<E>::remplace(position, aux);
                }
                //aseguro el burbujeo de forma constante si se remplaca uno se remplazan todos
                position = (position != ListaDE<E>::last()) ? ListaDE<E>::next(position) : nullptr;
            }
            ListaDE<E>::addLast(aux); //dado que hay siempre uno afuera , lo agrego a lo ultimo
            //porque aux en el final siempre sera el mas pesado
        }
    }

    virtual bool ordenBy(E p1, E p2) const override {
        if (!p1 || !p2) return false;
        return p1->distanciaA(referenciaDeComparacion) < p2->distanciaA(referenciaDeComparacion);
    }
};
#endif