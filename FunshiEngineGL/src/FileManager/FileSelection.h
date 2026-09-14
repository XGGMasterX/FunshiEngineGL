#ifndef FILESELECTION_H
#define FILESELECTION_H

#include <string>

class Carpeta;

// Estado de navegacion compartido entre los dos paneles del explorador de
// archivos (arbol + contenido). Es la fuente de verdad unica de la seleccion:
//  - carpetaActual: carpeta visible en el panel de contenido. El arbol la
//    escribe; el panel de contenido simplemente la lee cada frame.
//  - rutaVisible: ruta completa de carpetaActual, usada para re-resolver el
//    puntero cuando el arbol se reconstruye por un rescaneo (los punteros al
//    arbol viejo quedarian colgando).
//  - navegacionPendiente: ruta a abrir (doble clic en el contenido). El arbol
//    la consume al inicio de su frame (aplicacion diferida, FASE 1 -> FASE 2)
//    y la borra tras resolverla contra el arbol vigente.
//  - contadorCambios: sube solo cuando una operacion de Filesystem puede
//    modificar el arbol (crear/copiar/eliminar CARPETAS). El arbol lo compara
//    con su vista previa y se rescancea al detectar un cambio. Las operaciones
//    que solo alteran archivos (que no aparecen en el arbol) NO lo incrementan
//    para no colapsar el arbol de carpetas de forma gratuita.
struct FileSelection {
    Carpeta* carpetaActual = nullptr;
    std::string rutaVisible;
    std::string navegacionPendiente;
    unsigned long contadorCambios = 0;
};

#endif