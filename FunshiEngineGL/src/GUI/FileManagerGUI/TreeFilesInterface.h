#ifndef TREEFILESINTERFACE_H
#define TREEFILESINTERFACE_H

#include <string>

#include "../GeneralUserInterface.h"
#include "../../Herramientas/TreeGUI/TreeGUI.h"
#include "../WindowNames.h"

class File;
class Carpeta;
class FileManager;
class IconosGUI;

// Panel "BrowseFile": dibuja el arbol de carpetas del proyecto y es la unica
// vista que actualiza la seleccion compartida (FileSelection). Ya no posee
// modelo: conversa con la fachada FileManager (R1) y refleja los cambios del
// Filesystem comparando su ultimo contador con el de la seleccion (R3).
class TreeFilesInterface : public GeneralUserInterface {
protected:
    bool actualizar = false;
    FileManager* fileManager = nullptr;
    ArbolEnlazado<File*>* arbolDeArchivos = nullptr;
    IconosGUI* iconosGUI = nullptr;
    TreeIG::OpenState openNodes;
    // Eliminacion recursiva diferida: el menu contextual solo la encola y el
    // borrado del nodo se aplica al final del frame (fuera del recorrido), para
    // no invalidar iteradores (B4).
    Carpeta* carpetaAEliminar = nullptr;
    // Confirmacion modal de "Eliminar Carpeta" (R7).
    bool confirmarEliminar = false;
    Carpeta* carpetaAConfirmar = nullptr;
    // Ultimo contador de cambios que este panel ya rescaneco.
    unsigned long ultimoContadorVisto = 0;

public:
    TreeFilesInterface(bool stateGUI, FileManager* fileManager);
    TreeFilesInterface(const TreeFilesInterface&) = delete;
    TreeFilesInterface& operator=(const TreeFilesInterface&) = delete;

    void setIconosGUI(IconosGUI* iconosG);
    void solicitarActualizacion();

    virtual void initGUI() override;
    virtual void contentGUI() override;
    virtual void endGUI() override;
    virtual void printGUI() override;

private:
    void refrescarArbol();
    void aplicarNavegacionPendiente();
    TreeIG::RowResult drawFolderRow(File* element, bool wasOpen);
};

#endif