#ifndef UISPRITEANIM_HPP
#define UISPRITEANIM_HPP
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "shader.hpp"
#include "UIMesh.hpp"
#include "stb_image.h"
#include "spriteanimationtype.hpp"

class UISpriteAnim {
    public:
        UISpriteAnim(std::string textureAtlasPath, int frameWidth, int frameHeight, int xPos, int yPos);
        void initialize();
        void render(float deltaTime, SpriteAnimationType animationType = LOOP);
        glm::vec2 getPos();
        glm::vec2 getDims();
        bool animComplete();
    private:
        std::string textureAtlasPath;
        UIMesh uiMesh;
        Shader uiShader;
        unsigned int frameWidth;
        unsigned int frameHeight;
        int xPos;
        int yPos;
        unsigned int rows;
        unsigned int columns;
        unsigned int textureAtlasID;
        int atlasWidth;
        int atlasHeight;
        int curFrame;
        bool initialized;
        float timePerFrame2 = .025;
        float timeSinceLastFrame2 = 0;

        SpriteAnimationType curAnimType;
};

#endif