#include "item.hpp"

Item::Item(std::string name, glm::vec3 position, std::shared_ptr<Model> itemModel,
            std::shared_ptr<Shader> itemShader, float scale, std::shared_ptr<Shader> outlineShader) {
    this->name = name;
    this->position = position;
    this->itemModel = itemModel;
    this->itemShader = itemShader;
    this->outlineShader = outlineShader;
    this->scale = scale;
}

void Item::addToWorld(btDiscreteDynamicsWorld *world) {
    if(initialized) {
        this->itemRigidBody->addToWorld(world);
        this->itemRigidBody->entityRigidBody->setUserPointer(this);
        this->physWorld = world;
    }
}

void Item::initialize() {
    this->initialized = true;
    this->itemRigidBody = new RigidBodyEntity(
        this->itemModel, btVector3(this->position.x,this->position.y,this->position.z), BOX, 0.5f, btVector3(1.0, 1.0, 1.0), this->scale
    );
    this->itemRigidBody->initialize(glm::mat4(1.0));
}

void Item::render(float deltaTime, glm::mat4 model,
                  glm::mat4 view, glm::mat4 projection,
                  float curTime, glm::vec3 sceneLightPos) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0x00);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    this->itemShader->use();
    this->itemShader->setFloat("sceneTime", curTime);
    this->itemShader->setFloat("ambientStrength", 1.0);
    this->itemShader->setMat4("projection", projection);
    this->itemShader->setMat4("view", view);
    this->itemShader->setVec3("lightPos", sceneLightPos);
    glm::mat4 itemModelMatrix = itemRigidBody->render(glm::mat4(1.0), false);
    itemModelMatrix = glm::scale(itemModelMatrix, glm::vec3(this->scale));
    this->itemShader->setMat4("model", itemModelMatrix);
    this->itemShader->setFloat("opacity", 1.0);
    this->itemModel->draw(*this->itemShader, curTime);

    glEnable(GL_CULL_FACE);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    this->outlineShader->use();
    glm::mat4 finalItemModelMatrix = glm::scale(itemModelMatrix, glm::vec3(1.15, 1.15, 1.15));
    this->outlineShader->setMat4("model", finalItemModelMatrix);
    this->outlineShader->setMat4("view", view);
    this->outlineShader->setMat4("projection", projection);
    this->outlineShader->setFloat("curTime", curTime);
    this->itemModel->draw(*this->outlineShader);
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}


std::string Item::getHelpText() {
    return "GUN";
}

void Item::setPosition(glm::vec3 newPos) {
    this->position = newPos;
}

void Item::setPos(std::function<glm::vec3()> posCallback) {
}

glm::vec3 Item::getPos() {
    return this->position;
}

void Item::applyForce(glm::vec3 force) {
    // not needed
}

void Item::toggleRigidBody() {
    // not needed
}

void Item::toggleState() {
    
}

void Item::takeHit(int dmg) {

}

void Item::setScale(float scale) {

}

glm::vec3 Item::getForward() {
    return glm::vec3(1.0);
}

void Item::use() {

}

GameObjectInteractionType Item::getInteraction() {
    return COLLECT_ITEM;
}