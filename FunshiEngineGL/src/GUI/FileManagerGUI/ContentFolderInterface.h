#ifndef CONTENTFOLDERINTERFACE_H
#define CONTENTFOLDERINTERFACE_H

#include <string>

#include "../GeneralUserInterface.h"

class FileManager;
class IconosGUI;

// Panel "ShowFolder": muestra el contenido de la carpeta seleccionada en el
// arbol. Ya no se enlaza al arbol por puntero ni le pide el contenido: lee
// cada frame la seleccion compartida (FileSelection), se muestra a si mismo
// cuando hay carpeta (y se oculta si no) y notifica su navegacion por doble
// clic dejando la ruta pendiente en la seleccion (R3). Las operaciones de
// Filesystem van a la fachada FileManager, nunca a system(). (R1)
class ContentFolderInterface : public GeneralUserInterface {
private:
    FileManager* fileManager = nullptr;
    bool abrirPopupNombre = false;
    bool creandoCarpeta = false;
    bool creandoScript = false;
    char nombreNuevo[128] = "";
    IconosGUI* iconosGUI = nullptr;

    std::string seleccionarCarpetaSistema();
    std::string seleccionarArchivoSistema();
    void crearNuevoElemento();
    void recorrer(const std::string& path);

public:
    ContentFolderInterface(bool stateGUI, FileManager* fileManager);

    void setIconosGUI(IconosGUI* iconosG);

    virtual void initGUI() override;
    virtual void contentGUI() override;
    virtual void endGUI() override;
    virtual void printGUI() override;
};

#endif