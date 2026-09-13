#include "Modelos3D.h"

#include <GL/gl.h>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstring>
#include <fstream>
#include <iostream>

#include "../Objetos/Componentes/Material.h"

Modelos3D::Modelos3D(Entity* origin) : GameObject(origin) { filePath[0] = '\0'; }
Modelos3D::Modelos3D() : GameObject() { filePath[0] = '\0'; }

void Modelos3D::setPath(std::string path) {
#if defined(_WIN32)
    strncpy_s(filePath, sizeof(filePath), path.c_str(), _TRUNCATE);
#else
    std::strncpy(filePath, path.c_str(), sizeof(filePath) - 1);
    filePath[sizeof(filePath) - 1] = '\0';
#endif
    setObject();
}

std::string Modelos3D::getPath() { return filePath; }

void Modelos3D::setObject() {
    vertices.clear(); normals.clear(); indices.clear();
    if (filePath[0] == '\0') return;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath,
        aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
        aiProcess_GenSmoothNormals | aiProcess_PreTransformVertices);
    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        std::cerr << "Assimp ERROR al cargar " << filePath << ": "
                  << importer.GetErrorString() << '\n';
        return;
    }
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];
        if (!mesh) continue;
        const unsigned int base = static_cast<unsigned int>(vertices.size());
        vertices.reserve(vertices.size() + mesh->mNumVertices);
        normals.reserve(normals.size() + mesh->mNumVertices);
        indices.reserve(indices.size() + mesh->mNumFaces * 3);
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
            vertices.emplace_back(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
            if (mesh->HasNormals())
                normals.emplace_back(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
            else
                normals.emplace_back(0.f, 0.f, 0.f);
        }
        for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
            const aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices == 3) {
                indices.push_back(base + face.mIndices[0]);
                indices.push_back(base + face.mIndices[1]);
                indices.push_back(base + face.mIndices[2]);
            }
        }
    }
}

void Modelos3D::dibujar(float deltaTime) {
    update(deltaTime);
    Transform* transform = getGlobalTransform();
    if (transform) transform->position();
    if (Model* model = getComponent<Model>(); model && model->getPath() != filePath)
        setPath(model->getPath());
    if (Material* material = getComponent<Material>()) {
        material->aplicar();
    } else if (Color* color = getComponent<Color>()) {
        glMaterialfv(GL_FRONT, GL_DIFFUSE, color->getColor());
    } else {
        const GLfloat white[] = {1.f, 1.f, 1.f, 1.f};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_TRIANGLES);
    for (unsigned int idx : indices) {
        if (idx >= vertices.size()) continue;
        const vec3& normal = normals[idx];
        if (!(normal.x == 0.f && normal.y == 0.f && normal.z == 0.f))
            glNormal3fv(&normal.x);
        glVertex3fv(&vertices[idx].x);
    }
    glEnd();
    glPopMatrix();
}

void Modelos3D::serializeEntity() {
    const size_t length = strnlen(filePath, sizeof(filePath));
    myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&length), sizeof(size_t));
    if (length > 0) myBinario->getOfBinariFile()->write(filePath, length);
}

void Modelos3D::deserializeEntity() {
    size_t length = 0;
    myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&length), sizeof(size_t));
    char buffer[100] = {};
    const size_t toRead = std::min(length, sizeof(buffer) - 1);
    if (toRead > 0) {
        myBinario->getIfBinariFile()->read(buffer, toRead);
        std::strncpy(filePath, buffer, sizeof(filePath) - 1);
        filePath[sizeof(filePath) - 1] = '\0';
        setObject();
    } else {
        filePath[0] = '\0';
    }
}

void Modelos3D::saveEntity(std::string filename) {
    const std::string path = filename + "/ObjectN" + std::to_string(getId()) + ".db";
    myBinario = std::make_unique<Binario>(path);
    myBinario->ofOpenBinary();
    
    // Leer todo el contenido del archivo
    std::string fullPath = filename + "BBDDObjetos.txt";
    std::ifstream readFile(fullPath);
    std::string content;
    if (readFile.is_open()) {
        std::string line;
        while (std::getline(readFile, line)) {
            content += line + "\n";
        }
        readFile.close();
    }
    
    // Agregar el nuevo path
    content += path + "\n";
    
    // Escribir todo el contenido de una vez
    std::ofstream writeFile(fullPath);
    if (writeFile.is_open()) {
        writeFile << content;
        writeFile.close();
    }
    
    GameObject::serializeEntity();
    serializeEntity();
    myBinario->ofCloseBinary();
}

void Modelos3D::loadEntity(std::string filename) {
    const std::string path = filename + "/ObjectN" + std::to_string(getId()) + ".db";
    myBinario = std::make_unique<Binario>(path);
    myBinario->ifOpenBinary();
    GameObject::deserializeEntity();
    deserializeEntity();
    myBinario->ifCloseBinary();
}

bool Modelos3D::getBoundingBox(vec3& outMin, vec3& outMax) const {
    if (vertices.empty()) return false;
    outMin = vertices[0];
    outMax = vertices[0];
    for (size_t i = 1; i < vertices.size(); ++i) {
        outMin.x = std::min(outMin.x, vertices[i].x);
        outMin.y = std::min(outMin.y, vertices[i].y);
        outMin.z = std::min(outMin.z, vertices[i].z);
        outMax.x = std::max(outMax.x, vertices[i].x);
        outMax.y = std::max(outMax.y, vertices[i].y);
        outMax.z = std::max(outMax.z, vertices[i].z);
    }
    return true;
}

