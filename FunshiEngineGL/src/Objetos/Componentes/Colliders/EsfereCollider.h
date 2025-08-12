#ifndef ESFERECOLLIDER_H
#define ESFERECOLLIDER_H
#include "Collider.h"
#include <cmath>
#include <GL/gl.h>

class EsfereCollider : public Collider {
public:
	EsfereCollider(float radio, Transform* transformOfDadObject) : 
        Collider(radio, transformOfDadObject) {

	}

    virtual btCollisionShape* createCollisionShape() override {
        return new btSphereShape(radio);
    }


    virtual void dibujarCollider() override {
        Transform* globalT = getGlobalTransform();
        float* pos = globalT->getTranslatef();

        glPushMatrix();
        glTranslatef(pos[0], pos[1], pos[2]);

        const int meridians = 8;
        const int parallels = 4;

        glColor3f(0.0f, 1.0f, 0.0f);

        for (int m = 0; m < meridians; ++m) {
            float angle = (2.0f * 3.14159265358979323846 * m) / meridians;

            glBegin(GL_LINE_STRIP);
            for (int p = 0; p <= 20; ++p) {
                float lat = 3.14159265358979323846 * (float)p / 20.0f - 3.14159265358979323846 / 2.0f;

                float x = radio * cosf(lat) * cosf(angle);
                float y = radio * sinf(lat);
                float z = radio * cosf(lat) * sinf(angle);

                glVertex3f(x, y, z);
            }
            glEnd();
        }

        for (int p = 1; p <= parallels; ++p) {
            float lat = 3.14159265358979323846 * p / (parallels + 1) - 3.14159265358979323846 / 2.0f;

            glBegin(GL_LINE_LOOP);
            for (int m = 0; m < 40; ++m) {
                float lon = 2.0f * 3.14159265358979323846 * m / 40.0f;

                float x = radio * cosf(lat) * cosf(lon);
                float y = radio * sinf(lat);
                float z = radio * cosf(lat) * sinf(lon);

                glVertex3f(x, y, z);
            }
            glEnd();
        }

        glPopMatrix();

        delete globalT;
    }





};
#endif