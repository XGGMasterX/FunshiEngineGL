#ifndef GESTORDEARCHIVOS_H
#define GESTORDEARCHIVOS_H

#include <iostream>
#include <string>
#include <vector>

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <windows.h>
#include <shlobj.h>
#elif defined(__linux__)
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#endif

#include "../Estructuras/Trees/ArbolesEnlazados/ArbolEnlazado.h"
#include "Carpeta.h"

using namespace std;

class GestorDeArchivos {
private:
    ArbolEnlazado<File*>* treeFilePath;
    // folderActual ya no es necesario para construir el árbol, pero lo dejo si lo usás en otro lado
    Position<File*>* folderActual;

public:
    GestorDeArchivos(string pathProyect) {
        treeFilePath = new ArbolEnlazado<File*>();
        setTreeFilePath(pathProyect, "MotorGrafico");
    }

    ArbolEnlazado<File*>* getTreeFilePath() {
        return treeFilePath;
    }

private:
    // --- Helpers de recorrido por plataforma ---
#if defined(_WIN32)
    void recorrerDir(const std::string& path, Position<File*>* parent) {
        std::string searchPath = path + "\\*";
        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
        if (hFind == INVALID_HANDLE_VALUE) {
            std::cerr << "No se pudo abrir: " << path << '\n';
            return;
        }

        do {
            const char* nombre = findData.cFileName;
            if (strcmp(nombre, ".") == 0 || strcmp(nombre, "..") == 0)
                continue;

            std::string fullPath = path + "\\" + nombre;

            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                // PREORDEN: primero agrego la carpeta como hijo del padre actual
                Carpeta* newDirectory = new Carpeta(nombre);
                newDirectory->setPathRoot(path); // padre real del item
                //cout << path+"/"+ nombre << endl;
                Position<File*>* posHijo = treeFilePath->addNodeChildOf(parent, newDirectory);

                //luego recorro esa carpeta
                recorrerDir(fullPath, posHijo);
            } else {
                File* newFile = new File(nombre);
                newFile->setPathRoot(path); // padre real del item
                treeFilePath->addNodeChildOf(parent, newFile);
            }

        } while (FindNextFileA(hFind, &findData) != 0);

        FindClose(hFind);
    }
#elif defined(__linux__)
    void recorrerDir(const std::string& path, Position<File*>* parent) {
        DIR* dir = opendir(path.c_str());
        if (!dir) {
            std::cerr << "No se pudo abrir: " << path << '\n';
            return;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            const char* nombre = entry->d_name;
            if (strcmp(nombre, ".") == 0 || strcmp(nombre, "..") == 0)
                continue;

            std::string fullPath = path + "/" + nombre;

            struct stat info;
            if (stat(fullPath.c_str(), &info) != 0) {
                continue;
            }

            if (S_ISDIR(info.st_mode)) {
                // PREORDEN
                Carpeta* newDirectory = new Carpeta(nombre);
                newDirectory->setPathRoot(path);
                //cout << "Carpeta en :" << fullPath << endl;
                Position<File*>* posHijo = treeFilePath->addNodeChildOf(parent, newDirectory);
                recorrerDir(fullPath, posHijo);
            } else {
                File* newFile = new File(nombre);
                newFile->setPathRoot(path);
                treeFilePath->addNodeChildOf(parent, newFile);
            }
        }
        closedir(dir);
    }
#endif

public:
    // Construye un árbol nuevo a partir de 'path'. La raíz será 'name' con pathRoot = path (el proyecto).
    bool setTreeFilePath(const std::string& path, std::string name) {
        // Creo un árbol nuevo con la RAÍZ del proyecto
        ArbolEnlazado<File*>* nuevaTree = new ArbolEnlazado<File*>();
        Carpeta* root = new Carpeta(name);
        root->setPathRoot(path);//la raíz es el propio proyecto
        //cout << path << endl;
        Position<File*>* posRoot = nuevaTree->createRoot(root);

        // Guardo referencia del árbol anterior
        ArbolEnlazado<File*>* original = treeFilePath;

        // Apunto el treeFilePath al nuevo temporalmente
        treeFilePath = nuevaTree;

        // Construyo en PREORDEN bajo la raíz (posRoot)
        recorrerDir(path, posRoot);

        // Comparo con el anterior
        if (!compareTreesByPath(original, nuevaTree)) {
            // Son diferentes -> conservar el nuevo
            // (Nota: si querés evitar fugas, acá deberías borrar 'original')
            folderActual = posRoot; // opcional, por si lo usás
            return true;
        } else {
            // Son iguales -> descartar el nuevo y volver al original
            // (Nota: si querés evitar fugas, acá deberías borrar 'nuevaTree')
            treeFilePath = original;
            return false;
        }
    }

    bool compareTreesByPath(ArbolEnlazado<File*>* t1, ArbolEnlazado<File*>* t2) {
        if (t1->isEmpty() && t2->isEmpty()) return true;
        if (t1->isEmpty() || t2->isEmpty()) return false;
        return preOrdenNoExaustivo(t1, t2, t1->rootOfTree(), t2->rootOfTree());
    }

    bool preOrdenNoExaustivo(ArbolEnlazado<File*>* t1, ArbolEnlazado<File*>* t2,
                              Position<File*>* r1, Position<File*>* r2) {
        if (r1 == nullptr || r2 == nullptr) return false;

        File* file1 = r1->getElement();
        File* file2 = r2->getElement();
        if (file1->getPathName() != file2->getPathName() ||
            file1->getPathRoot() != file2->getPathRoot()) {
            return false;
        }

        bool external1 = t1->isExternal(r1);
        bool external2 = t2->isExternal(r2);
        if (external1 && external2) return true;
        if (external1 != external2) return false;

        ListaDE<Position<File*>*>* hijos1 = t1->childsOf(r1);
        ListaDE<Position<File*>*>* hijos2 = t2->childsOf(r2);
        if (hijos1->tam() != hijos2->tam()) return false;

        Position<Position<File*>*>* p1 = hijos1->first();
        Position<Position<File*>*>* p2 = hijos2->first();
        while (p1 != nullptr && p2 != nullptr) {
            if (!preOrdenNoExaustivo(t1, t2, p1->getElement(), p2->getElement())) {
                return false;
            }
            p1 = (p1 != hijos1->last()) ? hijos1->next(p1) : nullptr;
            p2 = (p2 != hijos2->last()) ? hijos2->next(p2) : nullptr;
        }
        return true;
    }

private:
#if defined(_WIN32)
    bool eliminarCarpetaWindows(const std::string& path) {
        if (path.empty()) return false;
        std::string pathDobleNull = path + '\0';
        pathDobleNull.push_back('\0');
        SHFILEOPSTRUCTA fileOp = { 0 };
        fileOp.wFunc = FO_DELETE;
        fileOp.pFrom = pathDobleNull.c_str();
        fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
        int res = SHFileOperationA(&fileOp);
        return (res == 0 && !fileOp.fAnyOperationsAborted);
    }

    bool crearCarpetaWindows(const std::string& path) {
        if (path.empty()) return false;
        BOOL res = CreateDirectoryA(path.c_str(), NULL);
        if (res == 0) {
            DWORD err = GetLastError();
            if (err == ERROR_ALREADY_EXISTS)
                return true;
            std::cerr << "Error creando carpeta (Windows): " << err << "\n";
            return false;
        }
        return true;
    }
#endif

#if defined(__linux__)
    bool eliminarCarpetaLinux(const std::string& path) {
        if (path.empty()) return false;
        std::string comando = "rm -rf \"" + path + "\"";
        int res = system(comando.c_str());
        return (res == 0);
    }

    bool crearCarpetaLinux(const std::string& path) {
        if (path.empty()) return false;
        if (mkdir(path.c_str(), 0777) == 0) return true;
        if (errno == EEXIST) return true;
        std::cerr << "Error creando carpeta (Linux): " << strerror(errno) << "\n";
        return false;
    }
#endif

public:
    bool eliminarCarpeta(const std::string& path) {
#if defined(_WIN32)
        return eliminarCarpetaWindows(path);
#elif defined(__linux__)
        return eliminarCarpetaLinux(path);
#else
        std::cerr << "Eliminar ruta no soportado en esta plataforma\n";
        return false;
#endif
    }

    bool crearCarpeta(const std::string& path) {
#if defined(_WIN32)
        return crearCarpetaWindows(path);
#elif defined(__linux__)
        return crearCarpetaLinux(path);
#else
        std::cerr << "Crear carpeta no soportado en esta plataforma\n";
        return false;
#endif
    }
};

#endif
