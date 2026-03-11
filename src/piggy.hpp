#ifndef PIGGY_HPP
#define PIGGY_HPP
#define GLM_ENABLE_EXPERIMENTAL
#include <string>
#include <vector>
#include "base/model.hpp"
#include "base/shader.hpp"
#include "base/rigidbodyentity.hpp"
#include "base/dialogue.hpp"
#include "base/collisionshapetype.hpp"
#include "base/gameobject.hpp"
#include "base/gameobjectinteractiontype.hpp"

class Piggy : public GameObject {
    public:
        std::string name;
        std::vector<std::string> lines;
        bool interacting;
        bool isHit;
        float hitTime;
        int lineIdx;
        bool initialized;
        RigidBodyEntity* piggyRigidBody;
        RigidBodyEntity* shatteredPigRigidBody;
        glm::mat4 modelMatrix;
        Piggy(std::string name);
        void render(float deltaTime, glm::mat4 model = glm::mat4(1.0), glm::mat4 view = glm::mat4(1.0), glm::mat4 projection = glm::mat4(1.0), float curTime = 0.0) override;
        GameObjectInteractionType getInteraction() override;
        glm::vec3 getPos();
        void setPos(std::function<glm::vec3()> posCallback) override;
        void initialize() override;
        void addToWorld(btDiscreteDynamicsWorld * world);
        void applyForce(glm::vec3 force) override;
        void toggleRigidBody() override;
        void toggleState() override;
        int getHealth();
        void takeHit(int dmg);
        std::string getHelpText() override;
    private:
        int health;
        std::shared_ptr<Model> piggyModel;
        std::shared_ptr<Model> shatteredPigModel;
        std::shared_ptr<Shader> piggyShader;
        std::function<glm::vec3()> positionCallback;
};

#endif