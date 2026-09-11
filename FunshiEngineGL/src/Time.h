#ifndef TIME_H
#define TIME_H

class Time {
private:
    static float deltaTime;
    static float lastFrameTime;
    static float fpsLastTime;
    static int frameCount;
    static float currentFPS;

public:
    static void start();
    static void update();
    static float getDeltaTime();
    static float getFPS();
    static void limitFPS(int targetFPS);
};

#endif
