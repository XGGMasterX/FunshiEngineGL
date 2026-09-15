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
// Pruebas headless del paquete Assets: normalizacion de rutas (AssetPath),
// geometria (Mesh) y cache Flyweight (AssetManager) con un loader artificial,
// sin pila grafica ni Assimp. Solo std C++17.

#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "../FunshiEngineGL/src/Assets/AssetException.h"
#include "../FunshiEngineGL/src/Assets/AssetManager.h"
#include "../FunshiEngineGL/src/Assets/AssetPath.h"
#include "../FunshiEngineGL/src/Assets/Mesh.h"

namespace fs = std::filesystem;

namespace {
int total = 0;
int fallos = 0;

#define CHECK(cond, msg)                                                      \
    do {                                                                      \
        ++total;                                                              \
        if (!(cond)) {                                                        \
            ++fallos;                                                         \
            std::cout << "FALLO: " << msg << " (linea " << __LINE__ << ")"    \
                      << std::endl;                                           \
        }                                                                     \
    } while (0)

// Malla unitaria de 8 vertices (cubo sin indices): util para validar bounds.
std::shared_ptr<Mesh> cuboUnitario() {
    auto mesh = std::make_shared<Mesh>();
    mesh->name = "cuboUnitario";
    const float c[][3] = {
        {0.f, 0.f, 0.f}, {1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, 1.f},
        {1.f, 1.f, 0.f}, {1.f, 0.f, 1.f}, {0.f, 1.f, 1.f}, {1.f, 1.f, 1.f}};
    for (const auto& v : c) mesh->vertices.emplace_back(v[0], v[1], v[2]);
    mesh->uvs.emplace_back(0.f, 0.f); // solo el primero: hasUvs() debe fallar
    return mesh;
}

// Loader artificial: crea una malla cuyo nombre es el path, cuenta cargas y
// falla para el path marcado como corrupto.
class LoaderStub : public IMeshLoader {
public:
    int cargas = 0;
    std::shared_ptr<Mesh> load(const std::string& path) override {
        ++cargas;
        if (AssetPath::hasExtension(path, "corrupto"))
            throw AssetLoadException(path, "archivo marcado como corrupto");
        auto mesh = std::make_shared<Mesh>();
        mesh->name = path;
        mesh->vertices.emplace_back(0.f, 0.f, 0.f);
        mesh->vertices.emplace_back(1.f, 0.f, 0.f);
        mesh->vertices.emplace_back(0.f, 1.f, 0.f);
        mesh->indices = {0, 1, 2};
        return mesh;
    }
};

void testAssetPath() {
    CHECK(AssetPath::normalize("a\\b/c//d/") == "a/b/c/d",
          "normalize unifica separadores y colapsa //");
    CHECK(AssetPath::normalize("model.obj\\") == "model.obj",
          "normalize quita el separador final");
    CHECK(AssetPath::normalize("C:/") == "C:/",
          "normalize preserva la raiz de unidad");
    CHECK(AssetPath::extension("Model.OBJ") == "obj",
          "extension en minusculas");
    CHECK(AssetPath::extension("models/cubo") == "",
          "sin punto no hay extension");
    CHECK(AssetPath::extension("a/b.c/d.obj") == "obj",
          "extension de la ultima seccion");
    CHECK(AssetPath::hasExtension("cubo.FBX", "fbx"),
          "hasExtension ignora mayusculas");
    CHECK(!AssetPath::hasExtension("cubo.obj", "fbx"),
          "hasExtension distingue extensiones");
    CHECK(AssetPath::join("Meshes", "cubo.obj") == "Meshes/cubo.obj",
          "join base + relativa");
    CHECK(AssetPath::join("", "x.obj") == "x.obj", "join con base vacia");
    CHECK(AssetPath::join("/base/", "//rel/m.obj") == "/base/rel/m.obj",
          "join normaliza ambas rutas");
    CHECK(AssetPath::basename("models/cubo.obj") == "cubo",
          "basename quita directorio y extension");
    const fs::path tempAbs = fs::temp_directory_path();
    CHECK(AssetPath::isAbsolute(tempAbs.string()),
          "isAbsolute acepta rutas absolutas reales");
    CHECK(!AssetPath::isAbsolute("rel/m.obj"), "isAbsolute rechaza relativas");
}

void testMesh() {
    auto mesh = cuboUnitario();
    CHECK(!mesh->isEmpty(), "mesh poblada no esta vacia");
    CHECK(!mesh->hasNormals(), "sin normales hasNormals() es false");
    CHECK(!mesh->hasUvs(), "con UVs incompletas hasUvs() es false");
    vec3 mn, mx;
    CHECK(mesh->computeBounds(mn, mx), "computeBounds con vertices");
    CHECK(mn.x == 0.f && mn.y == 0.f && mn.z == 0.f,
          "minimo del cubo unitario");
    CHECK(mx.x == 1.f && mx.y == 1.f && mx.z == 1.f,
          "maximo del cubo unitario");
    Mesh vacio;
    CHECK(!vacio.computeBounds(mn, mx), "computeBounds con mesh vacia es false");
}

void testAssetManager() {
    AssetManager sinLoader;
    CHECK(sinLoader.meshCount() == 0, "manager nuevo vacio");

    // Sin loader, pedir una malla ausente debe lanzar.
    try {
        sinLoader.getMesh("nos/e x_i-ste.obj");
        CHECK(false, "getMesh sin loader deberia lanzar AssetNotFoundException");
    } catch (const AssetNotFoundException& e) {
        CHECK(std::string(e.what()).find("Asset") != std::string::npos,
              "mensaje de AssetNotFoundException");
    } catch (...) {
        CHECK(false, "tipo de excepcion incorrecto para asset ausente");
    }

    auto loader = std::make_unique<LoaderStub>();
    LoaderStub* loaderRaw = loader.get();
    AssetManager manager(std::move(loader));

    // Carga y cache: el MISMO path solo se parsea una vez.
    auto a = manager.getMesh("Meshes/Cubo.obj");
    auto b = manager.getMesh("Meshes/Cubo.obj");
    CHECK(a.get() == b.get(), "getMesh con el mismo path devuelve el mismo objeto");
    CHECK(loaderRaw->cargas == 1, "la malla se parseo una sola vez");
    CHECK(manager.meshCount() == 1, "una entrada en el cache");

    // Claves normalizadas: distintos separadores -> mismo recurso.
    auto c = manager.getMesh("Meshes\\Cubo.obj");
    CHECK(c.get() == a.get(), "separadores distintos resuelven al mismo asset");
    CHECK(loaderRaw->cargas == 1, "sin recarga por solo cambiar separadores");

    // Loader que falla: la excepcion del loader debe propagarse.
    try {
        manager.getMesh("Ro to.obj.corrupto");
        CHECK(false, "getMesh con loader fallido deberia lanzar");
    } catch (const AssetLoadException&) {
        // esperado
    } catch (...) {
        CHECK(false, "tipo de excepcion incorrecto para carga fallida");
    }

    // putMesh/contains/remove.
    manager.putMesh("Procedural/Tri.obj", cuboUnitario());
    CHECK(manager.containsMesh("Procedural/Tri.obj"), "putMesh registra");
    CHECK(manager.meshCount() == 2, "una entrada mas tras putMesh");
    try {
        manager.putMesh("vacio.obj", std::make_shared<Mesh>());
        CHECK(false, "putMesh con malla vacia deberia lanzar");
    } catch (const AssetLoadException&) {
        // esperado
    } catch (...) {
        CHECK(false, "tipo de excepcion incorrecto para malla vacia");
    }
    manager.removeMesh("Procedural/Tri.obj");
    CHECK(!manager.containsMesh("Procedural/Tri.obj"), "removeMesh elimina");

    // reloadMesh: cambia la identidad pero no invalida a los holders viejos.
    {
        auto antes = manager.getMesh("Meshes/Cubo.obj");
        manager.reloadMesh("Meshes/Cubo.obj");
        auto despues = manager.getMesh("Meshes/Cubo.obj");
        CHECK(antes.get() != despues.get(),
              "reloadMesh reemplaza la malla en el cache");
        CHECK(antes->vertices.size() == 3,
              "el holder anterior conserva datos validos tras el reload");
    }

    // clearUnusedMeshes: evicta solo lo que nadie usa.
    {
        auto retenido = manager.getMesh("Meshes/Cubo.obj"); // use_count 2
        manager.clearUnusedMeshes();
        CHECK(manager.containsMesh("Meshes/Cubo.obj"),
              "malla retenida sobrevive a clearUnusedMeshes");
    }
    manager.clearUnusedMeshes();
    CHECK(!manager.containsMesh("Meshes/Cubo.obj"),
          "malla sin usuarios externos se evicta");
}

} // namespace

int main() {
    testAssetPath();
    testMesh();
    testAssetManager();

    std::cout << "Resultado: " << (total - fallos) << "/" << total
              << " OK" << std::endl;
    if (fallos > 0) {
        std::cout << fallos << " prueba(s) fallaron." << std::endl;
        return 1;
    }
    return 0;
}