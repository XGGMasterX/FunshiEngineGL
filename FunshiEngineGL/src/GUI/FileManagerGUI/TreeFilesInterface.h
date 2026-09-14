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
    Carpeta* carpetaAEliminar = nullptr;

    // --- NUEVO: Variable de navegación diferida. Es una RUTA (no un puntero)
    // para que sobreviva a un refresco del árbol sin quedar colgando (B6). ---
    std::string pendingFolderPath;

public:
    TreeFilesInterface(bool stateGUI, const std::string& pathProyect);
    ~TreeFilesInterface();
    TreeFilesInterface(const TreeFilesInterface&) = delete;
    TreeFilesInterface& operator=(const TreeFilesInterface&) = delete;

    void setIconosGUI(IconosGUI* iconosG);
    // Pide que el árbol se rescancee; lo usa el panel de contenido tras
    // crear carpetas en disco (B2).
    void solicitarActualizacion();

    // --- NUEVO: Solo guarda la ruta de la carpeta a abrir, no aplica nada ---
    void requestOpenFolder(Carpeta* parent, const std::string& childName);

    virtual void initGUI() override;
    virtual void contentGUI() override;
    virtual void endGUI() override;
    virtual void printGUI() override;

    Carpeta* getFolderContent();
    void setFolderContent(Carpeta* folder);

private:
    void refrescarArbol();
    TreeIG::RowResult drawFolderRow(File* element, bool wasOpen);
};

#endif
