#ifndef CUBECOLLIDER_H
#define CUBECOLLIDER_H
#include "Collider.h"
#include <GL/gl.h>
class CubeCollider : public Collider {
public:
    CubeCollider(float radio, Transform* transformOfDadObject) : 
        Collider(radio, transformOfDadObject) {

    }

    virtual btCollisionShape* createCollisionShape() override {
        // Usar el radio como la mitad de tamaño (box usa half extents)
        return new btBoxShape(btVector3(radio, radio, radio));
    }

    virtual void dibujarCollider() override {
        Transform* globalT = getGlobalTransform();
        float* pos = globalT->getTranslatef();

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

        delete globalT;
    }

};
#endif