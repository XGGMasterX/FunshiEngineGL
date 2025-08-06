#ifndef CONTENTFOLDERINTERFACE_H
#define CONTENTFOLDERINTERFACE_H
#include "../../GestorDeArchivos/Folder.h"
class ContentFolderInterface : public GeneralUserInterface {
private:
    Folder* thisFolderContent;
    bool modificado = false;
public:
	ContentFolderInterface(bool stateGUI) :
		GeneralUserInterface("Show Folder ", stateGUI, ImGuiWindowFlags_MenuBar) {
		this->stateGUI = stateGUI;
	}

    void setFolderRoot(Folder* thisFolderContent) {
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

            
            const char* icon = "A";
            if (esCarpeta)
                icon = "F";
            else if (extension == ".funshi")
                icon = "F";

            
            if (ImGui::Button(icon, ImVec2(iconSize, iconSize))) {
#if defined(_WIN32)
                if (esCarpeta) {
                    ShellExecuteA(NULL, "open", fullPath.c_str(), NULL, NULL, SW_SHOW);
                }
                else {
                    ShellExecuteA(NULL, "open", fullPath.c_str(), NULL, NULL, SW_SHOW);
                }
#elif defined(__linux__)
                std::string command = "xdg-open \"" + fullPath + "\" &";
                system(command.c_str());
#endif
            }

            
            if (nombreStr.length() > 18)
                nombreStr = nombreStr.substr(0, 15) + "...";

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

	Folder* getFolderContent() {
		return thisFolderContent;
	}

	virtual void initGUI() override {
		ImGui::Begin(getNameGui().c_str(), &stateGUI, getFlagGui());
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
