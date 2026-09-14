#ifndef MODELOS3D_H
#define MODELOS3D_H

#include <string>
#include <vector>

#include "../Matematicas/StructVec3.h"
#include "../Objetos/GameObject.h"

class Modelos3D : public GameObject {
private:
    std::vector<vec3> vertices;
    std::vector<vec3> normals;
    std::vector<unsigned int> indices;
    char filePath[100];

    void setObject();

public:
    explicit Modelos3D(Entity* origin);
    Modelos3D();

    void setPath(std::string path);
    std::string getPath();
    void dibujar(float deltaTime) override;
    bool getBoundingBox(vec3& outMin, vec3& outMax) const;
    // Vertices en espacio local del modelo (para construir shapes de colision).
    const std::vector<vec3>& getVertices() const { return vertices; }

protected:
    void serializeEntity() override;
    void deserializeEntity() override;

public:
    void saveEntity(std::string filename) override;
    void loadEntity(std::string filename) override;
};

#endif
