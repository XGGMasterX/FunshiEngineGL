/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
#include "IconosGUI.h"
#include <fstream>
#include <iostream>
#include <cctype>
#include <cstring>
#include <string>
#include <vector>

#include "../../GLCompat.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {

// Directorio del ejecutable (sin el nombre del binario, con separador final).
// Refleja donde quedo el build y NO depende del directorio desde el que se
// lance el engine: hasta ahora los iconos se resolvian 100% relativo al cwd y
// segun quien corriera el exe se veian las imagenes de una carpeta u otra.
std::string directorioEjecutable() {
#ifdef _WIN32
    char exe[MAX_PATH] = {};
    DWORD n = GetModuleFileNameA(nullptr, exe, MAX_PATH);
    if (n == 0 || n >= MAX_PATH) return "";
    const std::string path(exe, static_cast<std::size_t>(n));
    const std::size_t sep = path.find_last_of("\\/");
    return (sep == std::string::npos) ? "" : path.substr(0, sep + 1);
#else
    char link[4096] = {};
    const ssize_t n = readlink("/proc/self/exe", link, sizeof(link) - 1);
    if (n <= 0) return "";
    link[n] = '\0';
    const std::string path(link);
    const std::size_t sep = path.find_last_of('/');
    return (sep == std::string::npos) ? "" : path.substr(0, sep + 1);
#endif
}

} // namespace

IconosGUI::IconosGUI() {}

IconosGUI::~IconosGUI() {
    // Todos los iconos cargados se liberan igual; conviene recorrer el array
    // en vez de repetir el bloque glDeleteTextures para cada miembro.
    ImTextureID* iconos[] = {
        &iconoCarpeta, &iconoArchivo, &iconoCpp, &iconoHpp, &iconoJava,
        &iconoGameObject, &iconoBlend, &iconoCsv, &iconoExr, &iconoFbx,
        &iconoHdr, &iconoJpeg, &iconoJpg, &iconoJson, &iconoMax, &iconoMaya,
        &iconoMp3, &iconoObj, &iconoOgg, &iconoOtf, &iconoPng, &iconoPsd,
        &iconoRs, &iconoTga, &iconoTtf, &iconoWav, &iconoXml, &iconoDb,
        &iconoMtl, &iconoRar, &iconoZip,
    };
    for (ImTextureID* icono : iconos) {
        if (*icono != ImTextureID_Invalid) {
            GLuint id = (GLuint)(intptr_t)*icono;
            glDeleteTextures(1, &id);
            *icono = ImTextureID_Invalid;
        }
    }
}

void IconosGUI::init() {
    if (inicializado) return;
    iconoCarpeta = cargarPNG("folder.png");
    iconoArchivo = cargarPNG("file.png");
    iconoCpp = cargarPNG("cpp.png");
    iconoHpp = cargarPNG("hpp.png");
    iconoJava = cargarPNG("java.png");
    iconoGameObject = cargarPNG("cubo.png");
    // Iconos por extension de asset/formato (mismo nombre que el archivo en Imagenes/).
    iconoBlend = cargarPNG("blend.png");
    iconoCsv = cargarPNG("csv.png");
    iconoExr = cargarPNG("exr.png");
    iconoFbx = cargarPNG("fbx.png");
    iconoHdr = cargarPNG("hdr.png");
    iconoJpeg = cargarPNG("jpeg.png");
    iconoJpg = cargarPNG("jpg.png");
    iconoJson = cargarPNG("json.png");
    iconoMax = cargarPNG("max.png");
    iconoMaya = cargarPNG("maya.png");
    iconoMp3 = cargarPNG("mp3.png");
    iconoObj = cargarPNG("obj.png");
    iconoOgg = cargarPNG("ogg.png");
    iconoOtf = cargarPNG("otf.png");
    iconoPng = cargarPNG("png.png");
    iconoPsd = cargarPNG("psd.png");
    iconoRs = cargarPNG("rs.png");
    iconoTga = cargarPNG("tga.png");
    iconoTtf = cargarPNG("ttf.png");
    iconoWav = cargarPNG("wav.png");
    iconoXml = cargarPNG("xml.png");
    iconoDb = cargarPNG("db.png");
    iconoMtl = cargarPNG("mtl.png");
    iconoRar = cargarPNG("rar.png");
    iconoZip = cargarPNG("zip.png");
    inicializado = true;
}

ImTextureID IconosGUI::cargarPNG(const char* nombrePNG) {
    // Orden de busqueda:
    //  1) Relativo al directorio del ejecutable (determinista): los vectores
    //     de build activo suelen quedar junto al binario o un nivel arriba
    //     (build/ -> ../Imagenes, build-java/ -> ../FunshiEngineGL/Imagenes).
    //  2) Relativo al cwd (fallback historico): si el exe se corre desde
    //     build/, la raiz del proyecto o la raiz del proyecto del usuario.
    const std::string exeDir = directorioEjecutable();
    std::vector<std::string> carpetas;
    if (!exeDir.empty()) {
        carpetas.push_back(exeDir + "Imagenes/");
        carpetas.push_back(exeDir + "../Imagenes/");
        carpetas.push_back(exeDir + "../FunshiEngineGL/Imagenes/");
        carpetas.push_back(exeDir + "../../Imagenes/");
        carpetas.push_back(exeDir + "../../../Imagenes/");
    }
    const char* rutasCwd[] = {
        "Imagenes/", "../Imagenes/", "../../Imagenes/", "../../../Imagenes/"
    };
    for (const char* carpeta : rutasCwd) carpetas.emplace_back(carpeta);

    std::string rutaEncontrada;
    for (const std::string& carpeta : carpetas) {
        const std::string ruta = carpeta + nombrePNG;
        std::ifstream archivo(ruta.c_str(), std::ios::binary);
        if (archivo.good()) {
            rutaEncontrada = ruta;
            break;
        }
    }
    if (rutaEncontrada.empty()) {
        std::cerr << "[IconosGUI] No se encontro la imagen '" << nombrePNG
                  << "' (se probaron varias rutas relativas al cwd).\n";
        return ImTextureID_Invalid;
    }

    int ancho = 0, alto = 0, canales = 0;
    unsigned char* pixeles = stbi_load(rutaEncontrada.c_str(), &ancho, &alto, &canales, 4);
    if (!pixeles) {
        std::cerr << "[IconosGUI] stbi_load fallo en: " << rutaEncontrada << "\n";
        return ImTextureID_Invalid;
    }

    GLuint textura = 0;
    glGenTextures(1, &textura);
    glBindTexture(GL_TEXTURE_2D, textura);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Mipmaps (trilineales) para que la minificacion a tamanos chicos no
    // produzca "dientes"/alias (lo que pasaba con el muestreo lineal a secas).
    // gluBuild2DMipmaps genera la cadena completa en contextos compatibility
    // (glGenerateMipmap es de GL 3.0 y no esta en los headers legacy).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, ancho, alto, GL_RGBA, GL_UNSIGNED_BYTE, pixeles);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(pixeles);
    std::cout << "[IconosGUI] Textura cargada: " << rutaEncontrada
              << " (" << ancho << "x" << alto << ")\n";
    return (ImTextureID)(intptr_t)textura;
}

ImTextureID IconosGUI::getIconoPorExtension(const std::string& extension) const {
    std::string ext = extension;
    for (auto& c : ext) c = (char)tolower((unsigned char)c);

    if (ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".c") return iconoCpp;
    if (ext == ".h" || ext == ".hpp" || ext == ".hh" || ext == ".hxx") return iconoHpp;
    if (ext == ".java") return iconoJava;

    // Assets y formatos: modelos, imagenes, audio, fuentes y datos.
    if (ext == ".blend" || ext == ".blend1") return iconoBlend;      // Blender
    if (ext == ".max" || ext == ".3ds") return iconoMax;             // 3ds Max
    if (ext == ".ma" || ext == ".mb") return iconoMaya;              // Maya
    if (ext == ".fbx") return iconoFbx;
    if (ext == ".obj") return iconoObj;
    if (ext == ".png") return iconoPng;
    if (ext == ".jpg" || ext == ".jpe") return iconoJpg;
    if (ext == ".jpeg") return iconoJpeg;
    if (ext == ".tga") return iconoTga;
    if (ext == ".hdr") return iconoHdr;                              // HDR/OpenEXR legacy
    if (ext == ".exr") return iconoExr;
    if (ext == ".psd") return iconoPsd;
    if (ext == ".mp3") return iconoMp3;
    if (ext == ".wav") return iconoWav;
    if (ext == ".ogg") return iconoOgg;
    if (ext == ".ttf") return iconoTtf;
    if (ext == ".otf") return iconoOtf;
    if (ext == ".json") return iconoJson;
    if (ext == ".xml") return iconoXml;
    if (ext == ".csv") return iconoCsv;
    if (ext == ".rs") return iconoRs;                                // Rust
    if (ext == ".db" || ext == ".sqlite" || ext == ".sqlite3") return iconoDb; // Bases de datos
    if (ext == ".mtl") return iconoMtl;                              // Material Wavefront junto a .obj
    if (ext == ".rar" || ext == ".7z" || ext == ".tar" || ext == ".gz") return iconoRar;
    if (ext == ".zip") return iconoZip;
    return iconoArchivo;
}