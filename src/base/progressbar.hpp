#ifndef PROGRESSBAR_HPP
#define PROGRESSBAR_HPP
#include "shader.hpp"
#include "UIMesh.hpp"
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ProgressBar {
    public:
        ProgressBar();
        void init();
        void update(float progress);
    private:
        std::shared_ptr<Shader> progressBarShader;
        std::shared_ptr<UIMesh> uiMesh;
        unsigned int textureID;
};

#endif