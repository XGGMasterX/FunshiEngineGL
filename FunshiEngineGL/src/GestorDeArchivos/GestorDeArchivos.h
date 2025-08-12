#ifndef GESTORDEARCHIVOS_H
#define GESTORDEARCHIVOS_H

#include <iostream>
#include <string>
#include <vector>
#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <dirent.h>
#include <sys/stat.h>
#endif
#include "Carpeta.h"
using namespace std;

//recorre el sistema de carpetas del proyecto
//carga la estructura que usa la GUI con este sistema
class GestorDeArchivos {
private:
	ArbolEnlazado<File*>* treeFilePath;
    Position<File*>* folderActual;
public:
	GestorDeArchivos(string pathProyect) {
        treeFilePath = new ArbolEnlazado<File*>();
        setTreeFilePath(pathProyect);
	}

	ArbolEnlazado<File*>* getTreeFilePath() {
		return treeFilePath;
	}
    
    void recorrer(const std::string& path) {
        std::string searchPath = path + "\\*";

        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            std::cerr << "No se pudo abrir: " << path << '\n';
            return;
        }

        // Verifica si el directorio está vacío (solo contiene "." y "..")
        bool hayContenido = false;
        do {
            const char* nombre = findData.cFileName;
            if (strcmp(nombre, ".") != 0 && strcmp(nombre, "..") != 0) {
                hayContenido = true;
                break;
            }
        } while (FindNextFileA(hFind, &findData) != 0);

        FindClose(hFind);
        if (!hayContenido) return;

        // Volver a abrir para el recorrido real
        hFind = FindFirstFileA(searchPath.c_str(), &findData);
        if (hFind == INVALID_HANDLE_VALUE) return;

        do {
            const char* nombre = findData.cFileName;

            if (strcmp(nombre, ".") == 0 || strcmp(nombre, "..") == 0)
                continue;

            std::string fullPath = path + "\\" + nombre;

            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                Carpeta* newDirectory = new Carpeta(nombre);
                newDirectory->setPathRoot(path);

                if (treeFilePath->isEmpty()) {
                    folderActual = treeFilePath->createRoot(newDirectory);
                    recorrer(fullPath);
                }
                else {
                    Position<File*>* aux = folderActual;
                    folderActual = treeFilePath->addNodeChildOf(folderActual, newDirectory);
                    recorrer(fullPath);
                    folderActual = aux;
                }
            }
            else {
                File* newFile = new File(nombre);
                newFile->setPathRoot(path);
                treeFilePath->addNodeChildOf(folderActual, newFile);
            }

        } while (FindNextFileA(hFind, &findData) != 0);

        FindClose(hFind);
    }

	
	bool setTreeFilePath(const std::string& path) {
        ArbolEnlazado<File*>* nuevaTree = new ArbolEnlazado<File*>();
        ArbolEnlazado<File*>* original = treeFilePath;

        treeFilePath = nuevaTree;
        folderActual = nullptr;
        recorrer(path);

        if (!compareTreesByPath(original, nuevaTree)) {
            delete original;
            return true;
        }
        else {
            delete treeFilePath;
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
        if (file1->getPathName() != file2->getPathName() || file1->getPathRoot() != file2->getPathRoot()) {
            return false;
        }

       
        bool external1 = t1->isExternal(r1);
        bool external2 = t2->isExternal(r2);

        if (external1 && external2) {
            return true;
        }

        
        if (external1 != external2) {
            return false;
        }

        
        ListaDE<Position<File*>*>* hijos1 = t1->childsOf(r1);
        ListaDE<Position<File*>*>* hijos2 = t2->childsOf(r2);

        if (hijos1->tam() != hijos2->tam()) {
            return false;
        }

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

        bool eliminarCarpetaWindows(const std::string& path) {
            if (path.empty()) return false;

            // SHFILEOPSTRUCT requiere doble null terminador
            std::string pathDobleNull = path + '\0';
            pathDobleNull.push_back('\0');

            SHFILEOPSTRUCTA fileOp = { 0 };
            fileOp.wFunc = FO_DELETE;
            fileOp.pFrom = pathDobleNull.c_str();
            fileOp.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;

            int res = SHFileOperationA(&fileOp);
            return (res == 0 && !fileOp.fAnyOperationsAborted);
        }

        bool eliminarCarpetaLinux(const std::string& path) {
            if (path.empty()) return false;

            std::string comando = "rm -rf \"" + path + "\"";
            int res = system(comando.c_str());
            return (res == 0);
        }

public:
    // Método público que delega según plataforma
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

    private:

        bool crearCarpetaWindows(const std::string& path) {
            if (path.empty()) return false;

            // CreateDirectoryA devuelve 0 si falla, distinto de 0 si crea o ya existe
            BOOL res = CreateDirectoryA(path.c_str(), NULL);
            if (res == 0) {
                DWORD err = GetLastError();
                if (err == ERROR_ALREADY_EXISTS) {
                    return true; // ya existía la carpeta, ok
                }
                else {
                    std::cerr << "Error creando carpeta: " << err << "\n";
                    return false;
                }
            }
            return true;
        }

        bool crearCarpetaLinux(const std::string& path) {
            if (path.empty()) return false;

            // Usamos mkdir -p para crear la carpeta y todas sus padres
            std::string comando = "mkdir -p \"" + path + "\"";
            int res = system(comando.c_str());
            return (res == 0);
        }

public:
    // Método público que delega según plataforma
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