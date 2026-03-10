#include "progressbar.hpp"

ProgressBar::ProgressBar() {
    init();
}

void ProgressBar::init() {
    float topLeftX = 0;
    float topLeftY = 600;

    float topRightX = 0 + 800;
    float topRightY = 600;

    float bottomLeftX = 0;
    float bottomLeftY = 0;

    float bottomRightX = 0 + 800;
    float bottomRightY = 0;

    std::vector<glm::vec3> vertices = {
        glm::vec3(bottomLeftX, bottomLeftY, 0),
        glm::vec3(topLeftX, topLeftY, 0),
        glm::vec3(topRightX, topRightY, 0),
        glm::vec3(bottomLeftX, bottomLeftY, 0),
        glm::vec3(topRightX, topRightY, 0),
        glm::vec3(bottomRightX, bottomRightY, 0),
    };

    std::vector<glm::vec2> uvs = {
        glm::vec2(0.0f, 0.0f),  // Top-right
        glm::vec2(0.0f, 1.0f),  // Bottom-right
        glm::vec2(1.0f, 1.0f),  // Top-left
        glm::vec2(0.0f, 0.0f),  // Bottom-right
        glm::vec2(1.0f, 1.0f),  // Bottom-left
        glm::vec2(1.0f, 0.0f)   // Top-left
    };
    this->progressBarShader = std::make_shared<Shader>("src/shaders/uitextureelement.vs", "src/shaders/uiprogressbar.fs");
    this->uiMesh = std::make_shared<UIMesh>(vertices, uvs);

    int width, height, nChannels;
    unsigned char *data = stbi_load("resources/mainskybox/gradiesn.png", &width, &height, &nChannels, 0);

    if(data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "ProgressBar:: Failed to load texture";
    }
}

void ProgressBar::update(float progress) {
    this->progressBarShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    this->progressBarShader->setInt("uiTex", 0);
    this->progressBarShader->setFloat("progress", progress);
    this->progressBarShader->setVec2("screenSize", glm::vec2(800,600));
    this->progressBarShader->setMat4("projection", glm::ortho(0.0f, 800.0f, 0.0f, 600.0f));
    this->uiMesh->draw(*this->progressBarShader);
}