#include "ContentFolderInterface.h"

#include "../../Herramientas/PathUtils.h"
#include "../../FileManager/FileManager.h"
#include "../../FileManager/FileSelection.h"
#include "../WindowNames.h"
#include "../../Herramientas/IconosGUI/IconosGUI.h"
#include <imgui.h>
#include <cstring>
#include <algorithm>
#include <filesystem>

#if defined(_WIN32)
#include <windows.h>
#include <shlobj.h>
#elif defined(__linux__)
#include <cstdio>
#endif

ContentFolderInterface::ContentFolderInterface(bool stateGUI, FileManager* fileManager)
    : GeneralUserInterface(WindowNames::ShowFolder, stateGUI, ImGuiWindowFlags_MenuBar),
      fileManager(fileManager) {}

void ContentFolderInterface::setIconosGUI(IconosGUI* iconosG) { iconosGUI = iconosG; }

std::string ContentFolderInterface::seleccionarCarpetaSistema() {
#if defined(_WIN32)
    BROWSEINFOA bi = { 0 };
    bi.lpszTitle = "Selecciona una carpeta para copiar";
    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (pidl != 0) {
        char path[MAX_PATH];
        if (SHGetPathFromIDListA(pidl, path)) return std::string(path);
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
    if (GetOpenFileNameA(&ofn) == TRUE) return std::string(szFile);
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

void ContentFolderInterface::crearNuevoElemento() {
    FileSelection* sel = fileManager->getSelection();
    if (!sel->carpetaActual || nombreNuevo[0] == '\0') return;

    const std::string destFolder =
        sel->carpetaActual->getPathRoot() + PATH_SEP +
        sel->carpetaActual->getPathName();

    if (creandoCarpeta) {
        const std::string ruta = destFolder + PATH_SEP + nombreNuevo;
        if (fileManager->crearCarpeta(ruta)) {
            // El arbol de carpetas cambia: se rescanceara al detectar el
            // contador (R3). La carpeta visible se conserva por rutaVisible.
            sel->contadorCambios++;
        }
    } else {
        std::string nombre = nombreNuevo;
        if (creandoScript && nombre.find(".cpp") == std::string::npos)
            nombre += ".cpp";

        std::string contenido;
        if (creandoScript) {
            // Script componente usa la convencion <ClassName>.cpp.
            std::string clase = nombre;
            const size_t dot = clase.find_last_of('.');
            if (dot != std::string::npos) clase = clase.substr(0, dot);
            contenido =
                "#include \"../../Behaviour/IScriptBehaviour.h\"\n"
                "\n"
                "class " + clase + " : public IScriptBehaviour {\n"
                "public:\n"
                "    void onStart(GameObject* owner) override {}\n"
                "    void onUpdate(GameObject* owner, float deltaTime) override {}\n"
                "};\n";
        }
        const std::string ruta = destFolder + PATH_SEP + nombre;
        // No sube el contador: los archivos no aparecen en el arbol de
        // carpetas y no merece colapsar la navegacion por un rescaneo.
        fileManager->crearArchivo(ruta, contenido);
    }

    creandoCarpeta = false;
    creandoScript = false;
    memset(nombreNuevo, 0, sizeof(nombreNuevo));
}

void ContentFolderInterface::recorrer(const std::string& path) {
    FileSelection* sel = fileManager->getSelection();

    std::error_code ec;
    std::filesystem::directory_iterator dirIt(path, ec);
    if (ec) return;
    const std::filesystem::directory_iterator fin;

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

    for (; dirIt != fin;) {
        const std::filesystem::directory_entry entrada = *dirIt;
        dirIt.increment(ec);
        if (ec) { ec.clear(); continue; } // entrada con errores: la saltamos

        const std::string nombre = entrada.path().filename().string();
        if (nombre == "." || nombre == "..") continue;
        const std::string fullPath = entrada.path().string();
        // No seguir symlinks: pueden apuntar a carpetas del sistema.
        if (std::filesystem::is_symlink(entrada.symlink_status())) continue;
        const bool esCarpeta = entrada.is_directory();

        size_t dot = nombre.find_last_of('.');
        std::string extension = "";
        if (!esCarpeta && dot != std::string::npos && dot != 0)
            extension = nombre.substr(dot);

        std::string uniqueID = "##" + fullPath;

        int fila = indice / columnas;
        int col = indice % columnas;
        ImGui::SetCursorPosY(yInicio + fila * altoCelda);
        ImGui::SetCursorPosX(xBase + col * stepX);

        ImGui::BeginGroup();
        ImGui::PushID(uniqueID.c_str());

        ImTextureID icono = ImTextureID_Invalid;
        if (iconosGUI) {
            icono = esCarpeta ? iconosGUI->getIconoCarpeta() : iconosGUI->getIconoPorExtension(extension);
        }

        if (icono != ImTextureID_Invalid) {
            ImVec4 baseButton = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
            ImGui::PushStyleColor(ImGuiCol_Button, baseButton);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(baseButton.x * 1.4f, baseButton.y * 1.4f, baseButton.z * 1.4f, baseButton.w));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(baseButton.x * 1.8f, baseButton.y * 1.8f, baseButton.z * 1.8f, baseButton.w));

            // Ignoramos el retorno de ImageButton (clic simple)
            ImGui::ImageButton(uniqueID.c_str(), icono, ImVec2(iconSize, iconSize), ImVec2(0, 0), ImVec2(1, 1), ImVec4(0, 0, 0, 0));

            ImGui::PopStyleColor(3);
        } else {
            const char* icon = esCarpeta ? "F" : "A";
            ImGui::Button(icon, ImVec2(iconSize, iconSize));
        }

        // --- DETECCION DE DOBLE CLIC (solo sobre el item bajo el cursor) ---
        // IsMouseDoubleClicked es un estado global por-frame: sin el check de
        // IsItemHovered, en el frame del doble clic reaccionaban TODAS las
        // celdas dibujadas (abriendo apps de varios archivos o navegando al
        // ultimo folder procesado, no al que estaba bajo el cursor).
        const bool isDoubleClicked =
            ImGui::IsItemHovered() &&
            ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);

        if (isDoubleClicked) {
            if (esCarpeta && sel->carpetaActual) {
                // FASE 1: solo se registra la ruta a abrir; el arbol la
                // aplica al inicio de su contentGUI contra el arbol vigente.
                sel->navegacionPendiente =
                    sel->carpetaActual->getPathRoot() + PATH_SEP +
                    sel->carpetaActual->getPathName() + PATH_SEP + nombre;
            } else if (!esCarpeta) {
#if defined(_WIN32)
                ShellExecuteA(NULL, "open", fullPath.c_str(), NULL, NULL, SW_SHOW);
#elif defined(__linux__)
                system(("xdg-open \"" + fullPath + "\" &").c_str());
#endif
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
            ImGui::SetDragDropPayload("ARCHIVO_PATH", fullPath.c_str(), fullPath.size() + 1);
            ImGui::Text("Soltar en zona de destino");
            ImGui::Text("%s", nombre.c_str());
            ImGui::EndDragDropSource();
        }

        std::string nombreMostrado = (dot != std::string::npos && dot != 0) ? nombre.substr(0, dot) : nombre;
        bool truncado = false;
        if (ImGui::CalcTextSize(nombreMostrado.c_str()).x > iconSize) {
            while (!nombreMostrado.empty() && ImGui::CalcTextSize((nombreMostrado + "...").c_str()).x > iconSize)
                nombreMostrado.pop_back();
            nombreMostrado += "...";
            truncado = true;
        }

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, (iconSize - ImGui::CalcTextSize(nombreMostrado.c_str()).x) * 0.5f));
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
    }
}

void ContentFolderInterface::initGUI() {
    FileSelection* sel = fileManager->getSelection();
    ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());

    if (ImGui::BeginPopupContextWindow("AddFilesPopup", ImGuiPopupFlags_MouseButtonRight)) {
        if (ImGui::MenuItem("New Script")) {
            creandoCarpeta = false; creandoScript = true;
            memset(nombreNuevo, 0, sizeof(nombreNuevo));
            abrirPopupNombre = true;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("New Folder")) {
            creandoCarpeta = true; creandoScript = false;
            memset(nombreNuevo, 0, sizeof(nombreNuevo));
            abrirPopupNombre = true;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("New File")) {
            creandoCarpeta = false; creandoScript = false;
            memset(nombreNuevo, 0, sizeof(nombreNuevo));
            abrirPopupNombre = true;
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::MenuItem("Copy Exist Folder") && sel->carpetaActual) {
            std::string sourceFolder = seleccionarCarpetaSistema();
            if (!sourceFolder.empty()) {
                const std::string destFolder =
                    sel->carpetaActual->getPathRoot() + PATH_SEP +
                    sel->carpetaActual->getPathName();
                size_t pos = sourceFolder.find_last_of("/\\");
                std::string folderName = (pos != std::string::npos) ? sourceFolder.substr(pos + 1) : sourceFolder;
                const std::string finalDest = destFolder + PATH_SEP + folderName;
                if (fileManager->copiarCarpeta(sourceFolder, finalDest)) {
                    // El arbol se rescancea porque aparecen carpetas nuevas.
                    sel->contadorCambios++;
                }
            }
        }
        if (ImGui::MenuItem("Copy Exist File") && sel->carpetaActual) {
            std::string sourceFile = seleccionarArchivoSistema();
            if (!sourceFile.empty()) {
                const std::string destFolder =
                    sel->carpetaActual->getPathRoot() + PATH_SEP +
                    sel->carpetaActual->getPathName();
                size_t pos = sourceFile.find_last_of("/\\");
                std::string fileName = (pos != std::string::npos) ? sourceFile.substr(pos + 1) : sourceFile;
                const std::string finalDest = destFolder + PATH_SEP + fileName;
                // Sin contador: copiar un archivo no modifica el arbol.
                fileManager->copiarArchivo(sourceFile, finalDest);
            }
        }
        ImGui::EndPopup();
    }

    if (abrirPopupNombre) {
        ImGui::OpenPopup("Ingresar nombre");
        abrirPopupNombre = false;
    }

    if (ImGui::BeginPopupModal("Ingresar nombre", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Escribe el nombre del %s:",
                    creandoCarpeta ? "folder" : (creandoScript ? "script" : "file"));
        ImGui::InputText("##nombreNuevo", nombreNuevo, IM_ARRAYSIZE(nombreNuevo));
        const bool confirmado = ImGui::Button("Crear", ImVec2(120, 0)) ||
                                (ImGui::IsItemFocused() &&
                                 ImGui::IsKeyPressed(ImGuiKey_Enter));
        if (confirmado) {
            crearNuevoElemento();
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
    FileSelection* sel = fileManager->getSelection();
    if (!sel->carpetaActual) return;
    const std::string destFolder =
        sel->carpetaActual->getPathRoot() + PATH_SEP +
        sel->carpetaActual->getPathName();
    recorrer(destFolder);
}

void ContentFolderInterface::endGUI() { ImGui::End(); }

void ContentFolderInterface::printGUI() {
    FileSelection* sel = fileManager->getSelection();
    const bool hayCarpeta = sel && sel->carpetaActual != nullptr;
    // R3: el panel se gobierna a si mismo. Se dibuja si esta abierto o si hay
    // seleccion; se oculta automaticamente cuando no hay carpeta (aunque
    // stateGUI quede en true, sin Begin() la ventana no se muestra).
    if (stateGUI || hayCarpeta) {
        stateGUI = true;
        initGUI();
        contentGUI();
        endGUI();
    }
}