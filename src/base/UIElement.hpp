#ifndef UI_HPP
#define UI_HPP

#include <filesystem>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class UIElement { // maybe need to make ui mesh class but we will see. maybe just change buffer type in shader to vec3
    public:
        virtual void render(float deltaTime, float curTime) = 0;
        virtual void init(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight) = 0;
        virtual glm::vec2 getPos() = 0;
        virtual glm::vec2 getDims() = 0;
        virtual void updateWindowSize(int newWindowWidth, int newWindowHeight) = 0;
    private:
        int width;
        int height;
        int xPos;
        int yPos;
        int windowWidth;
        int windowHeight;
};

#endif