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
        btDynamicsWorld* physWorld;
        std::vector<std::shared_ptr<RigidBodyEntity>> shatteredPigRigidBodies;
        glm::mat4 modelMatrix;
        Piggy(std::string name, glm::vec3 position, float scale);
        void render(float deltaTime, glm::mat4 model = glm::mat4(1.0),
                    glm::mat4 view = glm::mat4(1.0), glm::mat4 projection = glm::mat4(1.0),
                    float curTime = 0.0, glm::vec3 sceneLightPos = glm::vec3(1.0)) override;
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
        void setScale(float scale);
        std::string getHelpText() override;
    private:
        int health;
        bool pigExploded;
        int shatterPiecesInPhysWorld = 0;
        std::shared_ptr<Model> piggyModel;
        std::shared_ptr<Model> shatteredPigModel;
        std::vector<std::shared_ptr<Mesh>> shatteredPigMeshPieces;
        std::shared_ptr<Shader> piggyShader;
        std::function<glm::vec3()> positionCallback;
        float scale = 1.0;
        glm::vec3 initialPosition;
};

#endif