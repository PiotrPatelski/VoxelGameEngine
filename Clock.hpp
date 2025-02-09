#pragma once

class Clock {
   public:
    Clock();
    ~Clock();
    inline float getDeltaTime() const { return deltaTime; }
    unsigned int calculateFps();

   private:
    float deltaTime{0.0f}; // Time between current frame and last frame
    float lastFrame{0.0f}; // Time of last frame
    float fpsTimer{0.0f};
    float fpsUpdateInterval{1.0f};
    unsigned int frameCount{0};
    unsigned int currentFps{0};
};