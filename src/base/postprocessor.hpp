#ifndef POSTPROCESSOR_HPP
#define POSTPROCESSOR_HPP

#include "UIMesh.hpp"
#include "shader.hpp"

class PostProcessor {
    public:
        PostProcessor();
        void initialize();
        void begin();
        void render(float dt);
        void setScreenSize(glm::vec2 screenSize);
    private:
        Shader *screenShader;
        bool initialized;
        UIMesh *quad;
        glm::vec2 screenSize();
        unsigned int quadVAO, quadVBO;
        unsigned int frameBuffer;
        unsigned int textureColorBuffer;
        unsigned int renderBuffer;
};

#endif