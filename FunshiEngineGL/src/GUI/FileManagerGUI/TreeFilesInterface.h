#ifndef TREEFILESINTERFACE_H
#define TREEFILESINTERFACE_H
#include <string>
#include "../GeneralUserInterface.h"
#include "../../Herramientas/TreeGUI/TreeGUI.h"
#include "../../GestorDeArchivos/GestorDeArchivos.h"

class File;
class Carpeta;
class GestorDeArchivos;
class IconosGUI;

// Explorador de archivos ("BrowseFile"). El recorrido del arbol usa el widget
// generico TreeIG::drawTree (compartido con la jerarquia de la escena); aqui
// solo vive la logica de dominio: seleccion de carpeta y su menu contextual.
class TreeFilesInterface : public GeneralUserInterface {
protected:
    bool actualizar = false;
    Carpeta* thisFolderContent = nullptr;
    std::string pathProyect;
    GestorDeArchivos* gestorDeArchivos = nullptr;
    ArbolEnlazado<File*>* arbolDeArchivos = nullptr;
    Carpeta* lastSelectedFolder = nullptr;
    IconosGUI* iconosGUI = nullptr;
    TreeIG::OpenState openNodes;
    // Borrado diferido: se aplica tras el recorrido para no invalidar
    // iteradores (mismo patron que en la jerarquia de escena).
    Carpeta* carpetaAEliminar = nullptr;

public:
    TreeFilesInterface(bool stateGUI, const std::string& pathProyect);
    ~TreeFilesInterface();

    TreeFilesInterface(const TreeFilesInterface&) = delete;
    TreeFilesInterface& operator=(const TreeFilesInterface&) = delete;

    void setIconosGUI(IconosGUI* iconosG);

    virtual void initGUI() override;
    virtual void contentGUI() override;
    virtual void endGUI() override;
    virtual void printGUI() override;

    Carpeta* getFolderContent();
    void setFolderContent(Carpeta* folder);

private:
    // Reconstruye el arbol desde disco y descarta seleccion/colapso previos.
    void refrescarArbol();
    TreeIG::RowResult drawFolderRow(File* element, bool wasOpen);
};
#endif