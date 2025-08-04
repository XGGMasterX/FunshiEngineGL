#ifndef TIME_H
#define TIME_H

#if defined(_WIN32)
#include <glfw3.h>
#elif defined(__linux__)
#include <GLFW/glfw3.h>
#endif
#include <chrono>
#include <thread>
#include <algorithm>

class Time {
private:
    static float deltaTime;
    static float lastFrameTime;
    static float fpsLastTime;
    static int frameCount;
    static float currentFPS;

public:
    static void start() {
        lastFrameTime = (float)glfwGetTime();
        fpsLastTime = lastFrameTime;
        frameCount = 0;
        currentFPS = 0.0f;
    }

    static void update() {
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        // Cálculo de FPS
        frameCount++;
        if (currentTime - fpsLastTime >= 1.0f) {
            currentFPS = frameCount / (currentTime - fpsLastTime);
            frameCount = 0;
            fpsLastTime = currentTime;
        }
    }

    static float getDeltaTime() {
        // Asegurar que deltaTime nunca sea negativo o cero
        return std::max(deltaTime, 0.0001f);
    }

    static float getFPS() {
        return currentFPS;
    }

    static void limitFPS(int targetFPS) {
        if (targetFPS <= 0) return;

        float targetFrameTime = 1.0f / targetFPS;
        float elapsed = (float)glfwGetTime() - lastFrameTime;
        float remainingTime = targetFrameTime - elapsed;

        if (remainingTime > 0.0f) {
            // Usar sleep_for para mayor precisión
            std::this_thread::sleep_for(
                std::chrono::microseconds(
                    (int)(remainingTime * 1000000 - 500) // Pequeño margen
                )
            );
        }

        update();
    }
};

// Inicialización de miembros estáticos
float Time::deltaTime = 0.0f;
float Time::lastFrameTime = 0.0f;
float Time::fpsLastTime = 0.0f;
int Time::frameCount = 0;
float Time::currentFPS = 0.0f;

#endif
