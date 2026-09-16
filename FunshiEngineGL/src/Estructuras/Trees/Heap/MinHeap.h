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
#ifndef MINHEAP_H
#define MINHEAP_H

#include "Heap.h"

#include <algorithm>
#include <cassert>
#include <vector>

// Monticulo de minimos (cola de prioridad donde el menor queda arriba).
// Implementacion sobre std::vector (heap binario): push() en O(log n),
// top() en O(1) y pop() en O(log n). E se compara con operator<.
template <typename E>
class MinHeap : public Heap<E> {
private:
    std::vector<E> items;

    static int indicePadre(int i) { return (i - 1) / 2; }
    static int indiceHijoIzq(int i) { return 2 * i + 1; }
    static int indiceHijoDer(int i) { return 2 * i + 2; }

    // Sube el elemento en i hasta su posicion de monticulo (tras un push).
    void siftUp(int i) {
        while (i > 0 && items[i] < items[indicePadre(i)]) {
            std::swap(items[i], items[indicePadre(i)]);
            i = indicePadre(i);
        }
    }

    // Baja el elemento en i hasta restaurar la propiedad (tras un pop).
    void siftDown(int i) {
        const int n = static_cast<int>(items.size());
        while (true) {
            int menor = i;
            const int izq = indiceHijoIzq(i);
            const int der = indiceHijoDer(i);
            if (izq < n && items[izq] < items[menor]) menor = izq;
            if (der < n && items[der] < items[menor]) menor = der;
            if (menor == i) break;
            std::swap(items[i], items[menor]);
            i = menor;
        }
    }

public:
    MinHeap() = default;

    bool isEmpty() const override { return items.empty(); }
    int tam() const override { return static_cast<int>(items.size()); }

    void push(E e) override {
        items.push_back(e);
        siftUp(static_cast<int>(items.size()) - 1);
    }

    E top() const override {
        assert(!isEmpty() && "MinHeap::top sobre un heap vacio");
        return items.front();
    }

    E pop() override {
        assert(!isEmpty() && "MinHeap::pop sobre un heap vacio");
        E extremo = items.front();
        items.front() = items.back();
        items.pop_back();
        if (!items.empty()) siftDown(0);
        return extremo;
    }

    void clear() override { items.clear(); }
};

#endif