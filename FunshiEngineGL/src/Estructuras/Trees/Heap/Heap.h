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
#ifndef HEAP_H
#define HEAP_H

#include <vector>

// Interfaz de una cola de prioridad (monticulos) sobre E. Los monticulos se
// implementan sobre un vector contiguo (heap binario clasico): el acceso por
// indice da posiciones de hijos/padre en O(1) y push/pop en O(log n).
//
// E debe ser comparable con operadores relacionales:
//   - MinHeap<E> usa <  (el top es el menor).
//   - MaxHeap<E> usa >  (el top es el mayor).
template <typename E>
class Heap {
public:
    virtual ~Heap() = default;

    virtual bool isEmpty() const = 0;
    virtual int tam() const = 0;

    // Inserta un elemento manteniendo la propiedad del monticulo.
    virtual void push(E e) = 0;

    // Devuelve el extremo (min en MinHeap, max en MaxHeap) sin extraerlo.
    virtual E top() const = 0;

    // Extrae y devuelve el extremo.
    virtual E pop() = 0;

    // Elimina todos los elementos. Los elementos E no son propiedad del heap.
    virtual void clear() = 0;
};

#endif