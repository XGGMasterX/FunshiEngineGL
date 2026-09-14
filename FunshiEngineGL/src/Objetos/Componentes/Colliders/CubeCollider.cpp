#include "CubeCollider.h"

#include <GL/gl.h>
#include <memory>
#include <btBulletDynamicsCommon.h>

CubeCollider::CubeCollider(float radio, Transform* transformOfDadObject)
    : Collider(radio, transformOfDadObject) {}

std::unique_ptr<btCollisionShape> CubeCollider::createCollisionShape() {
    // Usar el radio como la mitad de tamano (box usa half extents)
    return std::make_unique<btBoxShape>(btVector3(radio, radio, radio));
}

void CubeCollider::dibujarCollider() {
    Transform globalT = getGlobalTransform();
    float* pos = globalT.getTranslatef();

    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);

    // Dibujo del cubo en origen local
    GLfloat vertices[8][3] = {
        {-radio, -radio, -radio},
        { radio, -radio, -radio},
        { radio,  radio, -radio},
        {-radio,  radio, -radio},
        {-radio, -radio,  radio},
        { radio, -radio,  radio},
        { radio,  radio,  radio},
        {-radio,  radio,  radio}
    };

    GLuint edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0},
        {4,5}, {5,6}, {6,7}, {7,4},
        {0,4}, {1,5}, {2,6}, {3,7}
    };

    glColor3f(0.0f, 1.0f, 0.0f); // verde

    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        glVertex3fv(vertices[edges[i][0]]);
        glVertex3fv(vertices[edges[i][1]]);
    }
    glEnd();

    glPopMatrix();
}