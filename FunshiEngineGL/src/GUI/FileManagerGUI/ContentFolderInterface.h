#ifndef CONTENTFOLDERINTERFACE_H
#define CONTENTFOLDERINTERFACE_H
#include "../../GestorDeArchivos/Carpeta.h"
#include <string>
#include <fstream>     // Para crear archivos
#include <iostream>    // std::cerr, std::cout

#if defined(_WIN32)
#include <windows.h>   // WinAPI base
#include <shlobj.h>    // SHBrowseForFolderA, SHGetPathFromIDListA
#elif defined(__linux__)
#include <cstdio>      // popen, pclose
#include <sys/stat.h>  // mkdir
#endif

class ContentFolderInterface : public GeneralUserInterface {
private:
    Carpeta* thisFolderContent;
    bool modificado = false;
    bool abrirPopupNombre = false;
    bool creandoCarpeta = false; // true = carpeta, false = archivo
    bool creandoScript = false;
    char nombreNuevo[128] = "";

    bool copiarArchivo(const std::string& origen, const std::string& destino) {
        if (origen.empty() || destino.empty()) return false;

#if defined(_WIN32)
        // En Windows usamos CopyFileA, devuelve TRUE si copia ok
        return CopyFileA(origen.c_str(), destino.c_str(), FALSE) == TRUE;

#elif defined(__linux__)
        // En Linux usamos flujo para copiar manualmente (más portable que system("cp"))
        std::ifstream src(origen, std::ios::binary);
        std::ofstream dst(destino, std::ios::binary);
        if (!src || !dst) return false;

        dst << src.rdbuf();
        return src && dst;
#else
        return false;
#endif
    }


    std::string seleccionarCarpetaSistema() {
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
                path.erase(path.find_last_not_of("\n\r") + 1); // quitar salto de línea
                pclose(fp);
                return path;
            }
            pclose(fp);
        }
        return "";
#endif
    }

    std::string seleccionarArchivoSistema() {
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

public:
	ContentFolderInterface(bool stateGUI) :
		GeneralUserInterface("Show Folder ", stateGUI, ImGuiWindowFlags_MenuBar) {
		this->stateGUI = stateGUI;
	}

    void setFolderRoot(Carpeta* thisFolderContent) {
        this->thisFolderContent = thisFolderContent;
    }

    bool getModificado(){
        return modificado;
    }

    void setModificado(bool modificado) {
        this->modificado = modificado;
    }

    void recorrer(const std::string& path) {
        std::string searchPath = path + "\\*";

        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

        if (hFind == INVALID_HANDLE_VALUE) {
            std::cerr << "No se pudo abrir: " << path << '\n';
            return;
        }

        const float iconSize = 64.0f;
        const float spacing = 16.0f;
        const float fullSize = iconSize + spacing;

        do {
            const char* nombre = findData.cFileName;
            if (strcmp(nombre, ".") == 0 || strcmp(nombre, "..") == 0) continue;

            std::string fullPath = path + "\\" + nombre;
            bool esCarpeta = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

            std::string nombreStr = nombre;
            std::string extension = "";
            size_t dot = nombreStr.find_last_of('.');
            if (!esCarpeta && dot != std::string::npos && dot != 0) {
                extension = nombreStr.substr(dot);
            }

            std::string uniqueID = std::string("##") + fullPath;

            ImGui::BeginGroup();
            ImGui::PushID(uniqueID.c_str());

            // 1. Botón/Ícono (Inicio del Drag Source)
            const char* icon = esCarpeta ? "F" : "A";
            if (ImGui::Button(icon, ImVec2(iconSize, iconSize))) {
#if defined(_WIN32)
                ShellExecuteA(NULL, "open", fullPath.c_str(), NULL, NULL, SW_SHOW);
#elif defined(__linux__)
                system(("xdg-open \"" + fullPath + "\" &").c_str());
#endif
            }

            // 2. Configurar Drag and Drop
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload("ARCHIVO_PATH", fullPath.c_str(), fullPath.size() + 1);

                // Vista previa durante el drag
                ImGui::Text("Soltar en zona de destino");
                ImGui::Text("%s", nombre);
                ImGui::EndDragDropSource();
            }

            // 3. Texto debajo del ícono (manteniendo tu formato original)
            if (!esCarpeta && dot != std::string::npos && dot != 0) {
                std::string base = nombreStr.substr(0, dot);
                std::string ext = nombreStr.substr(dot);
                ImGui::TextWrapped("%s", base.c_str());
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(150, 150, 150, 255));
                ImGui::TextWrapped("%s", ext.c_str());
                ImGui::PopStyleColor();
            }
            else {
                ImGui::TextWrapped("%s", nombreStr.c_str());
            }

            ImGui::PopID();
            ImGui::EndGroup();

            // Ajuste de layout (igual que tu versión original)
            float cursorX = ImGui::GetCursorPosX();
            float availX = ImGui::GetContentRegionAvail().x;
            if (cursorX + fullSize < availX)
                ImGui::SameLine();

        } while (FindNextFileA(hFind, &findData) != 0);

        FindClose(hFind);
    }

    void setTreeFilePath(const std::string& path) {
        recorrer(path);
    }

    Carpeta* getFolderContent() {
		return thisFolderContent;
	}

	virtual void initGUI() override {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());

        if (ImGui::BeginPopupContextWindow("AddFilesPopup", ImGuiPopupFlags_MouseButtonRight)) {
            if (ImGui::MenuItem("New Script")) {
                creandoCarpeta = false;
                creandoScript = true;
                strcpy_s(nombreNuevo, sizeof(nombreNuevo), "");
                nombreNuevo[sizeof(nombreNuevo) - 1] = '\0';
                abrirPopupNombre = true;
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("New Folder")) {
                creandoCarpeta = true;
                creandoScript = false;
                strcpy_s(nombreNuevo, sizeof(nombreNuevo), "");
                nombreNuevo[sizeof(nombreNuevo) - 1] = '\0';
                abrirPopupNombre = true;
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("New File")) {
                creandoCarpeta = false;
                creandoScript = false;
                strcpy_s(nombreNuevo, sizeof(nombreNuevo), "");
                nombreNuevo[sizeof(nombreNuevo) - 1] = '\0';
                abrirPopupNombre = true;
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::MenuItem("Copy Exist Folder")) {
                std::string sourceFolder = seleccionarCarpetaSistema();
                if (!sourceFolder.empty()) {
                    // Carpeta destino base: la ruta completa de tu folder actual (incluye raíz + nombre)
                    std::string destFolder = thisFolderContent->getPathRoot() + "/" + thisFolderContent->getPathName();

                    // Extraer solo el nombre final de la carpeta seleccionada
                    size_t pos = sourceFolder.find_last_of("/\\");
                    std::string folderName = (pos != std::string::npos)
                        ? sourceFolder.substr(pos + 1)
                        : sourceFolder;

                    // Ruta completa destino donde se copiará la carpeta (dentro de tu carpeta actual)
                    std::string finalDest = destFolder + "/" + folderName;

#if defined(_WIN32)
                    // Crear carpeta destino si no existe
                    CreateDirectoryA(finalDest.c_str(), NULL);

                    // Copiar la carpeta seleccionada al destino
                    std::string command = "xcopy \"" + sourceFolder + "\" \"" + finalDest + "\\\" /E /I /Y";
                    system(command.c_str());
#elif defined(__linux__)
                    // Crear carpeta destino y padres si no existen
                    std::string commandCreate = "mkdir -p \"" + finalDest + "\"";
                    system(commandCreate.c_str());

                    // Copiar contenido de la carpeta seleccionada dentro del destino (no crear subcarpeta extra)
                    std::string commandCopy = "cp -r \"" + sourceFolder + "\"/* \"" + finalDest + "\"";
                    system(commandCopy.c_str());
#endif
                    setModificado(true);
                }
            }

            if (ImGui::MenuItem("Copy Exist File")) {
                std::string sourceFile = seleccionarArchivoSistema();
                if (!sourceFile.empty()) {
                    std::string destFolder = thisFolderContent->getPathRoot() + "/" + thisFolderContent->getPathName();

                    size_t pos = sourceFile.find_last_of("/\\");
                    std::string fileName = (pos != std::string::npos)
                        ? sourceFile.substr(pos + 1)
                        : sourceFile;

                    std::string finalDest = destFolder + "/" + fileName;

                    if (copiarArchivo(sourceFile, finalDest)) {
                        setModificado(true);
                    }
                    else {
                        std::cerr << "Error copiando archivo\n";
                    }
                }
            }
            //ACA PUEDO AGREGAR MAS OPCIONES A LA LISTA

            ImGui::EndPopup();
        }

        if (abrirPopupNombre) {
            ImGui::OpenPopup("Ingresar nombre");
            abrirPopupNombre = false;
        }

        if (ImGui::BeginPopupModal("Ingresar nombre", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Escribe el nombre del %s:", creandoCarpeta ? "folder" : "file");
            ImGui::InputText("##nombreNuevo", nombreNuevo, IM_ARRAYSIZE(nombreNuevo));

            if (ImGui::Button("Crear", ImVec2(120, 0))) {
                std::string currentPath = thisFolderContent->getPathRoot() + "/" + thisFolderContent->getPathName();
                std::string fullPath = currentPath + "/" + nombreNuevo;

#if defined(_WIN32)
                if (creandoCarpeta) {
                    CreateDirectoryA(fullPath.c_str(), NULL);
                }
                else if (creandoScript) {
                    std::ofstream filecpp(fullPath +".cpp");
                    filecpp << "#include \"" << nombreNuevo << ".hpp\"\n\n"
                        << "void " << nombreNuevo << "::onStart(GameObject* owner) {\n"
                        << "    std::cout << \"" << nombreNuevo << " iniciado en: \" << owner->getName() << \"\\n\";\n"
                        << "}\n\n"
                        << "void " << nombreNuevo << "::onUpdate(GameObject* owner, float deltaTime) {\n"
                        << "    // TODO: lógica por frame aquí\n"
                        << "}\n\n"
                        << "extern \"C\" __declspec(dllexport) IScriptBehaviour* CreateScript() {\n"
                        << "    return new " << nombreNuevo << "();\n"
                        << "}\n";
                    filecpp.close();
                    std::ofstream filehpp(fullPath + ".hpp");
                    filehpp << "#pragma once\n"
                        << "#include \"IScriptBehaviour.h\"\n"
                        << "#include <iostream>\n\n"
                        << "class " << nombreNuevo << " : public IScriptBehaviour {\n"
                        << "public:\n"
                        << "    void onStart(GameObject* owner) override;\n"
                        << "    void onUpdate(GameObject* owner, float deltaTime) override;\n"
                        << "};\n";
                    filehpp.close();
                }
                else {
                    std::ofstream file(fullPath);
                    file.close();
                }
#elif defined(__linux__)
                if (creandoCarpeta) {
                    mkdir(fullPath.c_str(), 0777);
                }
                else {
                    std::ofstream file(fullPath);
                    file.close();
                }
#endif
                setModificado(true);
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Cancelar", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
	}

	virtual void contentGUI() override {
        setTreeFilePath(thisFolderContent->getPathRoot() + "/" + thisFolderContent->getPathName());
	}
     

    virtual void endGUI() override {
        ImGui::End();
    }

	virtual void printGUI() override {
		if (stateGUI) {
			initGUI();
			contentGUI();
			endGUI();
		}
	}
};
#endif
