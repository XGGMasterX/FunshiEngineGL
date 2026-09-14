#include "MallaCollider.h"

#include <GL/gl.h>
#include <btBulletDynamicsCommon.h>

#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Modelos3D.h"

MallaCollider::MallaCollider(float radio, Transform* transformOfDadObject,
                             GameObject* meshOwner)
    : Collider(radio, transformOfDadObject), meshOwner(meshOwner) {}

std::unique_ptr<btCollisionShape> MallaCollider::createCollisionShape() {
    // Convex hull de los vertices del modelo (es el colisionador geometrico
    // exacto de la malla). Sin malla cargada se cae a la shape de respaldo.
    Modelos3D* modelo = dynamic_cast<Modelos3D*>(meshOwner);
    if (!modelo) return nullptr;

    const std::vector<vec3>& vertices = modelo->getVertices();
    if (vertices.size() < 4) return nullptr;

    auto hull = std::make_unique<btConvexHullShape>();
    for (const vec3& v : vertices) {
        hull->addPoint(btVector3(v.x, v.y, v.z));
    }
    return hull;
}

void MallaCollider::dibujarCollider() {
    Transform globalT = getGlobalTransform();
    float* pos = globalT.getTranslatef();

    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);
    glColor3f(0.0f, 1.0f, 0.0f);

    // Dibujo el hull como una caja envolvente rapida al radio.
    float r = getRadio();
    GLfloat vertices[8][3] = {
        {-r, -r, -r}, { r, -r, -r}, { r,  r, -r}, {-r,  r, -r},
        {-r, -r,  r}, { r, -r,  r}, { r,  r,  r}, {-r,  r,  r}
    };

    GLuint edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0},
        {4,5}, {5,6}, {6,7}, {7,4},
        {0,4}, {1,5}, {2,6}, {3,7}
    };

    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        glVertex3fv(vertices[edges[i][0]]);
        glVertex3fv(vertices[edges[i][1]]);
    }
    glEnd();

    glPopMatrix();
}