#ifndef ITEM_HPP
#define ITEM_HPP
#define GLM_ENABLE_EXPERIMENTAL
#include "gameobject.hpp"
#include "gameobjectinteractiontype.hpp"
#include "rigidbodyentity.hpp"
#include "model.hpp"
#include "shader.hpp"

class Item : public GameObject {
    public:
        Item(std::string name, glm::vec3 position, std::shared_ptr<Model> itemModel,
            std::shared_ptr<Shader> itemShader, float scale, std::shared_ptr<Shader> outlineShader);
        void use();
        std::string name;
        void render(float deltaTime, glm::mat4 model = glm::mat4(1.0), glm::mat4 view = glm::mat4(1.0), glm::mat4 projection = glm::mat4(1.0), float curTime = 0.0, glm::vec3 sceneLightPos = glm::vec3(0.0));
        void initialize();
        void setPosition(glm::vec3 newPos);
        void setSelected(bool selected);
        void setPos(std::function<glm::vec3()> posCallback);
        glm::vec3 getPos();
        void applyForce(glm::vec3 force);
        void toggleRigidBody();
        void toggleState();
        GameObjectInteractionType getInteraction();
        void takeHit(int dmg);
        void setScale(float scale);
        void addToWorld(btDiscreteDynamicsWorld *world);
        glm::vec3 getForward();
        void setForward(std::function<glm::vec3()> forwardCallback);
        void setParentTransform(std::function<glm::mat4()> parentTransformCallback);
        std::string getHelpText();
        glm::vec3 position;
        glm::vec3 rotation;
        bool selected;
    private:
        float scale;
        std::shared_ptr<Model> itemModel;
        std::shared_ptr<Shader> itemShader;
        std::shared_ptr<Shader> outlineShader;
        btDynamicsWorld* physWorld;
        bool initialized;
        RigidBodyEntity* itemRigidBody;
        bool twoHanded;
        bool itemHeld;
        std::function<glm::vec3()> positionCallback;
        std::function<glm::vec3()> forwardCallback;
        std::function<glm::mat4()> parentTransformCallback;
};

#endif