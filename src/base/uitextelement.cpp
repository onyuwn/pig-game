#include "uitextelement.hpp"

UITextElement::UITextElement(std::string fontPath, std::string text, int fontSize, int xPos, int yPos)
     : textShader(textShader), x(xPos), y(yPos) {
    this->fontPath = fontPath;
    this->text = text;
    this->fontSize = fontSize;
    this->x = xPos;
    this->y = yPos;
    initText();
}

void UITextElement::initText() {
    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: could not init freetype library" << std::endl;
        return;
    }

    if(FT_New_Face(ft, this->fontPath.c_str(), 0, &this->fontFace)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl; 
        return; 
    }

    FT_Set_Pixel_Sizes(this->fontFace, 0, this->fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for(unsigned char c = 0; c < 128; c++) {
        if(FT_Load_Char(this->fontFace, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            this->fontFace->glyph->bitmap.width,
            this->fontFace->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            this->fontFace->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(this->fontFace->glyph->bitmap.width, this->fontFace->glyph->bitmap.rows),
            glm::ivec2(this->fontFace->glyph->bitmap_left, this->fontFace->glyph->bitmap_top),
            static_cast<unsigned int>(this->fontFace->glyph->advance.x)
        };

        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(this->fontFace);
    FT_Done_FreeType(ft);

    this->textShader = Shader("src/shaders/text.vs", "src/shaders/text.fs");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 
}

void UITextElement::setText(std::string text) {
    this->text = text;
}

void UITextElement::render(float scale, glm::vec3 color, float curTime) {
    glClear(GL_DEPTH_BUFFER_BIT); // Clears only the depth buffer otherwise skybox gets fucked up
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    this->textShader.use();
    this->textShader.setFloat("u_time", curTime);
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    this->textShader.setMat4("projection", projection);
    glUniform3f(glGetUniformLocation(this->textShader.shaderProgram, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    std::string::const_iterator c;
    unsigned int newlineCount = 1;
    int initialX = x;

    for(c = this->text.begin(); c != this->text.end(); c++) {
        Character ch = Characters[*c];

        if(*c == '\n') { 
            this->x = initialX;
            newlineCount++;
        }

        float xpos = this->x + ch.Bearing.x * scale;
        float ypos = this->y - ((ch.Size.y * newlineCount) - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f } 
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        this->x += (ch.Advance >> 6) * scale;
    }

    this->x = initialX;

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

glm::vec2 UITextElement::getDims() {
    int totalWidth = 0;
    int totalHeight = 0;
    std::string::const_iterator c;

    for(c = this->text.begin(); c != this->text.end(); c++) {
        Character ch = Characters[*c];
        totalWidth+=ch.Size.x;
        totalHeight+=ch.Size.y;
    }

    return glm::vec2(totalWidth, totalHeight);
}

glm::vec2 UITextElement::getPos() {
    return glm::vec2(this->x, this->y);
}

void UITextElement::setPos(glm::vec2 newPos) {
    this->x = newPos.x;
    this->y = newPos.y;
}