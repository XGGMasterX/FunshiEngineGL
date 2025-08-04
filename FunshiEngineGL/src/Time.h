#ifndef TIME_H
#define TIME_H

#include <glfw3.h>
#include <vector>
#include <cstdlib>
//#include "Component.h" a terminar , es componnent o gizmo 

using namespace std;



class Time {
private:
    static float deltaTime;
    static float lastFrameTime;

public:
    static void update() {
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
    }

    static float getDeltaTime() {
        return deltaTime;
    }

    static void start() {
        lastFrameTime = (float)glfwGetTime();
    }

    static void fps(int FPS) {
        float targetFrameTime = 1.0f / FPS;
        float elapsed = (float)glfwGetTime() - lastFrameTime;
        if (elapsed < targetFrameTime) {
            float waitTime = targetFrameTime - elapsed;
            glfwWaitEventsTimeout(waitTime - 0.001);
        }
        update();
    }
};

float Time::deltaTime = 0.0f;
float Time::lastFrameTime = 0.0f;

#endif

