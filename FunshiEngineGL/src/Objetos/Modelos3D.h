#ifndef MODELOS3D_H
#define MODELOS3D_H

#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <algorithm>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Matematicas/StructVec3.h"
#include "../Objetos/Materiales/Material.h"
#include "../Objetos/GameObject.h"

class Modelos3D : public GameObject {
private:
    std::vector<vec3> vertices;
    std::vector<vec3> normals;                // ahora siempre mismo tamaño que vertices
    std::vector<unsigned int> indices;
    char filePath[100];

    void setObject() {
        vertices.clear();
        normals.clear();
        indices.clear();

        if (filePath[0] == '\0') return;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            filePath,
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_GenSmoothNormals |       // normales suaves si faltan o son planas
            aiProcess_PreTransformVertices     // aplica transformaciones de nodos
            // | aiProcess_ImproveCacheLocality
            // | aiProcess_OptimizeMeshes
        );

        if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
            std::cerr << "Assimp ERROR al cargar " << filePath << ": "
                      << importer.GetErrorString() << std::endl;
            return;
        }

        // Recorremos TODAS las mallas y acumulamos en buffers globales
        for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
            aiMesh* mesh = scene->mMeshes[m];
            if (!mesh) continue;

            // Guardamos el offset de inicio de esta malla en el vector global
            const unsigned int baseVertex = static_cast<unsigned int>(vertices.size());

            // Reservas para evitar realocaciones
            vertices.reserve(vertices.size() + mesh->mNumVertices);
            normals.reserve(normals.size() + mesh->mNumVertices);
            indices.reserve(indices.size() + mesh->mNumFaces * 3);

            // --- Vértices y normales (siempre 1:1) ---
            const bool hasNormals = mesh->HasNormals();
            for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
                // Posición
                vertices.emplace_back(mesh->mVertices[v].x,
                                      mesh->mVertices[v].y,
                                      mesh->mVertices[v].z);

                // Normal (si no hay, ponemos 0 para mantener alineación)
                if (hasNormals) {
                    normals.emplace_back(mesh->mNormals[v].x,
                                         mesh->mNormals[v].y,
                                         mesh->mNormals[v].z);
                } else {
                    normals.emplace_back(0.f, 0.f, 0.f);
                }
            }

            // --- Índices (siempre triángulos por aiProcess_Triangulate) ---
            for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
                const aiFace& face = mesh->mFaces[f];
                if (face.mNumIndices != 3) continue; // por seguridad

                // ¡Clave!: indices locales + baseVertex => indices globales correctos
                indices.push_back(baseVertex + face.mIndices[0]);
                indices.push_back(baseVertex + face.mIndices[1]);
                indices.push_back(baseVertex + face.mIndices[2]);
            }
        }
    }

public:
    Modelos3D(Entity* origin) : GameObject(origin) {
        filePath[0] = '\0';
    }

    Modelos3D() : GameObject() {
        filePath[0] = '\0';
    }

    void setPath(std::string path) {
    #if defined(_WIN32)
        strncpy_s(this->filePath, sizeof(this->filePath), path.c_str(), _TRUNCATE);
    #elif defined(__linux__)
        strncpy(this->filePath, path.c_str(), sizeof(this->filePath) - 1);
        this->filePath[sizeof(this->filePath) - 1] = '\0';
    #endif
        setObject();
    }

    std::string getPath() {
        return filePath;
    }

    void dibujar(float deltaTime) override {
        const int tam = 3;
        float* color[tam];

        update(deltaTime);
        getGlobalTransform()->position();
        Model* model = getComponent<Model>();
        if(model != nullptr){
          if(model->getPath() != string(filePath)){
             setPath(model->getPath());
          }
        }
        // Color por componente o blanco
        if (getComponent<Color>() != nullptr) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, getColor(*color, tam));
        } else {
            GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_TRIANGLES);

        // Ahora normals y vertices están alineados 1:1 con índices globales correctos
        for (size_t i = 0; i < indices.size(); i++) {
            unsigned int idx = indices[i];
            if (idx < vertices.size()) {
                // Si la normal es (0,0,0) la omitimos (OpenGL la mantiene previa)
                const vec3& n = normals[idx];
                if (!(n.x == 0.f && n.y == 0.f && n.z == 0.f)) {
                    glNormal3fv(&n.x);
                }
                glVertex3fv(&vertices[idx].x);
            }
        }
        glEnd();
        glPopMatrix();
    }

protected:
    virtual void serializeEntity() override {
        size_t len = strnlen(filePath, sizeof(filePath));
        myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&len), sizeof(size_t));
        if (len > 0) {
            myBinario->getOfBinariFile()->write(filePath, len);
        } else {
            std::cout << "No hay un path en el Objeto para almacenarlo" << std::endl;
        }
    }

    virtual void deserializeEntity() override {
        size_t len = 0;
        myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&len), sizeof(size_t));

        if (len > 0) {
            char buffer[100] = { 0 };
            size_t toRead = std::min(len, static_cast<size_t>(sizeof(buffer) - 1));
            myBinario->getIfBinariFile()->read(buffer, toRead);
            buffer[toRead] = '\0';

        #if defined(_WIN32)
            strncpy_s(filePath, sizeof(filePath), buffer, _TRUNCATE);
        #elif defined(__linux__)
            strncpy(this->filePath, buffer, sizeof(this->filePath) - 1);
            this->filePath[sizeof(this->filePath) - 1] = '\0';
        #endif
            std::cout << filePath << std::endl;
            setPath(filePath); // reconstruye buffers
        } else {
            filePath[0] = '\0';
            std::cout << "No hay un path en el binario" << std::endl;
        }
    }

public:
    virtual void saveEntity(std::string filename) override {
        std::string path = filename + "/ObjectN" + std::to_string(getId()) + ".db";
        myBinario = new Binario(path);
        myBinario->ofOpenBinary();

        std::ofstream archivo(filename + "BBDDObjetos.txt", std::ios::app);
        if (!archivo.is_open()) {
            std::cerr << "No se pudo abrir BBDDObjetos.txt" << std::endl;
            return;
        }
        archivo << path << std::endl;

        GameObject::serializeEntity();
        serializeEntity();

        myBinario->ofCloseBinary();
    }

    virtual void loadEntity(std::string filename) override {
        std::string path = filename + "/ObjectN" + std::to_string(getId()) + ".db";
        myBinario = new Binario(path);
        myBinario->ifOpenBinary();

        GameObject::deserializeEntity();
        deserializeEntity();

        myBinario->ifCloseBinary();
    }
};

#endif
