#include "GestorDeArchivos.h"

#include <cstring>
#include <iostream>
#include <utility>
#if defined(_WIN32)
#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <shlobj.h>
#elif defined(__linux__)
#include <cerrno>
#include <dirent.h>
#include <sys/stat.h>
#endif

GestorDeArchivos::GestorDeArchivos(std::string pathProyect)
    : treeFilePath(new ArbolEnlazado<File*>()), folderActual(nullptr) {
    setTreeFilePath(pathProyect, "MotorGrafico");
}

GestorDeArchivos::~GestorDeArchivos() {
    if (treeFilePath && !treeFilePath->isEmpty())
        liberarPreOrden(treeFilePath, treeFilePath->rootOfTree());
    delete treeFilePath;
    treeFilePath = nullptr;
    folderActual = nullptr;
}

ArbolEnlazado<File*>* GestorDeArchivos::getTreeFilePath() { return treeFilePath; }

#if defined(_WIN32)
void GestorDeArchivos::recorrerDir(const std::string& path, Position<File*>* parent) {
    WIN32_FIND_DATAA data;
    HANDLE handle = FindFirstFileA((path + "\\*").c_str(), &data);
    if (handle == INVALID_HANDLE_VALUE) return;
    do {
        const char* name = data.cFileName;
        if (!std::strcmp(name, ".") && !std::strcmp(name, "..")) continue;
        if (!std::strcmp(name, ".") || !std::strcmp(name, "..")) continue;
        const std::string fullPath = path + "\\" + name;
        File* item = (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            ? static_cast<File*>(new Carpeta(name)) : new File(name);
        item->setPathRoot(path);
        Position<File*>* child = treeFilePath->addNodeChildOf(parent, item);
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) recorrerDir(fullPath, child);
    } while (FindNextFileA(handle, &data));
    FindClose(handle);
}
#elif defined(__linux__)
void GestorDeArchivos::recorrerDir(const std::string& path, Position<File*>* parent) {
    DIR* dir = opendir(path.c_str());
    if (!dir) return;
    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0) continue;
        const std::string fullPath = path + "/" + entry->d_name;
        struct stat info;
        if (stat(fullPath.c_str(), &info) != 0) continue;
        // No seguir enlaces simbolicos: pueden apuntar a carpetas del sistema
        // o a si mismos (recursion infinita -> arbol gigante / stack overflow).
        struct stat linkInfo;
        if (lstat(fullPath.c_str(), &linkInfo) == 0 &&
            S_ISLNK(linkInfo.st_mode))
            continue;
        File* item = S_ISDIR(info.st_mode)
            ? static_cast<File*>(new Carpeta(entry->d_name)) : new File(entry->d_name);
        item->setPathRoot(path);
        Position<File*>* child = treeFilePath->addNodeChildOf(parent, item);
        if (S_ISDIR(info.st_mode)) recorrerDir(fullPath, child);
    }
    closedir(dir);
}
#endif

bool GestorDeArchivos::setTreeFilePath(const std::string& path, std::string name) {
    auto* newTree = new ArbolEnlazado<File*>();
    Carpeta* root = new Carpeta(std::move(name));
    root->setPathRoot(path);
    Position<File*>* rootPosition = newTree->createRoot(root);
    auto* original = treeFilePath;
    treeFilePath = newTree;
    recorrerDir(path, rootPosition);
    if (!compareTreesByPath(original, newTree)) {
        folderActual = rootPosition;
        // Liberar el arbol reemplazado junto a sus File*.
        if (original && !original->isEmpty())
            liberarPreOrden(original, original->rootOfTree());
        delete original;
        return true;
    }
    treeFilePath = original;
    // Liberar el arbol descartado en la comparacion junto a sus File*.
    if (newTree && !newTree->isEmpty())
        liberarPreOrden(newTree, newTree->rootOfTree());
    delete newTree;
    return false;
}

void GestorDeArchivos::liberarPreOrden(ArbolEnlazado<File*>* arbol,
                                       Position<File*>* p) {
    if (!arbol || !p) return;
    delete p->getElement();
    if (arbol->isInternal(p)) {
        auto* hijos = arbol->childsOf(p);
        auto* h = hijos->first();
        while (h) {
            liberarPreOrden(arbol, h->getElement());
            h = (h != hijos->last()) ? hijos->next(h) : nullptr;
        }
        delete hijos; // childsOf() asigna una lista nueva en cada llamada
    }
}

bool GestorDeArchivos::compareTreesByPath(ArbolEnlazado<File*>* first,
                                          ArbolEnlazado<File*>* second) {
    if (first->isEmpty() && second->isEmpty()) return true;
    if (first->isEmpty() || second->isEmpty()) return false;
    return preOrdenNoExaustivo(first, second, first->rootOfTree(), second->rootOfTree());
}

bool GestorDeArchivos::preOrdenNoExaustivo(ArbolEnlazado<File*>* first,
                                           ArbolEnlazado<File*>* second,
                                           Position<File*>* left, Position<File*>* right) {
    if (!left || !right) return false;
    File* a = left->getElement();
    File* b = right->getElement();
    if (a->getPathName() != b->getPathName() || a->getPathRoot() != b->getPathRoot()) return false;
    const bool externalA = first->isExternal(left), externalB = second->isExternal(right);
    if (externalA && externalB) return true;
    if (externalA != externalB) return false;
    auto* childrenA = first->childsOf(left);
    auto* childrenB = second->childsOf(right);
    if (childrenA->tam() != childrenB->tam()) {
        delete childrenA; // childsOf() asigna una lista nueva en cada llamada
        delete childrenB;
        return false;
    }
    auto* pA = childrenA->first();
    auto* pB = childrenB->first();
    while (pA && pB) {
        if (!preOrdenNoExaustivo(first, second, pA->getElement(), pB->getElement())) {
            delete childrenA;
            delete childrenB;
            return false;
        }
        pA = (pA != childrenA->last()) ? childrenA->next(pA) : nullptr;
        pB = (pB != childrenB->last()) ? childrenB->next(pB) : nullptr;
    }
    delete childrenA;
    delete childrenB;
    return true;
}

#if defined(_WIN32)
bool GestorDeArchivos::eliminarCarpetaWindows(const std::string& path) {
    if (path.empty()) return false;
    std::string doubleNull = path + '\0';
    doubleNull.push_back('\0');
    SHFILEOPSTRUCTA operation{};
    operation.wFunc = FO_DELETE;
    operation.pFrom = doubleNull.c_str();
    operation.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
    const int result = SHFileOperationA(&operation);
    return result == 0 && !operation.fAnyOperationsAborted;
}
bool GestorDeArchivos::crearCarpetaWindows(const std::string& path) {
    if (path.empty()) return false;
    if (CreateDirectoryA(path.c_str(), nullptr)) return true;
    return GetLastError() == ERROR_ALREADY_EXISTS;
}
#elif defined(__linux__)
bool GestorDeArchivos::eliminarCarpetaLinux(const std::string& path) {
    return !path.empty() && std::system(("rm -rf \"" + path + "\"").c_str()) == 0;
}
bool GestorDeArchivos::crearCarpetaLinux(const std::string& path) {
    if (path.empty()) return false;
    if (mkdir(path.c_str(), 0777) == 0 || errno == EEXIST) return true;
    return false;
}
#endif

bool GestorDeArchivos::eliminarCarpeta(const std::string& path) {
#if defined(_WIN32)
    return eliminarCarpetaWindows(path);
#elif defined(__linux__)
    return eliminarCarpetaLinux(path);
#else
    return false;
#endif
}

bool GestorDeArchivos::crearCarpeta(const std::string& path) {
#if defined(_WIN32)
    return crearCarpetaWindows(path);
#elif defined(__linux__)
    return crearCarpetaLinux(path);
#else
    return false;
#endif
}
