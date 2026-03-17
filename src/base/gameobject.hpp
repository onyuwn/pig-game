#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include <glm/glm.hpp>
#include <string>
#include <functional>
#include "gameobjectinteractiontype.hpp"

class GameObject {
    public:
        virtual void render(float deltaTime, glm::mat4 model = glm::mat4(1.0), glm::mat4 view = glm::mat4(1.0), glm::mat4 projection = glm::mat4(1.0), float curTime = 0.0, glm::vec3 sceneLightPos = glm::vec3(0.0)) = 0;
        virtual void initialize() = 0;
        virtual GameObjectInteractionType getInteraction() = 0;
        //maybe rename this to overridepos
        virtual void setPos(std::function<glm::vec3()> posCallback) = 0;
        virtual void applyForce(glm::vec3 force) = 0;
        virtual void takeHit(int dmg) = 0;
        virtual void toggleRigidBody() = 0;
        virtual void toggleState() = 0; // door
        virtual void setSelected(bool selected) = 0;
        virtual std::string getHelpText() = 0;
        bool selected;
        virtual void setScale(float scale) = 0;
        bool shouldBeDestroyed = false;
        virtual glm::vec3 getForward() = 0;
        glm::vec3 rotation;
        std::string name;
    private:
        float scale;
};

#endif