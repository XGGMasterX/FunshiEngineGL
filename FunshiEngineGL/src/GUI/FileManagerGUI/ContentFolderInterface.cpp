#include "ContentFolderInterface.h"

#include "../../Herramientas/IconosGUI/IconosGUI.h"
#include <imgui.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <algorithm>

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#elif defined(__linux__)
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>
#include <cerrno>
#endif

ContentFolderInterface::ContentFolderInterface(bool stateGUI)
    : GeneralUserInterface("Show Folder ", stateGUI,
                           ImGuiWindowFlags_MenuBar) {
    this->stateGUI = stateGUI;
}

void ContentFolderInterface::setFolderRoot(Carpeta* folder) {
    thisFolderContent = folder;
}

void ContentFolderInterface::setIconosGUI(IconosGUI* iconosG) {
    iconosGUI = iconosG;
}

bool ContentFolderInterface::getModificado() { return modificado; }

void ContentFolderInterface::setModificado(bool mod) { modificado = mod; }

bool ContentFolderInterface::copiarArchivo(const std::string& origen,
                                           const std::string& destino) {
    if (origen.empty() || destino.empty()) return false;

#if defined(_WIN32)
    return CopyFileA(origen.c_str(), destino.c_str(), FALSE) == TRUE;

#elif defined(__linux__)
    std::ifstream src(origen, std::ios::binary);
    std::ofstream dst(destino, std::ios::binary);
    if (!src || !dst) return false;
    dst << src.rdbuf();
    return src && dst;
#else
    return false;
#endif
}

std::string ContentFolderInterface::seleccionarCarpetaSistema() {
#if defined(_WIN32)
    BROWSEINFOA bi = { 0 };
    bi.lpszTitle = "Selecciona una carpeta para copiar";
    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (pidl != 0) {
        char path[MAX_PATH];
        if (SHGetPathFromIDListA(pidl, path)) {
            return std::string(path);
        }
    }
    return "";
#elif defined(__linux__)
    char buffer[512];
    FILE* fp = popen("zenity --file-selection --directory 2>/dev/null", "r");
    if (fp) {
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            std::string path(buffer);
            path.erase(path.find_last_not_of("\n\r") + 1);
            pclose(fp);
            return path;
        }
        pclose(fp);
    }
    return "";
#endif
}

std::string ContentFolderInterface::seleccionarArchivoSistema() {
#if defined(_WIN32)
    OPENFILENAMEA ofn;
    CHAR szFile[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Todos los archivos\0*.*\0";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        return std::string(szFile);
    }
    return "";
#elif defined(__linux__)
    char buffer[512];
    FILE* fp = popen("zenity --file-selection 2>/dev/null", "r");
    if (fp) {
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            std::string path(buffer);
            path.erase(path.find_last_not_of("\n\r") + 1);
            pclose(fp);
            return path;
        }
        pclose(fp);
    }
    return "";
#endif
}

void ContentFolderInterface::recorrer(const std::string& path) {
#if defined(_WIN32)
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = FindFirstFileA((path + "\\*").c_str(), &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "No se pudo abrir: " << path << '\n';
        return;
    }
#elif defined(__linux__)
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        std::cerr << "No se pudo abrir: " << path << '\n';
        return;
    }
#endif

    // Rejilla de celdas FIJAS: el espacio entre iconos NO depende del largo del nombre.
    const float iconSize = 87.0f;
    const float spacing = 16.0f;
    const float stepX = iconSize + spacing;
    const float altoTexto = ImGui::GetTextLineHeightWithSpacing();
    const float altoCelda = iconSize + altoTexto;

    float xBase = ImGui::GetCursorPosX();
    float availX = ImGui::GetContentRegionAvail().x;
    int columnas = std::max(1, (int)((availX + spacing) / stepX));
    float yInicio = ImGui::GetCursorPosY();
    int indice = 0;

#if defined(_WIN32)
    do {
        std::string nombre = findFileData.cFileName;
        if (nombre == "." || nombre == "..") continue;

        std::string fullPath = path + "\\" + nombre;

        bool esCarpeta = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

#elif defined(__linux__)
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string nombre = entry->d_name;
        if (nombre == "." || nombre == "..") continue;

        std::string fullPath = path;
        if (!fullPath.empty() && fullPath.back() != '/')
            fullPath += "/";
        fullPath += nombre;

        struct stat info;
        if (stat(fullPath.c_str(), &info) != 0) {
            std::cerr << "No se pudo obtener info de: " << fullPath << " ("
                      << strerror(errno) << ")\n";
            continue;
        }

        bool esCarpeta = S_ISDIR(info.st_mode);
#endif

        size_t dot = nombre.find_last_of('.');
        std::string extension = "";
        if (!esCarpeta && dot != std::string::npos && dot != 0)
            extension = nombre.substr(dot);

        std::string uniqueID = "##" + fullPath;

        // Ubicacion de la celda en la grilla (independiente del ancho del texto).
        int fila = indice / columnas;
        int col = indice % columnas;
        ImGui::SetCursorPosY(yInicio + fila * altoCelda);
        ImGui::SetCursorPosX(xBase + col * stepX);

        ImGui::BeginGroup();
        ImGui::PushID(uniqueID.c_str());

        ImTextureID icono = ImTextureID_Invalid;
        if (iconosGUI) {
            icono = esCarpeta ? iconosGUI->getIconoCarpeta()
                              : iconosGUI->getIconoPorExtension(extension);
        }

        bool abierto = false;
        if (icono != ImTextureID_Invalid) {
            // El ImageButton pinta el frame con ImGuiCol_Button (azul del tema
            // por defecto). Lo alineamos al color del fondo de la ventana y
            // dejamos hover/pressed levemente mas claros para conservar feedback.
            ImVec4 baseButton = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
            ImGui::PushStyleColor(ImGuiCol_Button, baseButton);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                  ImVec4(baseButton.x * 1.4f, baseButton.y * 1.4f,
                                         baseButton.z * 1.4f, baseButton.w));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                                  ImVec4(baseButton.x * 1.8f, baseButton.y * 1.8f,
                                         baseButton.z * 1.8f, baseButton.w));
            abierto = ImGui::ImageButton(uniqueID.c_str(), icono,
                                         ImVec2(iconSize, iconSize), ImVec2(0, 0),
                                         ImVec2(1, 1), ImVec4(0, 0, 0, 0));
            ImGui::PopStyleColor(3);
        } else {
            const char* icon = esCarpeta ? "F" : "A";
            abierto = ImGui::Button(icon, ImVec2(iconSize, iconSize));
        }

        if (abierto) {
            if (esCarpeta) {
                if (thisFolderContent) {
                    thisFolderContent->setPathRoot(fullPath); // cambiar carpeta
                    setModificado(true);                      // forzar refresco
                }
            } else {
#if defined(_WIN32)
                ShellExecuteA(NULL, "open", fullPath.c_str(), NULL, NULL, SW_SHOW);
#elif defined(__linux__)
                system(("xdg-open \"" + fullPath + "\" &").c_str());
#endif
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ARCHIVO_PATH", fullPath.c_str(),
                                      fullPath.size() + 1);
            ImGui::Text("Soltar en zona de destino");
            ImGui::Text("%s", nombre.c_str());
            ImGui::EndDragDropSource();
        }

        // Etiqueta en UNA linea y truncada con "..." si no cabe en la celda.
        // Estandar de interfaces con poco espacio: nombre completo como tooltip al pasar el mouse.
        std::string nombreMostrado =
            (dot != std::string::npos && dot != 0) ? nombre.substr(0, dot) : nombre;
        bool truncado = false;
        if (ImGui::CalcTextSize(nombreMostrado.c_str()).x > iconSize) {
            while (!nombreMostrado.empty() &&
                   ImGui::CalcTextSize((nombreMostrado + "...").c_str()).x >
                       iconSize)
                nombreMostrado.pop_back();
            nombreMostrado += "...";
            truncado = true;
        }
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                             std::max(0.0f, (iconSize - ImGui::CalcTextSize(nombreMostrado.c_str()).x) * 0.5f));
        ImGui::Text("%s", nombreMostrado.c_str());
        if (truncado && ImGui::IsItemHovered()) {
            if (!esCarpeta && !extension.empty())
                ImGui::SetTooltip("%s\nTipo: %s", nombre.c_str(), extension.c_str());
            else
                ImGui::SetTooltip("%s", nombre.c_str());
        }

        ImGui::PopID();
        ImGui::EndGroup();
        indice++;

#if defined(_WIN32)
    } while (FindNextFileA(hFind, &findFileData) != 0);
    FindClose(hFind);
#elif defined(__linux__)
    }
    closedir(dir);
#endif
}

void ContentFolderInterface::setTreeFilePath(const std::string& path) {
    recorrer(path);
}

Carpeta* ContentFolderInterface::getFolderContent() {
    return thisFolderContent;
}

void ContentFolderInterface::initGUI() {
    ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());

    if (ImGui::BeginPopupContextWindow("AddFilesPopup",
                                       ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("New Script")) {
            creandoCarpeta = false;
            creandoScript = true;
            memset(nombreNuevo, 0, sizeof(nombreNuevo));
            abrirPopupNombre = true;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("New Folder")) {
            creandoCarpeta = true;
            creandoScript = false;
            memset(nombreNuevo, 0, sizeof(nombreNuevo));
            abrirPopupNombre = true;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("New File")) {
            creandoCarpeta = false;
            creandoScript = false;
            memset(nombreNuevo, 0, sizeof(nombreNuevo));
            abrirPopupNombre = true;
            ImGui::CloseCurrentPopup();
        }

        if (ImGui::MenuItem("Copy Exist Folder") && thisFolderContent) {
            std::string sourceFolder = seleccionarCarpetaSistema();
            if (!sourceFolder.empty()) {
                std::string destFolder =
                    thisFolderContent->getPathRoot() + "/" +
                    thisFolderContent->getPathName();
                size_t pos = sourceFolder.find_last_of("/\\");
                std::string folderName =
                    (pos != std::string::npos) ? sourceFolder.substr(pos + 1)
                                               : sourceFolder;
                std::string finalDest = destFolder + "/" + folderName;

#if defined(_WIN32)
                CreateDirectoryA(finalDest.c_str(), NULL);
                std::string command =
                    "xcopy \"" + sourceFolder + "\" \"" + finalDest +
                    "\\\" /E /I /Y";
                system(command.c_str());
#elif defined(__linux__)
                std::string commandCreate = "mkdir -p \"" + finalDest + "\"";
                system(commandCreate.c_str());
                std::string commandCopy =
                    "cp -r \"" + sourceFolder + "\"/* \"" + finalDest + "\"";
                system(commandCopy.c_str());
#endif
                setModificado(true);
            }
        }

        if (ImGui::MenuItem("Copy Exist File") && thisFolderContent) {
            std::string sourceFile = seleccionarArchivoSistema();
            if (!sourceFile.empty()) {
                std::string destFolder =
                    thisFolderContent->getPathRoot() + "/" +
                    thisFolderContent->getPathName();
                size_t pos = sourceFile.find_last_of("/\\");
                std::string fileName =
                    (pos != std::string::npos) ? sourceFile.substr(pos + 1)
                                               : sourceFile;
                std::string finalDest = destFolder + "/" + fileName;

                if (copiarArchivo(sourceFile, finalDest)) {
                    setModificado(true);
                } else {
                    std::cerr << "Error copiando archivo\n";
                }
            }
        }

        ImGui::EndPopup();
    }

    if (abrirPopupNombre) {
        ImGui::OpenPopup("Ingresar nombre");
        abrirPopupNombre = false;
    }

    if (ImGui::BeginPopupModal("Ingresar nombre", NULL,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Escribe el nombre del %s:",
                    creandoCarpeta ? "folder" : "file");
        ImGui::InputText("##nombreNuevo", nombreNuevo,
                         IM_ARRAYSIZE(nombreNuevo));

        if (ImGui::Button("Crear", ImVec2(120, 0))) {
            if (!thisFolderContent) {
                std::cerr << "[ContentFolderInterface] Cannot create file/folder: thisFolderContent is nullptr!\n";
            } else {
                std::string currentPath = thisFolderContent->getPathRoot() +
                                          "/" +
                                          thisFolderContent->getPathName();
                std::string fullPath = currentPath + "/" + nombreNuevo;
                // Codigo de creacion de carpeta, archivo o script (igual que antes)
                setModificado(true);
            }
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancelar", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void ContentFolderInterface::contentGUI() {
    if (!thisFolderContent) {
        std::cerr << "[ContentFolderInterface] thisFolderContent is nullptr!\n";
        return;
    }

    std::string destFolder = thisFolderContent->getPathRoot() + "/" +
                             thisFolderContent->getPathName();

    // Aqui es donde realmente recorres la carpeta para dibujarla
    recorrer(destFolder);
}

void ContentFolderInterface::endGUI() { ImGui::End(); }

void ContentFolderInterface::printGUI() {
    if (stateGUI) {
        initGUI();
        contentGUI();
        endGUI();
    }
}