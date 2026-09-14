#include "FileManager.h"

#include "FileSystemWatcher.h"
#include "../GestorDeArchivos/Carpeta.h"
#include "../Herramientas/PathUtils.h"
#include "../Herramientas/TreeGUI/TreeGUI.h"

namespace {
// Busca pre-orden la primera Carpeta cuya ruta completa coincida. La raiz se
// trata como contenedor (su pathRoot+pathName no es una ruta real).
Carpeta* buscarPreOrden(ArbolEnlazado<File*>* arbol,
                        Position<File*>* current,
                        const std::string& ruta) {
    if (!arbol || !current) return nullptr;
    File* elemento = current->getElement();
    if (elemento && current != arbol->rootOfTree() &&
        (elemento->getPathRoot() + PATH_SEP + elemento->getPathName()) == ruta) {
        return dynamic_cast<Carpeta*>(elemento);
    }
    if (arbol->isInternal(current)) {
        Carpeta* encontrado = nullptr;
        TreeIG::forEachChild((TNodo<File*>*)current, [&](Position<File*>* hijo) {
            if (!encontrado) encontrado = buscarPreOrden(arbol, hijo, ruta);
        });
        return encontrado;
    }
    return nullptr;
}
} // namespace

FileManager::FileManager(const std::string& pathProyect)
    : pathProyect(pathProyect),
      gestor(new GestorDeArchivos(pathProyect)),
      vigilante(new FileSystemWatcher(pathProyect)) {}

FileManager::~FileManager() = default;

void FileManager::refrescar() {
    FileSelection* sel = getSelection();
    const std::string rutaVisible = sel->rutaVisible;
    // GestorDeArchivos ya comprueba por rutas y solo reconstruye si cambio.
    gestor->setTreeFilePath(pathProyect, "MotorGrafico");
    // Los punteros al arbol viejo quedaron liberados (o apuntarian a una
    // seleccion caducada): re-resolvemos la carpeta visible por su ruta.
    sel->carpetaActual = nullptr;
    if (!rutaVisible.empty())
        sel->carpetaActual = buscarCarpetaPorRuta(rutaVisible);
}

bool FileManager::huboCambiosExternos() {
    return vigilante ? vigilante->huboCambiosYConsumir() : false;
}

bool FileManager::crearCarpeta(const std::string& ruta) {
    return gestor->crearCarpeta(ruta);
}

bool FileManager::eliminarCarpeta(const std::string& ruta) {
    return gestor->eliminarCarpeta(ruta);
}

bool FileManager::crearArchivo(const std::string& ruta, const std::string& contenido) {
    return gestor->crearArchivo(ruta, contenido);
}

bool FileManager::copiarCarpeta(const std::string& origen, const std::string& destino) {
    return gestor->copiarCarpeta(origen, destino);
}

bool FileManager::copiarArchivo(const std::string& origen, const std::string& destino) {
    return gestor->copiarArchivo(origen, destino);
}

bool FileManager::renombrar(const std::string& ruta, const std::string& nuevoNombre) {
    return gestor->renombrar(ruta, nuevoNombre);
}

Carpeta* FileManager::buscarCarpetaPorRuta(const std::string& ruta) {
    ArbolEnlazado<File*>* arbol = gestor->getTreeFilePath();
    if (!arbol || arbol->isEmpty()) return nullptr;
    return buscarPreOrden(arbol, arbol->rootOfTree(), ruta);
}