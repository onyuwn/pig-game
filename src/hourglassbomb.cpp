#include "hourglassbomb.hpp"

HourGlassBomb::HourGlassBomb(std::string name, glm::vec3 position, std::shared_ptr<Model> itemModel,
                         std::shared_ptr<Shader> itemShader, float scale, std::shared_ptr<Shader> outlineShader,
                         glm::vec3 holdingScaleFactor)
                         : Item(name, position, itemModel, itemShader, scale, outlineShader, holdingScaleFactor) {
    this->selected = false;
    this->range = 100;
    this->explosionDamage = 10;
    this->bombActive = false;
    this->timeToExplosion = 5;
}

void HourGlassBomb::initializeAOE() {
    this->bombAreaOfEffectShape = new btSphereShape(3.0);
    this->areaOfEffect = new btCollisionObject();
    this->areaOfEffect->setCollisionShape(this->bombAreaOfEffectShape);
}

void HourGlassBomb::use() {
    // throw as if dropped but activate bomb timer then remove from world
    // i think player will handle throw but count down here?
    this->bombActive = true;
}

void HourGlassBomb::render(float deltaTime, glm::mat4 model,
                  glm::mat4 view, glm::mat4 projection,
                  float curTime, glm::vec3 sceneLightPos) {
    if(this->selected) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0x00);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }

    if(this->itemJustHeld == true && this->itemHeld == false) {
        this->addToWorld(this->physWorld); // TODO maybe implement "collision layers" (use different phys worlds)
        this->itemJustHeld = false;
    }

    this->itemShader->use();
    this->itemShader->setFloat("sceneTime", curTime);
    this->itemShader->setFloat("ambientStrength", 1.0);
    this->itemShader->setMat4("projection", projection);
    this->itemShader->setMat4("view", view);
    this->itemShader->setVec3("lightPos", sceneLightPos);
    this->itemShader->setFloat("hitTime", 0.0);

    if(this->bombActive) {
        glm::vec3 currentBombPos = this->getPos();
        btTransform currentTransform = this->areaOfEffect->getWorldTransform();
        currentTransform.setOrigin(btVector3(currentBombPos.x, currentBombPos.y, currentBombPos.z));
        this->areaOfEffect->setWorldTransform(currentTransform);
        float countDown = curTime - explosionStart;
        this->itemShader->setFloat("hitTime", countDown);
        if(countDown >= this->timeToExplosion) {
            //boom
        }
    }

    glm::mat4 itemModelMatrix = glm::mat4(1.0);
    if(this->itemHeld) {
        this->position = this->positionCallback();
        itemModelMatrix = this->parentTransformCallback();
        //itemModelMatrix = glm::translate(itemModelMatrix, this->positionCallback());
        // itemModelMatrix *= glm::inverse(
        //     glm::lookAt(
        //         glm::vec3(0.0),
        //         this->forwardCallback(),
        //         glm::vec3(0.0,1.0,0)
        //     )
        // );
        if(this->itemJustHeld == true) {
            this->physWorld->removeRigidBody(this->itemRigidBody->entityRigidBody);
            this->itemJustHeld = false;
        }
        itemModelMatrix = glm::scale(itemModelMatrix, glm::vec3(this->scale * this->holdingScaleFactor));
        //itemModelMatrix = glm::translate(itemModelMatrix, this->position);
    } else {
        itemModelMatrix = itemRigidBody->render(itemModelMatrix, false);
        itemModelMatrix = glm::scale(itemModelMatrix, glm::vec3(this->scale));
    }
    this->itemShader->setMat4("model", itemModelMatrix);
    this->itemShader->setFloat("opacity", 1.0);
    this->itemModel->draw(*this->itemShader, curTime);

    if(this->selected) {
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
    this->selected = false;
}

ItemUseType HourGlassBomb::getItemUseType() {
    return ItemUseType::THROW;
}