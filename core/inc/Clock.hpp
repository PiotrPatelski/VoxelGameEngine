#pragma once

class Clock {
   public:
    Clock() = default;
    ~Clock() = default;
    Clock(const Clock&) = delete;
    Clock(Clock&&) = delete;
    Clock& operator=(const Clock&) = delete;
    Clock& operator=(Clock&&) = delete;
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