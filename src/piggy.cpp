#include "piggy.hpp"

Piggy::Piggy(std::string name)
    : name(name), interacting(false) {
    this->initialized = false;
    this->isHit = false;
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelMatrix = model;
    this->health=100;
}

void Piggy::initialize() {
    this->initialized=true;
    this->piggyModel = std::make_shared<Model>((char*)"resources/piggyiso.obj");
    this->shatteredPigModel = std::make_shared<Model>((char*)"resources/pig/shatteredpig.gltf");
    this->piggyShader = std::make_shared<Shader>("src/shaders/basic.vs", "src/shaders/basic.fs");
    this->piggyRigidBody = new RigidBodyEntity(*this->piggyModel, btVector3(2,2,2), BOX, 10.0f);
    this->shatteredPigRigidBody = new RigidBodyEntity(*this->shatteredPigModel, btVector3(2,2,2), BOX, 1.0f);
    this->piggyRigidBody->initialize(this->modelMatrix);
    this->shatteredPigRigidBody->initialize(this->modelMatrix);
}

void Piggy::addToWorld(btDiscreteDynamicsWorld *world) {
    if(initialized) {
        this->piggyRigidBody->addToWorld(world);
        this->piggyRigidBody->entityRigidBody->setUserPointer(this);
        this->shatteredPigRigidBody->addToWorld(world);
        // this->shatteredPigRigidBody->entityRigidBody->setUserPointer(this);
        // this->shatteredPigRigidBody->entityRigidBody->activate(false);
    }
}

void Piggy::render(float deltaTime, glm::mat4 model, glm::mat4 view, glm::mat4 projection, float curTime) {
    this->piggyShader->use();
    this->piggyShader->setFloat("sceneTime", curTime);
    this->piggyShader->setFloat("hitTime", hitTime);
    this->piggyShader->setMat4("projection", projection);
    this->piggyShader->setMat4("view", view);
    if(this->interacting) { // position override
        glm::vec3 direction = glm::normalize(this->positionCallback() - this->getPos());
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), this->getPos());
        glm::vec3 currentForward = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 axis = glm::cross(currentForward, direction);
        float angle = acos(glm::dot(currentForward, direction));
        if (glm::length(axis) > 0.0001f) {
            axis = glm::normalize(axis);
            glm::mat4 rotationMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0,axis.y,0)); // important. probably a better way to do this with lookat
            modelMatrix = rotationMatrix * modelMatrix;
        }
        modelMatrix = glm::translate(modelMatrix, -this->getPos());
        piggyRigidBody->render(modelMatrix, true);
    } else {
        if(this->isHit==true) {
            this->isHit = false;
            hitTime = curTime;
        }
        if(this->health > 0) {
            glm::mat4 newPiggyModelMatrix = piggyRigidBody->render(glm::mat4(1.0), false);
            this->piggyShader->setMat4("model", newPiggyModelMatrix);
            this->piggyModel->draw(*this->piggyShader, curTime);
        } else {
            //this->shatteredPigRigidBody->entityRigidBody->activate(true);
            glm::mat4 newPiggyModelMatrix = shatteredPigRigidBody->render(glm::mat4(1.0), false);
            this->piggyShader->setMat4("model", newPiggyModelMatrix);
            this->shatteredPigModel->draw(*this->piggyShader, curTime);
        }
    }
}

GameObjectInteractionType Piggy::getInteraction() {
    return HIT;
}

void Piggy::setPos(std::function<glm::vec3()> posCallback) {
    this->positionCallback = posCallback; // used to look at player
}

glm::vec3 Piggy::getPos() {
    return this->piggyRigidBody->getPos();
}

void Piggy::applyForce(glm::vec3 force) {
    // not needed
}

void Piggy::toggleRigidBody() {
    // not needed
}

void Piggy::toggleState() {
    
}

std::string Piggy::getHelpText() {
    return "";
}

int Piggy::getHealth() {
    return health;
}

void Piggy::takeHit(int dmg) {
    this->health-=dmg;
    this->isHit=true;
}