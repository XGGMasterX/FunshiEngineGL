#ifndef TREE_GUI_H
#define TREE_GUI_H

#include <set>
#include <imgui.h>

#include "../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"

// Desduplicacion entre jerarquias (escena, explorador de archivos,...):
// un unico recorrido pre-orden de ArbolEnlazado<T> que dibuja un
// TreeNodeEx por nodo, con estado de colapso persistente, PushID por nodo y
// restore de la pila de widgets. El contenido de la fila lo decide el
// renderizador (drawRow), que aporta la logica especifica de cada dominio.
namespace TreeIG {

// Estado de colapso compartido. Se indexa con el puntero del elemento: clave
// estable mientras el elemento viva en el arbol.
using OpenState = std::set<const void*>;

// Resultado del renderizador de una fila.
struct RowResult {
    bool open = false;    // true -> el nodo quedo abierto (se recorren hijos)
    bool toggled = false; // true -> el usuario toco la flecha este frame
};

// Recorre en pre-orden un ArbolEnlazado<T> (T = tipo de elemento, p.ej.
// GameObject*). La raiz se trata como contenedor: sus hijos se recorren sin
// dibujar la raiz misma. drawRow(elemento, wasOpen) devuelve RowResult.
// IMPORTANTE: drawRow debe leer IsItemToggledOpen() justo despues de dibujar
// su TreeNodeEx, antes de emitir menus/drag&drop (sobreescriben el "last item").
//
// Los hijos se recorren sobre la lista interna del TNodo (getChilds()) en
// lugar de childsOf(): esta estructura clasica nunca libera sus nodos y
// childsOf() construye una ListaDE nueva en cada llamada, con lo que con
// muchos objetos anidados se fugaba memoria por frame.
template <typename T, typename Fn>
void forEachChild(TNodo<T>* node, Fn&& fn) {
    ListaDE<TNodo<T>*>* children = node->getChilds();
    Position<TNodo<T>*>* child = children->first();
    while (child) {
        fn(child->getElement());
        child = (child != children->last()) ? children->next(child) : nullptr;
    }
}
template <typename T, typename Row>
void drawTree(ArbolEnlazado<T>* tree, Position<T>* pos, OpenState& openNodes,
              Row&& drawRow) {
    if (!tree || !pos) return;

    if (pos == tree->rootOfTree()) {
        if (!((TNodo<T>*)pos)->getChilds()->isEmpty()) {
            forEachChild((TNodo<T>*)pos, [&](Position<T>* child) {
                drawTree(tree, child, openNodes, drawRow);
            });
        }
        return;
    }

    // Solo los nodos hoja-vacia-nula no se dibujan; el resto, siempre.
    TNodo<T>* node = (TNodo<T>*)pos;
    T element = node->getElement();
    if (!element) return;

    // PushID por nodo: mientras el elemento viva, su puntero es una clave
    // estable y unica (evita IDs de ImGui compartidos entre nodos iguales).
    const void* key = element;
    ImGui::PushID(key);

    const bool wasOpen = openNodes.count(key) != 0;
    const RowResult result = drawRow(element, wasOpen);

    if (result.toggled) {
        if (wasOpen)
            openNodes.erase(key);
        else
            openNodes.insert(key);
    }

    // Contrato de ImGui: si TreeNodeEx devolvio true (nodo abierto) SE DEBE
    // llamar TreePop() esta misma frame, tenga o no hijos. Desplegar un nodo
    // sin hijos sin el TreePop() descuadra el TreeDepth interno de ImGui y
    // corrompe el dibujo/crash posterior de la ventana.
    if (result.open) {
        if (!node->getChilds()->isEmpty()) {
            forEachChild(node, [&](Position<T>* child) {
                drawTree(tree, child, openNodes, drawRow);
            });
        }
        ImGui::TreePop();
    }

    ImGui::PopID();
}

} // namespace TreeIG

#endif