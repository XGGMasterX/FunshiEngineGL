/*
    FunshiEngineGL - Motor de juegos 3D con OpenGL e ImGui
    Copyright 2026 Gianfranco Ivan Enrique

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    SPDX-License-Identifier: Apache-2.0
*/
#include "EngineTime.h"

// glfwGetTime: solo el reloj de GLFW, sin ningun estado de GL encima.
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

float Time::deltaTime = 0.0f;
float Time::lastFrameTime = 0.0f;
float Time::fpsLastTime = 0.0f;
int Time::frameCount = 0;
float Time::currentFPS = 0.0f;

void Time::start() {
    lastFrameTime = static_cast<float>(glfwGetTime());
    fpsLastTime = lastFrameTime;
    frameCount = 0;
    currentFPS = 0.0f;
}

void Time::update() {
    const float currentTime = static_cast<float>(glfwGetTime());
    deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;
    ++frameCount;
    if (currentTime - fpsLastTime >= 1.0f) {
        currentFPS = frameCount / (currentTime - fpsLastTime);
        frameCount = 0;
        fpsLastTime = currentTime;
    }
}

float Time::getDeltaTime() {
    return (deltaTime > 0.0001f) ? deltaTime : 0.0001f;
}

float Time::getFPS() {
    return currentFPS;
}

void Time::limitFPS(int targetFPS) {
    if (targetFPS <= 0) return;
    const float targetFrameTime = 1.0f / targetFPS;
    const float elapsed = static_cast<float>(glfwGetTime()) - lastFrameTime;
    const float remainingTime = targetFrameTime - elapsed;
    if (remainingTime > 0.0f) {
        std::this_thread::sleep_for(
            std::chrono::microseconds(static_cast<int>(remainingTime * 1000000 - 500)));
    }
    update();
}
