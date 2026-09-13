#ifndef CONTENTFOLDERINTERFACE_H
#define CONTENTFOLDERINTERFACE_H
#include "../GeneralUserInterface.h"
#include "../../GestorDeArchivos/Carpeta.h"
#include <string>

class IconosGUI;

// Panel de contenido de la carpeta seleccionada en el arbol de archivos.
// La logica de archivos (copiar, seleccionar con zenity/GetOpenFileNameA,
// recorrer la rejilla) vive en ContentFolderInterface.cpp.
class ContentFolderInterface : public GeneralUserInterface {
private:
    Carpeta* thisFolderContent = nullptr;
    bool modificado = false;
    bool abrirPopupNombre = false;
    bool creandoCarpeta = false;
    bool creandoScript = false;
    char nombreNuevo[128] = "";

    IconosGUI* iconosGUI = nullptr;

    bool copiarArchivo(const std::string& origen, const std::string& destino);
    std::string seleccionarCarpetaSistema();
    std::string seleccionarArchivoSistema();

public:
    ContentFolderInterface(bool stateGUI);

    void setFolderRoot(Carpeta* folder);
    void setIconosGUI(IconosGUI* iconosG);
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