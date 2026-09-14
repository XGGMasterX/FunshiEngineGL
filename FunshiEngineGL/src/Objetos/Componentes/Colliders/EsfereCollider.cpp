#include "EsfereCollider.h"

#include <cmath>
#include <GL/gl.h>
#include <memory>
#include <btBulletDynamicsCommon.h>

EsfereCollider::EsfereCollider(float radio, Transform* transformOfDadObject)
    : Collider(radio, transformOfDadObject) {}

std::unique_ptr<btCollisionShape> EsfereCollider::createCollisionShape() {
    return std::make_unique<btSphereShape>(radio);
}

void EsfereCollider::dibujarCollider() {
    Transform globalT = getGlobalTransform();
    float* pos = globalT.getTranslatef();

    const float PI = 3.14159265358979323846f;

    glPushMatrix();
    glTranslatef(pos[0], pos[1], pos[2]);

    const int meridians = 8;
    const int parallels = 4;

    glColor3f(0.0f, 1.0f, 0.0f);

    for (int m = 0; m < meridians; ++m) {
        float angle = (2.0f * PI * m) / meridians;

        glBegin(GL_LINE_STRIP);
        for (int p = 0; p <= 20; ++p) {
            float lat = PI * float(p) / 20.0f - PI / 2.0f;

            float x = radio * cosf(lat) * cosf(angle);
            float y = radio * sinf(lat);
            float z = radio * cosf(lat) * sinf(angle);

            glVertex3f(x, y, z);
        }
        glEnd();
    }

    for (int p = 1; p <= parallels; ++p) {
        float lat = PI * p / (parallels + 1) - PI / 2.0f;

        glBegin(GL_LINE_LOOP);
        for (int m = 0; m < 40; ++m) {
            float lon = 2.0f * PI * m / 40.0f;

            float x = radio * cosf(lat) * cosf(lon);
            float y = radio * sinf(lat);
            float z = radio * cosf(lat) * sinf(lon);

            glVertex3f(x, y, z);
        }
        glEnd();
    }

    glPopMatrix();
}