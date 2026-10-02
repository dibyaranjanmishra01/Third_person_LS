#pragma once
#include <iostream>

namespace input
{
    class InputState
    {
        public:
            InputState(const bool* keyboard, float mouseDeltaX, float mouseDeltaY);

            void debugPrintInputState() const; 
            
            float getForward() const;

            float getBackword() const;

            float getLeft() const;

            float getRight() const;

            bool getJump() const;

            float getMouseDeltaX() const;

            float getMouseDeltaY() const;
        private:
            float forward = 0.0f;
            float backward = 0.0f;
            float left = 0.0f;
            float right = 0.0f;
            bool jump = false;

            float mouseDeltaX = 0.0f;
            float mouseDeltaY = 0.0f;
    };
}
