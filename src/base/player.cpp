#include "player.hpp"

Player::Player(Camera &camera, btDiscreteDynamicsWorld *world,
               UIMaster &uiCallback, bool &physDebugOn, std::string playerModelPath)
                : camera(camera), uiCallback(uiCallback), physDebugOn(physDebugOn),
                  playerModelPath(playerModelPath),
                  pauseRequested(false),
                  controlsDisabled(false), itemInHand(false), name("player") {
    this->camera = camera;
    this->world = world;
        // build collision shape (box for rn)
    this->playerCollisionShape = new btCapsuleShape(1.0, 2.0);
    // create rigid body
    this->initialized = false;
    this->uiCallback = uiCallback;
    this->name="player";
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

    this->playerShader = std::make_shared<Shader>("src/shaders/superbasic.vs", "src/shaders/superbasic.fs");
    this->playerModel = std::make_shared<Model>((char*)playerModelPath.data());
}

void Player::render(float curTime, float deltaTime, glm::vec2 windowDims) {
    glm::mat4 model = glm::translate(glm::mat4(1.0), getPlayerPos() - glm::vec3(0, .25, 0)); // ARM ADJUST
    model = glm::scale(model, glm::vec3(.25, .25, .25));
    model *= glm::mat4(getPlayerRotationMatrix());

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom * 2.0f), (float)windowDims.x / (float)windowDims.y, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix(getPlayerPos());

    this->playerShader->use();
    playerShader->setMat4("projection", projection);
    playerShader->setMat4("view", view);
    playerShader->setMat4("model", model);
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

    btVector3 force(0, 0, 0);  // Reset force each frame

    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    this->uiCallback.setMousePos(xPos, (yPos - 600) * -1);

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // and menu open
        clickRequested = true;
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && clickRequested) {
        this->uiCallback.checkClick(xPos, (yPos - 600) * -1);
        clickRequested = false;
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
    glm::vec3 outEnd = outOrigin + this->camera.WorldUp * (-2.25f - playerHeight);
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
    if(RayCallback.hasHit() && !itemInHand) {
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
            } else if(interactionType == HOLD_ITEM) {
                hitObject->setPos([this]() { return getPlayerHandPos(); });
                this->itemInHand = true;
                this->heldItem = hitObject;
            } else if(interactionType == THROW_ITEM) {
                hitObject->applyForce(getPlayerHandPos()); // need to reactivate the rigid body of the interactive entity
            } else if(interactionType == TOGGLE) {
                hitObject->toggleState();
            } else if(interactionType == HIT) {
                std::cout <<"HIT\n";
                hitObject->takeHit(10);
            }
        } else {
            std::cout <<"NADA\n";
        }
    } else if(itemInHand) { // throw that shit
        this->heldItem->toggleRigidBody();
        glm::vec3 throwingForce = this->camera.Front * 5.0f + glm::vec3(0,5,0);
        this->heldItem->applyForce(throwingForce);
        // apply force
        this->heldItem = nullptr;
        itemInHand = false;
    }
}

void Player::pollInteractables() {
    glm::vec3 outOrigin = getPlayerPos();
    glm::vec3 outEnd = outOrigin + this->camera.Front * 100.0f;
    btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z));
    world->rayTest(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z), RayCallback);
    if(RayCallback.hasHit() && !itemInHand) {
        // set gameobject as selected
        GameObject* hitObject = (GameObject*)RayCallback.m_collisionObject->getUserPointer();
        if(hitObject != nullptr) {
            hitObject->selected = true;
            // render help text to screen
        }
    }
}

glm::vec3 Player::getPlayerPos() {
    btTransform curTransform = this->playerRigidBody->getWorldTransform();
    btVector3 curPos = curTransform.getOrigin();
    //btQuaternion curRot = curTransform.getRotation();
    return glm::vec3(curPos.x(), curPos.y() + playerHeight, curPos.z());
}

glm::vec3 Player::getPlayerHandPos() {
    return getPlayerPos() + this->camera.Front * 2.0f;
}

glm::mat3 Player::getPlayerRotationMatrix() {
    return glm::mat3(this->camera.Right, this->camera.Up, this->camera.Front);
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

void Player::applyForce(glm::vec3) {

}

void Player::takeHit(int dmg) {

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

GameObjectInteractionType Player::getInteraction() {
    return HIT;
}

void Player::notifySpotted() {
    this->uiCallback.addImagePanel(64, 64, this->aggroCount * 64, 0, "resources/eyetest1.png");
    this->aggroCount++;
}
