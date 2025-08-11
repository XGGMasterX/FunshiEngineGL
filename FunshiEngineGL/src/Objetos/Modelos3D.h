#ifndef MODELOS3D_H
#define MODELOS3D_H

#include <vector>
#include <string>
#include <iostream>
#include <cstring>  // necesario para strncpy_s

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Matematicas/StructVec3.h"
#include "../Objetos/Materiales/Material.h"
#include "../Objetos/GameObject.h"

class Modelos3D : public GameObject {
private:
    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<unsigned int> indices;
    char filePath[100];

    void setObject() {
        vertices.clear();
        normals.clear();
        indices.clear();

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filePath,
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_JoinIdenticalVertices);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "Assimp ERROR: " << importer.GetErrorString() << std::endl;
            return;
        }

        // Procesar todas las mallas del modelo
        for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
            aiMesh* mesh = scene->mMeshes[m];

            // Procesar vértices
            for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
                vertices.emplace_back(
                    mesh->mVertices[v].x,
                    mesh->mVertices[v].y,
                    mesh->mVertices[v].z
                );
            }

            // Procesar normales
            if (mesh->HasNormals()) {
                for (unsigned int n = 0; n < mesh->mNumVertices; n++) {
                    normals.emplace_back(
                        mesh->mNormals[n].x,
                        mesh->mNormals[n].y,
                        mesh->mNormals[n].z
                    );
                }
            }

            // Procesar índices (caras)
            for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
                aiFace face = mesh->mFaces[f];
                for (unsigned int i = 0; i < face.mNumIndices; i++) {
                    indices.push_back(face.mIndices[i]);
                }
            }
        }
    }

public: 
    Modelos3D(std::string filePath) : GameObject() {
#if defined(_WIN32)
        strncpy_s(this->filePath, sizeof(this->filePath), filePath.c_str(), _TRUNCATE);
#elif defined(__linux__)
        strncpy(this->filePath, filePath.c_str(), sizeof(this->filePath) - 1);
        this->filePath[sizeof(this->filePath) - 1] = '\0';
#endif
        setObject();
    }

    Modelos3D() : GameObject() {}

    void setPath(std::string path) {
#if defined(_WIN32)
        strncpy_s(this->filePath, sizeof(this->filePath), path.c_str(), _TRUNCATE);
#elif defined(__linux__)
        strncpy(this->filePath, filePath, sizeof(this->filePath) - 1);
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
        getComponent<Transform>()->position();

        //AGREGA EL Color
        if (getComponent<Color>() != nullptr) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE, getColor(*color, tam));
        }
        else {
            GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
        }
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBegin(GL_TRIANGLES);

        for (size_t i = 0; i < indices.size(); i++) {
            unsigned int idx = indices[i];

            if (!normals.empty() && idx < normals.size())
                glNormal3fv(&normals[idx].x);

            if (idx < vertices.size())
                glVertex3fv(&vertices[idx].x);
        }
        glEnd();
        glPopMatrix();
    }

private:
    void serializeObject() {
        size_t len = std::strlen(filePath);
        myBinario->getOfBinariFile()->write(reinterpret_cast<const char*>(&len), sizeof(size_t));
        if (len > 0) {
            myBinario->getOfBinariFile()->write(filePath, len);
        }
        else {
            std::cout << "No hay un path en el Objeto para almacenarlo" << std::endl;
        }
    }

    void deserializeObject() {
        size_t len;
        myBinario->getIfBinariFile()->read(reinterpret_cast<char*>(&len), sizeof(size_t));

        if (len > 0) {
            char buffer[100] = { 0 };
            len = std::min(len, size_t(99));
            myBinario->getIfBinariFile()->read(buffer, len);
            buffer[len] = '\0';

 #if defined(_WIN32)
            strncpy_s(filePath, sizeof(filePath), buffer, _TRUNCATE);
 #elif defined(__linux__)
            strncpy(this->filePath, filePath, sizeof(this->filePath) - 1);
            this->filePath[sizeof(this->filePath) - 1] = '\0';
 #endif
            std::cout << filePath << std::endl;
            setPath(filePath); //PARA QUE LO ARME SINO DE NADA ME SIRVE EL PATH XD
        }
        else {
            filePath[0] = '\0';
            std::cout << "No hay un path en el binario" << std::endl;
        }
    }

public:
    void saveObject(std::string filename) {
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
        serializeObject();

        myBinario->ofCloseBinary();
    }

    void loadObject(std::string filename) {
        std::string path = filename + "/ObjectN" + std::to_string(getId()) + ".db";
        myBinario = new Binario(path);
        myBinario->ifOpenBinary();

        GameObject::deserializeEntity();
        deserializeObject();

        myBinario->ifCloseBinary();
    }
};
#endif
