#include "uipanel.hpp"

void clickQuit() {
    std::cout << "QUIT CLICKED" << std::endl;
}

UIPanel::UIPanel(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight, glm::vec4 backgroundColor) :
    width(width), height(height), xPos(xPos), yPos(yPos), windowWidth(windowWidth), windowHeight(windowHeight), backgroundColor(backgroundColor)
{
    init(width, height, xPos, yPos, windowWidth, windowHeight);
}

UIPanel::UIPanel(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight, std::string backgroundImagePath) :
    width(width), height(height), xPos(xPos), yPos(yPos), windowWidth(windowWidth), windowHeight(windowHeight), backgroundImagePath(backgroundImagePath)
{
    this->backgroundColor = glm::vec4(0,0,0,0);
    init(width, height, xPos, yPos, windowWidth, windowHeight);
}

void UIPanel::init(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight) {
    float topLeftX = xPos;
    float topLeftY = yPos;

    float topRightX = xPos + width;
    float topRightY = yPos;

    float bottomLeftX = xPos;
    float bottomLeftY = yPos + height;

    float bottomRightX = xPos + width;
    float bottomRightY = yPos + height;

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

    if(this->backgroundImagePath.size() > 0) {
        int nChannels;
        std::cout << "loading ui panel bg: " << this->backgroundImagePath << std::endl;
        unsigned char *data = stbi_load(this->backgroundImagePath.c_str(), &width, &height, &nChannels, 0);
        std::cout << "UI PANEL BG DIMS: W:" << width << " H:" << height << std::endl;
        if(data) {
            glGenTextures(1, &backgroundImageTextureID);
            glBindTexture(GL_TEXTURE_2D, backgroundImageTextureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            std::cout << "UISpriteAnim:: Failed to load texture atlas.";
        }
    }

    this->uiMesh = std::make_shared<UIMesh>(vertices, uvs);
    this->uiPanelShader = std::make_shared<Shader>("src/shaders/uielement.vs", "src/shaders/uielement.fs");
    UIButton *quitButton = new UIButton(200, 50, 300, 300, 800.0f, 600.0f, glm::vec4(0.0, 0.0, 1.0, 1.0), "QUIT", clickQuit);
    this->buttons.push_back(quitButton);
}

void UIPanel::render(float deltaTime, float curTime) {
    this->uiPanelShader->use();

    if(this->backgroundImagePath.size() > 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, backgroundImageTextureID);
        uiPanelShader->setInt("uiTex", 0);
    }

    this->uiPanelShader->setMat4("projection", glm::ortho(0.0f, 800.0f, 0.0f, 600.0f));
    this->uiPanelShader->setVec4("inColor", this->backgroundColor);
    this->uiPanelShader->setFloat("dHoverTime", 0);
    this->uiPanelShader->setFloat("animTime", 2);
    uiMesh->draw(*uiPanelShader);
    glBindTexture(GL_TEXTURE_2D, 0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    for(int i = 0; i < this->buttons.size(); i++) {
        this->buttons[i]->render(deltaTime, curTime);
    }
    glEnable(GL_DEPTH_TEST);
}

glm::vec2 UIPanel::getPos() {
    return glm::vec2(windowWidth, windowHeight);
}

glm::vec2 UIPanel::getDims() {
    return glm::vec2(width, height);
}

void UIPanel::updateWindowSize(int newWindowWidth, int newWindowHeight) {

}