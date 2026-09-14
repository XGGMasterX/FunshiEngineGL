#include "MallaCollider.h"

#include <GL/gl.h>
#include <btBulletDynamicsCommon.h>

#include "../../../Objetos/GameObject.h"
#include "../../../Objetos/Modelos3D.h"

MallaCollider::MallaCollider(float radio, Transform* transformOfDadObject,
                             GameObject* owner)
    : Collider(radio, transformOfDadObject, owner) {}

std::unique_ptr<btCollisionShape> MallaCollider::createCollisionShape() {
    // Convex hull de los vertices del modelo (es el colisionador geometrico
    // exacto de la malla). Sin malla cargada se cae a la shape de respaldo.
    Modelos3D* modelo = dynamic_cast<Modelos3D*>(owner);
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
    float modelArr[16];
    buildMatrixFromTransform(&globalT, modelArr);

    glPushMatrix();
    glMultMatrixf(modelArr);
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 1.0f, 0.0f);

    // Dibujo el hull real (bordes del convex hull) en vez de la caja
    // aproximada. getCollisionShape() construye la shape de forma lazy si
    // todavia no existe; es lo que permite ver el hull apenas se crea el
    // collider, sin esperar a que la fisica lo genere.
    btConvexHullShape* hull =
        dynamic_cast<btConvexHullShape*>(getCollisionShape());
    if (hull && hull->getNumPoints() > 0) {
        const btVector3* points = hull->getUnscaledPoints();
        const int numPoints = hull->getNumPoints();
        glBegin(GL_LINES);
        for (int i = 0; i < numPoints; ++i) {
            const btVector3& a = points[i];
            const btVector3& b = points[(i + 1) % numPoints];
            glVertex3f(a.x(), a.y(), a.z());
            glVertex3f(b.x(), b.y(), b.z());
        }
        glEnd();
    } else {
        // Respaldo grafico: caja envolvente rapida al radio.
        float r = getRadio();
        GLfloat box[8][3] = {
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
            glVertex3fv(box[edges[i][0]]);
            glVertex3fv(box[edges[i][1]]);
        }
        glEnd();
    }

    glEnable(GL_LIGHTING);
    glPopMatrix();
}