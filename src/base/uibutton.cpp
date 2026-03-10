#include "uibutton.hpp"

UIButton::UIButton(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight, glm::vec4 backgroundColor, std::string buttonText, void(*clickEvent)())
    : width(width), height(height), xPos(xPos), yPos(yPos), windowWidth(windowWidth), windowHeight(windowHeight), backgroundColor(backgroundColor), buttonText(buttonText), click(clickEvent),
      hovering(false), hoverStarted(false), hoverStartTime(0) {
        this->buttonTextElement = std::make_shared<UITextElement>("resources/text/Angelic Peace.ttf", buttonText, 48, 200, 100);
        init(width, height, xPos, yPos, windowWidth, windowHeight);
}

void UIButton::init(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight) {
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

    this->uiMesh = std::make_shared<UIMesh>(vertices, uvs);
    this->buttonShader = std::make_shared<Shader>("src/shaders/uielement.vs", "src/shaders/uielement.fs");
}

void UIButton::render(float deltaTime, float curTime) {
    if(hovering && !hoverStarted) {
        hoverStarted = true;
        hoverStartTime = curTime;
    } else if(!hovering) {
        hoverStartTime = 0;
        hoverStarted = false;
    }
    this->buttonShader->use();
    this->buttonShader->setMat4("projection", glm::ortho(0.0f, 800.0f, 0.0f, 600.0f));
    this->buttonShader->setVec4("inColor", this->backgroundColor);
    if(this->hoverStartTime > 0) {
        this->buttonShader->setFloat("dHoverTime", curTime - this->hoverStartTime);
    } else {
        this->buttonShader->setFloat("dHoverTime", 0);
    }
    this->buttonShader->setFloat("animTime", .25);
    uiMesh->draw(*buttonShader); // also subtract width of total text
    glm::vec2 textDims = this->buttonTextElement->getDims();
    this->buttonTextElement->setPos(glm::vec2((this->xPos + (this->width / 2)) - (textDims.x / 4), this->yPos + (this->height / 2))); 
    this->buttonTextElement->render(1, glm::vec3(0, 1.0, 0), curTime);
}

glm::vec2 UIButton::getPos() {
    return glm::vec2(xPos, yPos);
}

glm::vec2 UIButton::getDims() {
    return glm::vec2(width, height);
}

void UIButton::updateWindowSize(int newWindowWidth, int newWindowHeight) {

}

void UIButton::setClickEvent(void(*clickEvent)()) {
    this->click = clickEvent;
}
