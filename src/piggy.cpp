#include "piggy.hpp"

Piggy::Piggy(std::string name, glm::vec3 position, float scale,
    std::shared_ptr<Model> _pigModel, std::shared_ptr<Model> _shatteredPigModel,
    std::shared_ptr<Shader> _pigShader)
    : name(name), interacting(false) {
    this->initialized = false;
    this->isHit = false;
    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->modelMatrix = model;
    this->health=20;
    pigExploded = false;
    this->shatterPiecesInPhysWorld=0;
    this->scale=scale;
    this->initialPosition=position;
    this->shouldBeDestroyed=false;
    this->rotation = glm::vec3(0.0);
    this->player=nullptr;
    this->piggyModel = _pigModel;
    this->shatteredPigModel = _shatteredPigModel;
    this->piggyShader = _pigShader;
    this->position = this->initialPosition;
}

void Piggy::initialize() {
    this->initialized=true;
    //this->piggyModel = std::make_shared<Model>((char*)"resources/piggyiso.obj");
    //this->shatteredPigModel = std::make_shared<Model>((char*)"resources/pig/shatteredpig.gltf");
    //this->piggyShader = std::make_shared<Shader>("src/shaders/basic.vs", "src/shaders/basic.fs");
    this->piggyRigidBody = new RigidBodyEntity(
        this->piggyModel, btVector3(this->initialPosition.x,this->initialPosition.y,this->initialPosition.z), BOX, 0.5f, btVector3(1.0, 1.0, 1.0), this->scale
    );
    auto shatterings = this->shatteredPigModel->getMeshes();
    for(auto& mesh : shatterings) {
        auto pigPiece = std::make_shared<RigidBodyEntity>(
            &mesh,
            btVector3(this->initialPosition.x,
                    this->initialPosition.y,
                    this->initialPosition.z),
            BOX,
            0.5f,
            btVector3(1.0, 1.0, 1.0),
            this->scale);

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
    if(this->isHit==true) {
        this->isHit = false;
        hitTime = curTime;
    }
    if(this->player != nullptr && this->health > 0) { // position override
        glm::vec3 piggyPos = this->getPos();
        glm::vec3 playerPos = this->player->getPlayerPos(); 
        // TODO: SET 4 TO GROUND HEIGHT??
        glm::vec3 direction = glm::normalize(glm::vec3(piggyPos.x, 4.0, piggyPos.z) - glm::vec3(playerPos.x, 4.0, playerPos.z));
        glm::vec3 newForward = direction; // duh
        this->forward = newForward;
        glm::vec3 movement = this->forward * deltaTime * 4.0f;
        this->position -= movement;
        //glm::mat4 newPiggyModelMatrix = glm::translate(glm::mat4(1.0), this->position);
        // teleport problem HEREEEughh  --- OMFG lookat rotates...and moves lmao
        glm::mat4 newPiggyModelMatrix = glm::inverse(
            glm::lookAt(glm::vec3(this->position.x, 4.0, this->position.z), direction + glm::vec3(this->position.x, 4.0, this->position.z), glm::vec3(0,1.0,0))
        );
        newPiggyModelMatrix = glm::scale(newPiggyModelMatrix, glm::vec3(scale));
        glm::vec4 forwardUnNormal = newPiggyModelMatrix * glm::vec4(0.0, 0.0, 1.0, 0.0);
        this->piggyRigidBody->setPos(this->position);
        glm::vec3 rigidBodyPos = this->piggyRigidBody->getPos();
        this->piggyShader->setMat4("model", newPiggyModelMatrix);
        this->piggyShader->setFloat("opacity", 1.0);
        this->piggyModel->draw(*this->piggyShader, curTime);
    } else {
        this->followingPlayer = false;
        this->rotation.y = curTime;
        if(this->health > 0) {
            glm::mat4 newPiggyModelMatrix = piggyRigidBody->render(glm::mat4(1.0), false);
            this->position = this->piggyRigidBody->getPos();
            newPiggyModelMatrix = glm::rotate(newPiggyModelMatrix, this->rotation.y, glm::vec3(0,1.0,0));
            glm::vec4 forwardUnNormal = newPiggyModelMatrix * glm::vec4(0.0, 0.0, 1.0, 0.0);
            this->forward = glm::normalize(glm::vec3(forwardUnNormal));
            bool playerSpotted = this->playerSpotted(glm::normalize(glm::vec3(forwardUnNormal)));
            if(playerSpotted && this->player != nullptr) {
                lastPlayerSpottedTime = curTime;
            }
            newPiggyModelMatrix = glm::scale(newPiggyModelMatrix, glm::vec3(scale));
            this->piggyModelMatrix = newPiggyModelMatrix;
            this->piggyShader->setMat4("model", newPiggyModelMatrix);
            this->piggyShader->setFloat("opacity", 1.0);
            this->piggyModel->draw(*this->piggyShader, curTime);
        } else if(!pigExploded || (pigExploded && curTime - hitTime < 3)) {
            for(int i = 0; i < this->shatteredPigModel->getMeshes().size(); i++) {
                if(shatterPiecesInPhysWorld < i + 1) { // add shattered pieces if not added already
                    this->shatteredPigRigidBodies[i]->addToWorld(this->physWorld);
                    this->shatteredPigRigidBodies[i]->entityRigidBody->activate(true);
                    glm::vec3 offset = glm::vec3(0.0);
                    if(i == 0) {
                        offset = glm::vec3(0.0);
                    } else if(i == 1) {
                        offset = this->forward * 4.0f;
                    } else {
                        offset = this->forward * -4.0f;
                    }
                    this->shatteredPigRigidBodies[i]->setPos(this->piggyRigidBody->getPos() + offset);
                    shatterPiecesInPhysWorld++;
                }

                glm::mat4 newPiggyModelMatrix = this->shatteredPigRigidBodies[i]->render(glm::mat4(1.0), false);
                newPiggyModelMatrix = glm::scale(newPiggyModelMatrix, glm::vec3(scale));
                glm::vec3 throwingForce = glm::vec3(0.0);
                if(i == 0) {
                    throwingForce = glm::vec3(0,10,0);
                } else if(i == 1) {
                    throwingForce = (this->forward * 10.0f) + glm::vec3(0.0,5.0,0.0);
                } else {
                    throwingForce = (-this->forward * 10.0f) + glm::vec3(0.0,5.0,0.0);
                }
                btVector3 btForce = btVector3(throwingForce.x, throwingForce.y, throwingForce.z);
                if(!pigExploded) {
                    this->shatteredPigRigidBodies[i]->entityRigidBody->setCollisionFlags(0);
                    this->shatteredPigRigidBodies[i]->entityRigidBody->applyCentralImpulse(btForce);
                    hitTime = curTime;
                    pigExploded = true;
                }
                this->piggyShader->setMat4("model", newPiggyModelMatrix);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                this->piggyShader->setFloat("opacity", 1.25 - ((curTime - hitTime) / 2.0));
                this->shatteredPigModel->getMeshes()[i].draw(*this->piggyShader);
            }

            if(!pigExploded || (pigExploded && curTime - hitTime <= 2)) { // remove base pig rigid body
                if(piggyRigidBody != nullptr) {
                    piggyRigidBody->entityRigidBody->activate(false);
                    if(this->physWorld != nullptr) {
                        this->physWorld->removeRigidBody(piggyRigidBody->entityRigidBody); // will null out rigidbody
                    }
                }
            }

            if(pigExploded && curTime - hitTime > 2) { // disable collision bodies for shattered pieces
                for(int i = 0; i < this->shatteredPigModel->getMeshes().size(); i++) {
                    this->shatteredPigRigidBodies[i]->entityRigidBody->activate(false);
                    this->shatteredPigRigidBodies[i]->entityRigidBody->setCollisionFlags(this->shatteredPigRigidBodies[i]->entityRigidBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
                    if(this->physWorld != nullptr && curTime - hitTime > 3) { // let fall through floor then remove entirely
                        this->physWorld->removeRigidBody(this->shatteredPigRigidBodies[i]->entityRigidBody);
                    }
                }
            }
        } else {
            shouldBeDestroyed = true;
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
    return this->position;
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

bool Piggy::playerSpotted(glm::vec3 forward) {
    glm::vec3 outOrigin = getPos();
    glm::vec3 outEnd = outOrigin + forward * (float)100.0;
    btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z));
    this->physWorld->rayTest(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z), RayCallback);
    //printf("forward: %f, %f, %f spotted? %s\n", forward.x, forward.y, forward.z, RayCallback.hasHit() ? "true": "false");
    if(RayCallback.hasHit() && followingPlayer == false) {
        this->player = (Player*)(RayCallback.m_collisionObject->getUserPointer());
        if(this->player != nullptr && this->player->name == "player") {
            bool isAlive = player->isAlive(); 
            this->player->notifySpotted();  
            followingPlayer = true;
        } else {
            this->player = nullptr;
        }
    }
    return RayCallback.hasHit();
}

void Piggy::setRotation(glm::vec3 newRotation) {
    this->rotation=newRotation;
}

glm::vec3 Piggy::getForward() {
    glm::vec3 forward;
    forward.x = cos(glm::radians(this->rotation.x)) * cos(glm::radians(this->rotation.y));
    forward.y = sin(glm::radians(this->rotation.y));
    forward.z = sin(glm::radians(this->rotation.x)) * cos(glm::radians(this->rotation.y));
    return glm::normalize(forward);
}
