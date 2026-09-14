#ifndef GESTORDEARCHIVOS_H
#define GESTORDEARCHIVOS_H

#include <string>

#include "../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "Carpeta.h"

// Infraestructura de acceso al Filesystem del explorador de archivos:
// construye el arbol de carpetas de un proyecto y ejecuta las operaciones de
// dominio (crear/eliminar/copiar). R2: todo el acceso a disco usa
// std::filesystem (ambas plataformas) en lugar de APIS por #ifdef
// (dirent.h / WinAPI) y de system("rm -rf ...").
//
// Nunca es dueno del ArbolEnlazado salvo del arbol que construye: libera los
// File* que crea (los nodos del arbol no son duenos de sus elementos).
class GestorDeArchivos {
private:
    ArbolEnlazado<File*>* treeFilePath;
    Position<File*>* folderActual;

    void recorrerDir(const std::string& path, Position<File*>* parent);
    // Borra los File* (elementos) de un arbol en pre-orden. El arbol de
    // nodos (TNodo/ListaDE) no libera sus elementos al destruirse.
    void liberarPreOrden(ArbolEnlazado<File*>* arbol, Position<File*>* p);
    bool compareTreesByPath(ArbolEnlazado<File*>* first,
                            ArbolEnlazado<File*>* second);
    bool preOrdenNoExaustivo(ArbolEnlazado<File*>* first,
                             ArbolEnlazado<File*>* second,
                             Position<File*>* left, Position<File*>* right);

public:
    explicit GestorDeArchivos(std::string pathProyect);
    ~GestorDeArchivos();

    GestorDeArchivos(const GestorDeArchivos&) = delete;
    GestorDeArchivos& operator=(const GestorDeArchivos&) = delete;

    ArbolEnlazado<File*>* getTreeFilePath();
    bool setTreeFilePath(const std::string& path, std::string name);
    bool eliminarCarpeta(const std::string& path);
    bool crearCarpeta(const std::string& path);
    bool crearArchivo(const std::string& path, const std::string& contenido);
    bool copiarCarpeta(const std::string& origen, const std::string& destino);
    bool copiarArchivo(const std::string& origen, const std::string& destino);
};

#endif