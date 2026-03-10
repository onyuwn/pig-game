#include "billboardentity.hpp"

// todo: use quad not model

BillboardEntity::BillboardEntity(Model& entityModel, glm::vec3 position, glm::vec2 size) : entityModel(entityModel), billboardShader(billboardShader) {
    this->position = position;
    this->size = size;
    init();
}

void BillboardEntity::init() {
    this->billboardShader = Shader("src/shaders/billboard.vs", "src/shaders/billboard.fs");
}

void BillboardEntity::setPos(glm::vec3 pos) {
    this->position = pos;
}

void BillboardEntity::render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 up, glm::vec3 right, glm::vec3 playerPos) {
    this->billboardShader.use();
    this->billboardShader.setMat4("projection", projectionMatrix);
    this->billboardShader.setMat4("view", viewMatrix);
    this->billboardShader.setVec3("playerPos", playerPos);
    unsigned int cameraRightLoc = glGetUniformLocation(this->billboardShader.shaderProgram, "cameraRightWorldSpace");
    if(cameraRightLoc == -1) {
        printf("FAILED TO INIT CAMERARIGHT");
    }
    //glUniform3f(cameraRightLoc, viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
    glUniform3f(cameraRightLoc, right.x, right.y, right.z);

    unsigned int cameraUpLoc = glGetUniformLocation(this->billboardShader.shaderProgram, "cameraUpWorldSpace");
    if(cameraUpLoc == -1) {
        printf("FAILED TO INIT cameraUpLoc");
    }
    //glUniform3f(cameraUpLoc, viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
    glUniform3f(cameraUpLoc, up.x, up.y, up.z);

    unsigned int billboardPosLoc = glGetUniformLocation(this->billboardShader.shaderProgram, "billboardPos");
    if(billboardPosLoc == -1) {
        printf("FAILED TO INIT billboardPosLoc");
    }
    glUniform3f(billboardPosLoc, position.x, position.y, position.z);

    unsigned int billboardSizeLoc = glGetUniformLocation(this->billboardShader.shaderProgram, "billboardSize");
    if(billboardSizeLoc == -1) {
        printf("FAILED TO INIT billboardSizeLoc");
    }
    glUniform2f(billboardSizeLoc, size.x, size.y);
    this->entityModel.draw(this->billboardShader);
}