/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
#ifndef LISTMERGESORT_H
#define LISTMERGESORT_H

#include "../ListasEnlazadas/ListasDoblementeEnlazada/ListaDE.h"

#include <functional>

// Merge sort estable en O(n log n) sobre ListaDE<E>.
//
// Ordena la MISMA lista pasada (no devuelve una copia): ListaDE no tiene
// constructor de copia propio y una copia por valor haria doble free de sus
// nodos. La ordenacion usa la interfaz publica (first/remove/addLast/tam):
// los elementos E se MUEVEN entre listas, nunca se copian, y ListaDE no es
// duena de los elementos (punteros/valores), asi que el resultado sigue
// referenciando los mismos elementos.
//
// Cmp debe implementar cmp(a, b) == true si a va antes que b
// (por defecto std::less<E>).
template <typename E, typename Cmp = std::less<E>>
class ListMergeSort {
private:
    // Reparte los elementos de `lista` en dos mitades iguales.
    static void split(ListaDE<E>& lista, ListaDE<E>& izq, ListaDE<E>& der) {
        const int mitad = lista.tam() / 2;
        int i = 0;
        while (!lista.isEmpty()) {
            if (i < mitad)
                izq.addLast(lista.remove(lista.first()));
            else
                der.addLast(lista.remove(lista.first()));
            ++i;
        }
    }

    // Fusiona dos listas ordenadas en la lista destino (vacia), tomando cada
    // vez el menor primero (estabilidad: empates -> el de la izquierda).
    static void merge(ListaDE<E>& destino, ListaDE<E>& izq,
                      ListaDE<E>& der, const Cmp& cmp) {
        while (!izq.isEmpty() && !der.isEmpty()) {
            // Se prefiere el de `izq` cuando cmp decide que va primero;
            // en empate sigue cayendo primero el de la izquierda (estable).
            if (cmp(izq.first()->getElement(), der.first()->getElement()))
                destino.addLast(izq.remove(izq.first()));
            else
                destino.addLast(der.remove(der.first()));
        }
        while (!izq.isEmpty())
            destino.addLast(izq.remove(izq.first()));
        while (!der.isEmpty())
            destino.addLast(der.remove(der.first()));
    }

    static void ordenar(ListaDE<E>& lista, const Cmp& cmp) {
        if (lista.tam() > 1) {
            ListaDE<E> izq, der;
            split(lista, izq, der);
            ordenar(izq, cmp);
            ordenar(der, cmp);
            merge(lista, izq, der, cmp);
        }
    }

public:
    // Ordena `lista` in-place segun `cmp`.
    static void sort(ListaDE<E>& lista, Cmp cmp = Cmp{}) {
        ordenar(lista, cmp);
    }
};

#endif