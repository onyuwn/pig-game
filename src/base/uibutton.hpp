#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include "UIElement.hpp"
#include "UIMesh.hpp"
#include "uitextelement.hpp"
#include "shader.hpp"
#include <string>
#include <glm/glm.hpp>
#include <vector>

class UIButton : public UIElement {
    public:
        UIButton(int width, int height, int xPos, int yPos,
                 float windowWidth, float windowHeight, glm::vec4 backgroundColor,
                 std::string buttonText, void(*clickEvent)());
        void init(int width, int height, int xPos, int yPos, float windowWidth, float windowHeight) override;
        void render(float deltaTime, float curTime) override;
        glm::vec2 getPos() override;
        glm::vec2 getDims() override;
        void updateWindowSize(int newWindowWidth, int newWindowHeight) override;
        void setClickEvent(void(*clickEvent)());
        void(*click)();
        bool hovering;
        float hoverStartTime;
    private:
        int width;
        int height;
        int xPos;
        int yPos;
        int windowWidth;
        int windowHeight;
        bool hoverStarted;
        glm::vec4 backgroundColor;
        std::string buttonText;
        std::shared_ptr<UITextElement> buttonTextElement;
        std::shared_ptr<Shader> buttonShader;
        std::shared_ptr<UIMesh> uiMesh;
};

#endif