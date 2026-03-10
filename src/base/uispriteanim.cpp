#include "uispriteanim.hpp"

UISpriteAnim::UISpriteAnim(std::string textureAtlasPath, int frameWidth, int frameHeight, int xPos, int yPos)
    : textureAtlasPath(textureAtlasPath), frameWidth(frameWidth), frameHeight(frameHeight), xPos(xPos), yPos(yPos),
      initialized(false), uiMesh(uiMesh), uiShader(uiShader) {
    initialize();
}

void UISpriteAnim::initialize() {
    float topLeftX = xPos;
    float topLeftY = yPos;

    float topRightX = xPos + frameWidth;
    float topRightY = yPos;

    float bottomLeftX = xPos;
    float bottomLeftY = yPos + frameHeight;

    float bottomRightX = xPos + frameWidth;
    float bottomRightY = yPos + frameHeight;

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

    int nChannels;
    std::cout << "loading sprite anim: " << textureAtlasPath << std::endl;
    unsigned char *data = stbi_load(textureAtlasPath.c_str(), &atlasWidth, &atlasHeight, &nChannels, 0);

    if(data) {
        glGenTextures(1, &textureAtlasID);
        glBindTexture(GL_TEXTURE_2D, textureAtlasID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlasWidth, atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        rows = atlasHeight / frameHeight;
        columns = atlasWidth / frameWidth;
        std::cout << "ROWS: " << rows << " COLUMNS " << columns << std::endl;
        std::cout << "fWIDTH: " << atlasWidth << " fHEIGHT " << atlasHeight << std::endl;
        Shader uiShader("src/shaders/uitextureelement.vs", "src/shaders/uispriteanim.fs");
        this->uiShader = uiShader;
        this->uiMesh = UIMesh(vertices, uvs);
        this->initialized = true;
    }
    else
    {
        std::cout << "UISpriteAnim:: Failed to load texture atlas.";
    }
}

void UISpriteAnim::render(float deltaTime, SpriteAnimationType animationType) {
    this->uiShader.use();
    curAnimType = animationType;
    if(this->initialized) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureAtlasID);
        uiShader.setInt("uiTex", 0);
        if(timeSinceLastFrame2 >= timePerFrame2) {
            timeSinceLastFrame2 = 0;
            if(curFrame < (columns*rows) && animationType == LOOP) {
                curFrame++;
            } else if(curFrame < (columns*rows) - 1 && animationType == ONCE_STOP) {
                curFrame++;
            } else if(curFrame > 0 && animationType == REVERSE_DONE) {
                curFrame--;
            }
            int curRow = curFrame / columns;
            int curCol = curFrame - (columns * curRow);
            std::cout << "curFrame: " << curFrame << std::endl;
            this->uiShader.setVec2("atlasSize", glm::vec2(atlasWidth, atlasHeight));
            this->uiShader.setVec2("frameSize", glm::vec2(frameWidth, frameHeight));
            this->uiShader.setInt("curRow", curRow);
            this->uiShader.setInt("curCol", curCol);
            if(curFrame >= (columns*rows) && animationType == LOOP) {
                curFrame = 0;
            } else if(curFrame >= (columns*rows) && animationType == ONCE_STOP) {
                curFrame = (columns*rows) - 1;
            }
        }
        uiShader.setMat4("projection", glm::ortho(0.0f, 800.0f, 0.0f, 600.0f));
        uiMesh.draw(uiShader);
        glBindTexture(GL_TEXTURE_2D, 0);
        timeSinceLastFrame2+=deltaTime;
    }
}

glm::vec2 UISpriteAnim::getPos() {
    return glm::vec2(xPos, yPos);
}

glm::vec2 UISpriteAnim::getDims() {
    return glm::vec2(frameWidth, frameHeight);
}

bool UISpriteAnim::animComplete() {
    if(curAnimType == ONCE_STOP && curFrame == (columns*rows)) {
        return true;
    } else if (curAnimType == REVERSE_DONE && curFrame <= 0) {
        return true;
    } else {
        return false;
    }
}