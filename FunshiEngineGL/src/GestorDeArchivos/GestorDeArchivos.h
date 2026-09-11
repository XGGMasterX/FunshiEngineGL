#ifndef GESTORDEARCHIVOS_H
#define GESTORDEARCHIVOS_H

#include <string>

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <windows.h>
#include <shlobj.h>
#elif defined(__linux__)
#include <dirent.h>
#include <sys/stat.h>
#endif

#include "../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "Carpeta.h"

class GestorDeArchivos {
private:
    ArbolEnlazado<File*>* treeFilePath;
    Position<File*>* folderActual;

    void recorrerDir(const std::string& path, Position<File*>* parent);
    bool compareTreesByPath(ArbolEnlazado<File*>* first,
                            ArbolEnlazado<File*>* second);
    bool preOrdenNoExaustivo(ArbolEnlazado<File*>* first,
                             ArbolEnlazado<File*>* second,
                             Position<File*>* left, Position<File*>* right);

#if defined(_WIN32)
    bool eliminarCarpetaWindows(const std::string& path);
    bool crearCarpetaWindows(const std::string& path);
#elif defined(__linux__)
    bool eliminarCarpetaLinux(const std::string& path);
    bool crearCarpetaLinux(const std::string& path);
#endif

public:
    explicit GestorDeArchivos(std::string pathProyect);
    ~GestorDeArchivos();

    GestorDeArchivos(const GestorDeArchivos&) = delete;
    GestorDeArchivos& operator=(const GestorDeArchivos&) = delete;

    ArbolEnlazado<File*>* getTreeFilePath();
    bool setTreeFilePath(const std::string& path, std::string name);
    bool eliminarCarpeta(const std::string& path);
    bool crearCarpeta(const std::string& path);
};

#endif
