#ifndef UITEXTELEMENT_HPP
#define UITEXTELEMENT_HPP

#include <string>
#include <iostream>
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.hpp"
#include "uielementanchortype.hpp"

class UITextElement {
    public:
        UITextElement(std::string fontPath, std::string text, int fontSize, int xPos, int yPos);
        void initText();
        void render(float scale, glm::vec3 color, float curTime, glm::vec2 windowDims);
        void setText(std::string text);

        struct Character {
            unsigned int TextureID;
            glm::ivec2 Size;
            glm::ivec2 Bearing;
            unsigned int Advance;
        };

        glm::vec2 getDims();
        glm::vec2 getPos();
        void setPos(glm::vec2 newPos);
        void setAnchorType(UIElementAnchorType anchorType);
    private:
        std::string text;
        std::string fontPath;
        FT_Face fontFace;
        int fontSize;
        std::map<char, Character> Characters;
        Shader textShader;
        unsigned int VAO, VBO;
        int x;
        int y;
        UIElementAnchorType anchorType = TOP_LEFT;
};

#endif