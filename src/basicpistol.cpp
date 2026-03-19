#include "basicpistol.hpp"

BasicPistol::BasicPistol(std::string name, glm::vec3 position, std::shared_ptr<Model> itemModel,
                         std::shared_ptr<Shader> itemShader, float scale, std::shared_ptr<Shader> outlineShader,
                         glm::vec3 holdingScaleFactor)
                         : Item(name, position, itemModel, itemShader, scale, outlineShader, holdingScaleFactor) {
    this->selected = false;
    this->range = 100;
    this->damage = 10;
    this->shootingSpeed = .5;
}

void BasicPistol::use() {
    glm::vec3 outOrigin = this->positionCallback();
    glm::vec3 outEnd = outOrigin + this->forwardCallback() * this->range;
    btCollisionWorld::ClosestRayResultCallback RayCallback(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z));
    this->physWorld->rayTest(btVector3(outOrigin.x, outOrigin.y, outOrigin.z), btVector3(outEnd.x, outEnd.y, outEnd.z), RayCallback);
    if(RayCallback.hasHit()) {
        GameObject* hitObject = (GameObject*)RayCallback.m_collisionObject->getUserPointer(); // todo cast to generic gameobject
        if(hitObject != nullptr) {
            GameObjectInteractionType interactionType = hitObject->getInteraction();
            if(interactionType == HIT) {
                hitObject->takeHit(10);
            }
        }
    }
}

ItemUseType BasicPistol::getItemUseType() {
    return ItemUseType::SHOOT;
}

// void BasicPistol::addToWorld(btDiscreteDynamicsWorld *world) {
//     if(initialized) {
//         this->itemRigidBody->addToWorld(world);
//         this->itemRigidBody->entityRigidBody->setUserPointer(this);
//         this->physWorld = world;
//     }
// }

// void BasicPistol::initialize() {
//     this->initialized = true;
//     this->itemRigidBody = new RigidBodyEntity(
//         this->itemModel, btVector3(this->position.x,this->position.y,this->position.z), BOX, 0.5f, btVector3(1.0, 1.0, 1.0), this->scale
//     );
//     this->itemRigidBody->initialize(glm::mat4(1.0));
// }

