#ifndef WINDOWNAMES_H
#define WINDOWNAMES_H

// Nombres de ventana ImGui del editor. Se centralizan para que el dock
// (DockSpaceInterface) y los paneles compartan los mismos literales. Antes la
// ventana del contenido era "Show Folder " (con espacio) escrita a mano en
// dos sitios: renombrarla en uno rompia el anclaje del dock en silencio.
namespace WindowNames {
inline constexpr const char* BrowseFile     = "BrowseFile";
inline constexpr const char* ShowFolder     = "ShowFolder";
inline constexpr const char* SelectedObjects = "SelectedObjects";
inline constexpr const char* Settings       = "Settings";
inline constexpr const char* MenuBar        = "MenuBar";
inline constexpr const char* EditorDockSpace = "EditorDockSpace";
} // namespace WindowNames

#endif