#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "shader.hpp"
#include "stb_image.h"

class Skybox {
    public:
        Skybox(std::vector<std::string> faces);
        void render(glm::mat4 view, glm::mat4 projection);
        Shader skyboxShader;
    
    private:
        std::vector<std::string> faces;
        void initSkybox();
        unsigned int skyboxVBO, skyboxVAO, cubemapTexture;

};

#endif