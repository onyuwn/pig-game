#include "piggy.hpp"

Piggy::Piggy(std::string name, glm::vec3 position, float scale)
    : name(name), interacting(false) {
    this->initialized = false;
    this->isHit = false;
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelMatrix = model;
    this->health=100;
    pigExploded = false;
    this->shatterPiecesInPhysWorld=0;
    this->scale=scale;
    this->initialPosition=position;
}

void Piggy::initialize() {
    this->initialized=true;
    this->piggyModel = std::make_shared<Model>((char*)"resources/piggyiso.obj");
    this->shatteredPigModel = std::make_shared<Model>((char*)"resources/pig/pigshatter4.gltf");
    this->piggyShader = std::make_shared<Shader>("src/shaders/basic.vs", "src/shaders/basic.fs");
    this->piggyRigidBody = new RigidBodyEntity(this->piggyModel, btVector3(this->initialPosition.x,this->initialPosition.y,this->initialPosition.z), BOX, 0.5f, btVector3(1.0, 1.0, 1.0), this->scale);
    auto& shatterings = this->shatteredPigModel->getMeshes();
    for(int i = 0; i < shatterings.size(); i++) {
        Mesh *curMesh = &shatterings[i];
        std::shared_ptr<RigidBodyEntity> pigPiece = std::make_shared<RigidBodyEntity>(curMesh, btVector3(this->initialPosition.x,this->initialPosition.y,this->initialPosition.z), BOX, .5f, btVector3(1.0, 1.0, 1.0), this->scale);
        pigPiece->initialize(this->modelMatrix);
        this->shatteredPigRigidBodies.push_back(pigPiece);
    }
    this->piggyRigidBody->initialize(this->modelMatrix);
}

void Piggy::addToWorld(btDiscreteDynamicsWorld *world) {
    if(initialized) {
        this->piggyRigidBody->addToWorld(world);
        this->piggyRigidBody->entityRigidBody->setUserPointer(this);
        this->physWorld = world;
    }
}

void Piggy::render(float deltaTime, glm::mat4 model, glm::mat4 view, glm::mat4 projection, float curTime, glm::vec3 sceneLightPos) {
    this->piggyShader->use();
    this->piggyShader->setFloat("sceneTime", curTime);
    this->piggyShader->setFloat("hitTime", hitTime);
    this->piggyShader->setFloat("ambientStrength", 1.0);
    this->piggyShader->setMat4("projection", projection);
    this->piggyShader->setMat4("view", view);
    this->piggyShader->setVec3("lightPos", sceneLightPos);
    this->piggyShader->setVec3("lightColor", glm::vec3(1.0));
    if(this->interacting) { // position override
        glm::vec3 direction = glm::normalize(this->positionCallback() - this->getPos());
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), this->getPos());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
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
            newPiggyModelMatrix = glm::scale(newPiggyModelMatrix, glm::vec3(scale));
            this->piggyShader->setMat4("model", newPiggyModelMatrix);
            this->piggyModel->draw(*this->piggyShader, curTime);
        } else if(!pigExploded || (pigExploded && curTime - hitTime <= 2)) {
            if(piggyRigidBody != nullptr) {
                piggyRigidBody->entityRigidBody->activate(false);
                if(this->physWorld != nullptr) {
                    this->physWorld->removeRigidBody(piggyRigidBody->entityRigidBody);
                }
            }
            for(int i = 0; i < this->shatteredPigModel->getMeshes().size(); i++) {
                
                if(shatterPiecesInPhysWorld < i + 1) {
                    this->shatteredPigRigidBodies[i]->addToWorld(this->physWorld);
                    this->shatteredPigRigidBodies[i]->entityRigidBody->activate(true);
                    glm::vec3 offset = glm::vec3(0.0);
                    if(i % 2 == 0) {
                        offset = glm::vec3(-2,2,-2);
                    } else if(i % 3 == 0) {
                        offset = glm::vec3(2,2,2);
                    } else {
                        offset = glm::vec3(-2,2,2);
                    }
                    this->shatteredPigRigidBodies[i]->setPos(this->piggyRigidBody->getPos() + offset);
                    shatterPiecesInPhysWorld++;
                }

                glm::mat4 newPiggyModelMatrix = this->shatteredPigRigidBodies[i]->render(glm::mat4(1.0), false);
                newPiggyModelMatrix = glm::scale(newPiggyModelMatrix, glm::vec3(scale));
                glm::vec3 throwingForce = glm::vec3(0.0);
                if(i % 2 == 0) {
                    throwingForce = glm::vec3(2,7,5 + i);
                } else if(i % 3 == 0) {
                    throwingForce = glm::vec3(i,10,3);
                } else {
                    throwingForce = glm::vec3(5 + i,7,2 + i);
                }
                btVector3 btForce = btVector3(throwingForce.x, throwingForce.y, throwingForce.z);
                if(!pigExploded) {
                    this->shatteredPigRigidBodies[i]->entityRigidBody->setCollisionFlags(0);
                    this->shatteredPigRigidBodies[i]->entityRigidBody->applyCentralImpulse(btForce);
                    hitTime = curTime;
                    pigExploded = true;
                }
                this->piggyShader->setMat4("model", newPiggyModelMatrix);
                this->piggyShader->setFloat("opacity", 1.0 - (2.0 / (curTime - hitTime)));
                this->shatteredPigModel->getMeshes()[i].draw(*this->piggyShader);
            }
        }

        if(pigExploded && curTime - hitTime > 2) {
            for(int i = 0; i < this->shatteredPigModel->getMeshes().size(); i++) {
                this->shatteredPigRigidBodies[i]->entityRigidBody->activate(false);
                if(this->physWorld != nullptr) {
                    this->physWorld->removeRigidBody(this->shatteredPigRigidBodies[i]->entityRigidBody);
                }
            }
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

void Piggy::setScale(float scale) {
    this->scale = scale;
}