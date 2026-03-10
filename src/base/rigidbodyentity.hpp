#ifndef RIGIDBODYENTITY_HPP
#define RIGIDBODYENTITY_HPP
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "model.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "collisionshapetype.hpp"

class RigidBodyEntity {
    public:
        btCollisionShape* entityCollisionShape;
        btRigidBody* entityRigidBody;
        RigidBodyEntity(Model& entityModel, btVector3 defaultPos = btVector3(0,1,0), CollisionShapeType collisionShapeType = BOX, float mass = 1.0, btVector3 boxShape = btVector3(1.0, 1.0, 1.0));
        //~RigidBodyEntity();
        void render(Shader &shader, glm::mat4 model = glm::mat4(1.0), bool positionOverride = false);
        void activateInteraction();
        void initialize(glm::mat4 model = glm::mat4(1.0));
        void addToWorld(btDynamicsWorld * world);
        glm::vec3 getPos();
        glm::mat4 getFinalModelMatrix();
        void setPos(glm::vec3 newPos);
    private:
        CollisionShapeType collisionShapeType;
        Model &entityModel;
        btVector3 defaultPos;
        btVector3 boundingBox;
        float mass;
        bool initialized;
        glm::mat4 finalModelMatrix;
};

#endif