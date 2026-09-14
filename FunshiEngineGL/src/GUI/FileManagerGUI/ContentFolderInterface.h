#ifndef CONTENTFOLDERINTERFACE_H
#define CONTENTFOLDERINTERFACE_H

#include "../GeneralUserInterface.h"
#include "../../GestorDeArchivos/Carpeta.h"
#include <string>

class IconosGUI;
class TreeFilesInterface; // Forward declaration

class ContentFolderInterface : public GeneralUserInterface {
private:
    Carpeta* thisFolderContent = nullptr;
    bool modificado = false;
    bool abrirPopupNombre = false;
    bool creandoCarpeta = false;
    bool creandoScript = false;
    char nombreNuevo[128] = "";
    IconosGUI* iconosGUI = nullptr;

    TreeFilesInterface* treeFilesInterface = nullptr; // Puntero para notificar doble clic

    bool copiarArchivo(const std::string& origen, const std::string& destino);
    std::string seleccionarCarpetaSistema();
    std::string seleccionarArchivoSistema();
    bool crearCarpetaEnSistema(const std::string& path);
    bool crearArchivoEnSistema(const std::string& path, const std::string& contenido);
    void crearNuevoElemento();

public:
    ContentFolderInterface(bool stateGUI);

    void setFolderRoot(Carpeta* folder);
    void setIconosGUI(IconosGUI* iconosG);
    void setTreeFilesInterface(TreeFilesInterface* tree); // Nuevo setter

    bool getModificado();
    void setModificado(bool mod);

    void recorrer(const std::string& path);
    void setTreeFilePath(const std::string& path);
    Carpeta* getFolderContent();

    virtual void initGUI() override;
    virtual void contentGUI() override;
    virtual void endGUI() override;
    virtual void printGUI() override;
};

#endif
