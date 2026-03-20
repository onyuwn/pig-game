#include "player.hpp"
#include "../basicpistol.hpp"
#include "../hourglassbomb.hpp"

Player::Player(Camera &camera, btDiscreteDynamicsWorld *world,
               UIMaster &uiCallback, bool &physDebugOn, std::string playerModelPath)
                : camera(camera), uiCallback(uiCallback), physDebugOn(physDebugOn),
                  playerModelPath(playerModelPath),
                  pauseRequested(false),
                  controlsDisabled(false), itemInRightHand(false), itemInLeftHand(false), name("player") {
    this->camera = camera;
    this->world = world;
        // build collision shape (box for rn)
    this->playerCollisionShape = new btCapsuleShape(4.0, 2.0);
    // create rigid body
    this->initialized = false;
    this->uiCallback = uiCallback;
    this->name="player";
    this->health = 100;
    this->leftHandItem = nullptr;
    this->rightHandItem = nullptr;
    this->clickRequested = false;
    this->rightClickRequested = false;
}

void Player::initialize() {
    glm::quat defaultQuat = glm::quat(0,0,0,0);
    glm::vec3 defaultPos = glm::vec3(3,0,0);

    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(btVector3(2, 2, 2));

    btDefaultMotionState* motionstate = new btDefaultMotionState(startTransform);

    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
        1,
        motionstate,
        this->playerCollisionShape,
        btVector3(0,2,0)
    );
    this->playerRigidBody = new btRigidBody(rigidBodyCI);
    this->playerRigidBody->activate(true);
    this->playerRigidBody->setFriction(.5);
    this->playerRigidBody->setDamping(.5f, 2.0f);
    this->name="player";
    this->playerRigidBody->setUserPointer(this);
    this->initialized=true;
    this->healthText = new UITextElement("resources/text/Angelic Peace.ttf", "X", 48, 0, 0);
    this->helpText = new UITextElement("resources/text/Angelic Peace.ttf", "", 24, -20, -100);
    this->healthText->setAnchorType(BOTTOM_RIGHT);
    this->helpText->setAnchorType(DEAD_CENTER);
    this->healthText->setText(std::to_string(this->health));
    this->helpText->setText("HELPTEST");
    this->uiCallback.addTextElement(this->healthText);
    this->uiCallback.addTextElement(this->helpText);

    this->playerShader = std::make_shared<Shader>("src/shaders/armaturegameobject.vs", "src/shaders/superbasic.fs");
    this->playerModel = std::make_shared<Model>((char*)playerModelPath.data());
    Animation* restAnim = new Animation((char*)"resources/character/arms2.gltf", this->playerModel.get(), 1);
    Animation* shootAnim = new Animation((char*)"resources/character/arms2.gltf", this->playerModel.get(), 3);
    Animation* shoveAnim = new Animation((char*)"resources/character/arms2.gltf", this->playerModel.get(), 5);
    Animation* throwLAnim = new Animation((char*)"resources/character/arms2.gltf", this->playerModel.get(), 7);
    Animation* throwRAnim = new Animation((char*)"resources/character/arms2.gltf", this->playerModel.get(), 9);
    this->animator = std::make_shared<Animator>(restAnim);
    this->animator->playAnimation(restAnim);
    animations.insert({"rest", restAnim});
    animations.insert({"shoot", shootAnim});
    animations.insert({"throwL", throwLAnim});
    animations.insert({"throwR", throwRAnim});
    animations.insert({"shove", shoveAnim});
    this->curAnim = restAnim;
}

void Player::render(float curTime, float deltaTime, glm::vec2 windowDims) {
    if(curAnim != nullptr && playingAnimation && curTime - animationStart < (curAnim->getDuration() / curAnim->getTicksPerSecond())) {
        this->animator->updateAnimation(deltaTime);
    } else {
        curAnim = this->animations["rest"];
        this->animator->playAnimation(curAnim);
        this->animator->updateAnimation(deltaTime);
    }
    this->healthText->setText(std::to_string(this->health));
    glm::mat4 model = glm::translate(glm::mat4(1.0), getPlayerPos() - glm::vec3(0, .25, 0)); // ARM ADJUST
    model = glm::scale(model, glm::vec3(.25, .25, .25));
    model *= glm::mat4(getPlayerRotationMatrix());

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom * 2.0f), (float)windowDims.x / (float)windowDims.y, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix(getPlayerPos());

    this->playerShader->use();
    auto transforms = this->animator->getFinalBoneMatrices();
    playerShader->setMat4("projection", projection);
    playerShader->setMat4("view", view);
    playerShader->setMat4("model", model);
    playerShader->setVec3("lightPos", this->getPlayerRightHandPos());
    playerShader->setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));
    for (int i = 0; i < transforms.size(); ++i) {
        this->playerShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    }
    this->playerModel->draw(*this->playerShader, curTime);
}

void Player::addToWorld(btDiscreteDynamicsWorld * world) {
    world->addRigidBody(this->playerRigidBody);
}

void Player::UpdatePlayer(float curTime, float deltaTime, GLFWwindow *window, bool &pauseCallback) {
    processInput(window, curTime, deltaTime, pauseCallback);
    pollInteractables();
}

void Player::processInput(GLFWwindow *window, float curTime, float deltaTime, bool &pauseCallback)
{ // this should just apply force to the rigid body, translate view matrix with rigid body pos through getter method in the main loop
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !pauseRequested) {
        pauseRequested = true;
    }

    if(pauseRequested && glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
        pauseRequested = false;
        pauseCallback = !pauseCallback;
        controlsDisabled = !controlsDisabled;

        if(pauseCallback) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !interactRequested) {
        this->interact(curTime);
        interactRequested = true;
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
        interactRequested = false;
    }
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        physDebugOn = true;
    } else {
        physDebugOn = false;
    }

    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && (itemInLeftHand || itemInRightHand)) { // throw that shit
         if(itemInLeftHand && leftHandItem != nullptr) {
            glm::vec3 throwingForce = this->camera.Front * 5.0f + glm::vec3(0,5,0);
            this->leftHandItem->applyForce(throwingForce);
            // apply force
            this->leftHandItem->itemJustHeld = true;
            this->leftHandItem->itemHeld = false;
            this->leftHandItem = nullptr;
            this->itemInLeftHand = false;
            curAnim = this->animations["throwL"];
            this->animator->playAnimation(curAnim);
            animationStart = curTime;
            playingAnimation = true;  
        } else if(itemInRightHand && rightHandItem != nullptr) {
            glm::vec3 throwingForce = this->camera.Front * 5.0f + glm::vec3(0,5,0);
            this->rightHandItem->applyForce(throwingForce);
            // apply force
            this->rightHandItem->itemJustHeld = true;
            this->rightHandItem->itemHeld = false;
            this->rightHandItem = nullptr;
            this->itemInRightHand = false;
            curAnim = this->animations["throwR"];
            this->animator->playAnimation(curAnim);
            animationStart = curTime;
            playingAnimation = true;
        }
    }

    //this->playerRigidBody->activate(true);
    float velocity = (this->camera.MovementSpeed * deltaTime);
    //std::cout << "VELOCITY: " << this->camera.MovementSpeed << " * " << deltaTime << " = " << velocity << std::endl;
    if(velocity < .1) { // TODO: player movement is effected by game performance
        velocity = .1;
    }
    btTransform curTransform = this->playerRigidBody->getWorldTransform();
    btVector3 curPos = curTransform.getOrigin();
    glm::vec3 cameraFrontNormal = glm::normalize(glm::vec3(this->camera.Front.x, 0, this->camera.Front.z));
    glm::vec3 cameraRightNormal = glm::normalize(glm::vec3(this->camera.Right.x, 0, this->camera.Right.z));
    btVector3 btFront = btVector3(cameraFrontNormal.x, 0, cameraFrontNormal.z); // todo use y component if grounded?
    btVector3 btRight = btVector3(cameraRightNormal.x, 0, cameraRightNormal.z);

    if(itemInLeftHand || itemInRightHand) {
        itemHeldTime = curTime - itemHoldStart;
    }

    btVector3 force(0, 0, 0);  // Reset force each frame

    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    this->uiCallback.setMousePos(xPos, (yPos - 600) * -1);

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // and menu open
        clickRequested = true;
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) { // and menu open
        rightClickRequested = true;
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && clickRequested) {
        if(pauseRequested) {
            this->uiCallback.checkClick(xPos, (yPos - 600) * -1);
        } else { // USE ITEM
            if(rightHandItem == nullptr && leftHandItem == nullptr) {
                this->shove(curTime);
            } else {
                this->useRightHandItem(curTime);
            }
        }
        clickRequested = false;
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && rightClickRequested) {
        if(pauseRequested) {
        } else { // USE ITEM
            
            if(rightHandItem == nullptr && leftHandItem == nullptr) {
                this->shove(curTime);
            } else {
                this->useLeftHandItem(curTime);
            }
        }
        rightClickRequested = false;
    }

    if(!controlsDisabled) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            this->playerRigidBody->setActivationState(1);
            this->playerRigidBody->activate(true);
            force += btFront * velocity * 50;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            this->playerRigidBody->setActivationState(1);
            this->playerRigidBody->activate(true);
            force += btFront * velocity * -50;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            this->playerRigidBody->setActivationState(1);
            this->playerRigidBody->activate(true);
            force += btRight * velocity * -50;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            this->playerRigidBody->setActivationState(1);
            this->playerRigidBody->activate(true);
            force += btRight * velocity * 50;
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && checkGrounded()) {
            this->playerRigidBody->setActivationState(1);
            this->playerRigidBody->activate(true);
            force += btVector3(0, 1, 0) * velocity * 2000;
        }

        // Apply force smoothly
        if (force.length() != 0 && checkGrounded()) {
            btVector3 currentVelocity = playerRigidBody->getLinearVelocity();
            btVector3 clampedVelocity = currentVelocity;
            float maxSpeed = 100.0f;  // Set the max horizontal speed
            if (clampedVelocity.x() > maxSpeed) {
                clampedVelocity.setX(maxSpeed);
            }
            if (clampedVelocity.x() < -maxSpeed) {
                clampedVelocity.setX(-maxSpeed);
            }
            if (clampedVelocity.z() > maxSpeed) {
                clampedVelocity.setZ(maxSpeed);
            }
            if (clampedVelocity.z() < -maxSpeed) {
                clampedVelocity.setZ(-maxSpeed);
            }

            // Set the vertical velocity (y) without clamping it
            clampedVelocity.setY(currentVelocity.y());

            // Apply the clamped velocity back to the rigid body (only affects horizontal speed)
            //playerRigidBody->setLinearVelocity(clampedVelocity);
            this->playerRigidBody->applyCentralForce(force);
        } else if(checkGrounded()) {
            btVector3 currentVelocity = playerRigidBody->getLinearVelocity();
            btScalar dampingFactor = 0.98f;  // Damping factor for smooth deceleration
            playerRigidBody->setLinearVelocity(currentVelocity * dampingFactor);
        } else { // in the air
            btVector3 currentVelocity = playerRigidBody->getLinearVelocity();
            btVector3 clampedVelocity = currentVelocity;
            float maxSpeed = 50.0f;  // Set the max horizontal speed
            if (clampedVelocity.x() > maxSpeed) {
                clampedVelocity.setX(maxSpeed);
            }
            if (clampedVelocity.x() < -maxSpeed) {
                clampedVelocity.setX(-maxSpeed);
            }
            if (clampedVelocity.z() > maxSpeed) {
                clampedVelocity.setZ(maxSpeed);
            }
            if (clampedVelocity.z() < -maxSpeed) {
                clampedVelocity.setZ(-maxSpeed);
            }

            // Set the vertical velocity (y) without clamping it
            clampedVelocity.setY(currentVelocity.y());
            // Apply the clamped velocity back to the rigid body (only affects horizontal speed)
            playerRigidBody->setLinearVelocity(clampedVelocity);
            this->playerRigidBody->applyCentralForce(force);
        }
    }
}

bool Player::checkGrounded() {
    glm::vec3 outOrigin = getPlayerPos();
    glm::vec3 outEnd = outOrigin + this->camera.WorldUp * (-6.0f - playerHeight);
    btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z));
    world->rayTest(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z), RayCallback);
    return RayCallback.hasHit();
}

void Player::interact(float curTime) {
    // cast ray, check if entity has interactable content
    glm::vec3 outOrigin = getPlayerPos();
    glm::vec3 outEnd = outOrigin + this->camera.Front * 100.0f;
    btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z));
    world->rayTest(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z), RayCallback);
    if(RayCallback.hasHit()) {
        GameObject* hitObject = (GameObject*)RayCallback.m_collisionObject->getUserPointer(); // todo cast to generic gameobject
        if(hitObject != nullptr) {
            GameObjectInteractionType interactionType = hitObject->getInteraction();
            if(interactionType == DIALOGUE) {
                // if interaction returns something....
                // std::string dialogLine = hitObject->getDialogueLine();
                // hitObject->setPos([this]() { return getPlayerPos(); });
                // if(dialogLine == "\0") {
                //     this->uiCallback.clearDialog();
                //     controlsDisabled = false;
                // } else {
                //     std::cout << dialogLine << std::endl;
                //     this->uiCallback.showDialog(dialogLine);
                //     controlsDisabled = true;
                // }
            } else if(interactionType == HOLD_ITEM && rightHandItem == nullptr) {
                hitObject->setPos([this]() { return getPlayerRightHandPos(); });
                ((Item*)hitObject)->setForward([this]() { return getForward(); });
                ((Item*)hitObject)->setParentTransform([this]() { return getPlayerRightHandTransform(); });
                ((Item*)hitObject)->itemHeld=true;
                ((Item*)hitObject)->itemJustHeld=true;
                this->itemHoldStart = curTime;
                this->itemInRightHand = true;
                this->rightHandItem = (Item*)hitObject;
            } else if(interactionType == HOLD_ITEM && leftHandItem == nullptr) {
                hitObject->setPos([this]() { return getPlayerLeftHandPos(); });
                ((Item*)hitObject)->setForward([this]() { return getForward(); });
                ((Item*)hitObject)->setParentTransform([this]() { return getPlayerLeftHandTransform(); });
                ((Item*)hitObject)->itemHeld=true;
                ((Item*)hitObject)->itemJustHeld=true;
                this->itemHoldStart = curTime;
                this->itemInLeftHand = true;
                this->leftHandItem = (Item*)hitObject;
            }
        } else {
            std::cout <<"NADA\n";
        }
    }
}

void Player::useRightHandItem(float curTime) {
    if(rightHandItem != nullptr) {
        if(rightHandItem->getItemUseType() == ItemUseType::SHOOT) {
            curAnim = this->animations["shoot"];
            this->animator->playAnimation(curAnim);
            playingAnimation = true;
            animationStart = curTime;
            BasicPistol* isPistol = dynamic_cast<BasicPistol*>(rightHandItem);
            if(isPistol != nullptr) {
                ((BasicPistol*)rightHandItem)->use();
            }
        } else if(rightHandItem->getItemUseType() == ItemUseType::THROW) {
            glm::vec3 throwingForce = this->camera.Front * 5.0f + glm::vec3(0,5,0);
            this->rightHandItem->applyForce(throwingForce);
            // apply force
            this->rightHandItem->itemJustHeld = true;
            this->rightHandItem->itemHeld = false;
            this->itemInRightHand = false;
            curAnim = this->animations["throwR"];
            this->animator->playAnimation(curAnim);
            animationStart = curTime;
            playingAnimation = true;
            HourGlassBomb* isBomb = dynamic_cast<HourGlassBomb*>(rightHandItem);
            if(isBomb != nullptr) {
                ((HourGlassBomb*)rightHandItem)->use();
            }
            this->rightHandItem = nullptr;
        }
    }
}

void Player::useLeftHandItem(float curTime) {
    if(leftHandItem != nullptr) {
        if(leftHandItem->getItemUseType() == ItemUseType::SHOOT) {
            curAnim = this->animations["shoot"];
            this->animator->playAnimation(curAnim);
            playingAnimation = true;
            animationStart = curTime;
            leftHandItem->use();
        } else if(leftHandItem->getItemUseType() == ItemUseType::THROW) {
            glm::vec3 throwingForce = this->camera.Front * 5.0f + glm::vec3(0,5,0);
            this->leftHandItem->applyForce(throwingForce);
            // apply force
            this->leftHandItem->itemJustHeld = true;
            this->leftHandItem->itemHeld = false;
            this->itemInLeftHand = false;
            curAnim = this->animations["throwR"];
            this->animator->playAnimation(curAnim);
            animationStart = curTime;
            playingAnimation = true;
            HourGlassBomb* isBomb = dynamic_cast<HourGlassBomb*>(leftHandItem);
            if(isBomb != nullptr) {
                ((HourGlassBomb*)leftHandItem)->use();
            }
            this->leftHandItem = nullptr;
        }
    }
}

void Player::shove(float curTime) {
    curAnim = this->animations["shove"];
    this->animator->playAnimation(curAnim);
    playingAnimation = true;
    animationStart = curTime;
    glm::vec3 outOrigin = getPlayerPos();
    glm::vec3 outEnd = outOrigin + this->camera.Front * 5.0f;
    btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z));
    world->rayTest(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z), RayCallback);
    if(RayCallback.hasHit()) {
        GameObject* hitObject = (GameObject*)RayCallback.m_collisionObject->getUserPointer();
        if(hitObject != nullptr) {
            hitObject->applyForce(this->getForward() * 4.0f + glm::vec3(0.0, 10.0, 0.0));
        }
    } else {
        this->helpText->setText("");
    }
}

void Player::setSelected(bool selected) {
    this->selected = selected;
}

void Player::pollInteractables() {
    glm::vec3 outOrigin = getPlayerPos() + (this->getForward() * 3.0f); // collider of item in hand getting in the way -- removing held items from simulation
    glm::vec3 outEnd = outOrigin + this->camera.Front * 100.0f;
    btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z));
    world->rayTest(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z), RayCallback);
    if(RayCallback.hasHit()) {
        // set gameobject as selected
        GameObject* hitObject = (GameObject*)RayCallback.m_collisionObject->getUserPointer();
        if(hitObject != nullptr) {
            hitObject->setSelected(true);
            this->helpText->setText(hitObject->getHelpText());
            // render help text to screen
        }
    } else {
        this->helpText->setText("");
    }
}

glm::vec3 Player::getPlayerPos() {
    btTransform curTransform = this->playerRigidBody->getWorldTransform();
    btVector3 curPos = curTransform.getOrigin();
    //btQuaternion curRot = curTransform.getRotation();
    return glm::vec3(curPos.x(), curPos.y() + playerHeight, curPos.z());
}

glm::vec3 Player::getPlayerRightHandPos() {
    return this->getPlayerPos() + ((glm::vec3)this->animator->getGlobalBoneTransform("Bone.003.L")[3] * -this->getForward());
}

glm::mat4 Player::getPlayerRightHandTransform() {
    glm::mat4 model = glm::translate(glm::mat4(1.0), getPlayerPos()); // ARM ADJUST
    //model = glm::scale(model, glm::vec3(.25, .25, .25));
    model *= glm::mat4(getPlayerRotationMatrix());
    return model * glm::translate(glm::mat4(1.0), (glm::vec3)this->animator->getGlobalBoneTransform("Bone.003.R")[3]) * glm::translate(glm::mat4(1.0), glm::vec3(-0.75, 0.25, 1.25));
    //return model * glm::translate(glm::mat4(1.0), (glm::vec3)this->animator->getGlobalBoneTransform("Bone.003.L")[3]) * glm::mat4(getPlayerRotationMatrix()) * glm::translate(glm::mat4(1.0), glm::vec3(-0.25, 0.0, 1.25));
}

glm::mat4 Player::getPlayerLeftHandTransform() {
    glm::mat4 model = glm::translate(glm::mat4(1.0), getPlayerPos()); // ARM ADJUST
    model *= glm::mat4(getPlayerRotationMatrix());
    return model * glm::translate(glm::mat4(1.0), (glm::vec3)this->animator->getGlobalBoneTransform("Bone.003.L")[3]) * glm::translate(glm::mat4(1.0), glm::vec3(.5, 0.25, 1.25));
}

glm::vec3 Player::getPlayerLeftHandPos() {
    return this->getPlayerPos() + (glm::vec3)this->animator->getGlobalBoneTransform("Bone.003.R")[3]; 
}

glm::mat3 Player::getPlayerRotationMatrix() {
    return glm::mat3(this->camera.Right, this->camera.Up, -this->camera.Front);
}

bool Player::isAlive() {
    return this->getPlayerPos().y > -20.0f;
}

bool Player::isControlDisabled() {
    return this->controlsDisabled;
}

void Player::render(float deltaTime, glm::mat4 model, glm::mat4 view, glm::mat4 projection, float curTime, glm::vec3 sceneLightPos) {

}

void Player::setPos(std::function<glm::vec3()> posCallback) {

}

void Player::applyForce(glm::vec3 force) {
    btVector3 btForce = btVector3(force.x, force.y, force.z);
    this->playerRigidBody->applyCentralImpulse(btForce);
}

void Player::takeHit(int dmg) {
    this->health -= dmg;
    this->healthText->setText(std::to_string(this->health));
}

void Player::setScale(float scale) {

}

void Player::toggleRigidBody() {

}

void Player::toggleState() {

}

std::string Player::getHelpText() {
    return "player";
}

glm::vec3 Player::getForward() {
    return this->camera.Front;
}

glm::vec3 Player::getRight() {
    return this->camera.Right;
}

GameObjectInteractionType Player::getInteraction() {
    return HIT;
}

void Player::notifySpotted() {
    this->uiCallback.addImagePanel(64, 64, this->aggroCount * 64, 0, "resources/eyetest1.png");
    this->aggroCount++;
}
