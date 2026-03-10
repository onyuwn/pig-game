#ifndef UIPANEL_HPP
#define UIPANEL_HPP

#include "UIElement.hpp"
#include "UIMesh.hpp"
#include "uibutton.hpp"
#include "shader.hpp"
#include <glm/glm.hpp>
#include <vector>
#include "stb_image.h"

class UIPanel : public UIElement {
    public:
        UIPanel(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight, glm::vec4 backgroundColor);
        UIPanel(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight, std::string backgroundImagePath);
        void render(float deltaTime, float curTime) override;
        void init(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight) override;
        glm::vec2 getPos() override;
        glm::vec2 getDims() override;
        void updateWindowSize(int newWindowWidth, int newWindowHeight) override;
        void addButton(UIButton *button);
        std::vector<UIButton*> buttons;
    private:
        int width;
        int height;
        int xPos;
        int yPos;
        int windowWidth;
        int windowHeight;
        glm::vec4 backgroundColor;
        std::string backgroundImagePath;
        unsigned int backgroundImageTextureID;
        std::shared_ptr<Shader> uiPanelShader;
        std::shared_ptr<UIMesh> uiMesh;
};

#endif