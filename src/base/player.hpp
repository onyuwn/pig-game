#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "model.hpp"
#include "camera.hpp"
#include <glm/gtc/quaternion.hpp>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "rigidbodyentity.hpp"
#include "uimaster.hpp"
#include "gameobject.hpp"
#include "gameobjectinteractiontype.hpp"

class Player : GameObject {
    public:
        btCollisionShape* playerCollisionShape;
        btRigidBody* playerRigidBody;
        glm::vec3 position;

        Player(Camera &camera, btDiscreteDynamicsWorld *world, UIMaster &uiCallback, bool &physDebugCallback, std::string playerModelPath); // todo: add player model
        void initialize();
        void UpdatePlayer(float curTime, float deltaTime, GLFWwindow *window, bool &pauseCallback);
        void processInput(GLFWwindow *window, float curTime, float deltaTime, bool &pauseCallback);
        void addToWorld(btDiscreteDynamicsWorld * world);
        void interact(float curTime);
        bool checkGrounded();
        glm::vec3 getPlayerPos();
        glm::vec3 getPlayerHandPos();
        glm::mat3 getPlayerRotationMatrix();
        bool isAlive();
        bool isControlDisabled();
        void render(float curTime, float deltaTime, glm::vec2 windowDims);
        void pollInteractables();
        
        void render(float deltaTime, glm::mat4 model = glm::mat4(1.0), glm::mat4 view = glm::mat4(1.0), glm::mat4 projection = glm::mat4(1.0), float curTime = 0.0, glm::vec3 sceneLightPos = glm::vec3(0.0));
        void setPos(std::function<glm::vec3()> posCallback);
        void applyForce(glm::vec3 force);
        void takeHit(int dmg);
        void setScale(float scale);
        void toggleRigidBody();
        void toggleState();
        std::string getHelpText();
        GameObjectInteractionType getInteraction();
        bool selected = false;
        bool shouldBeDestroyed = false;
        glm::vec3 getForward();
        glm::vec3 rotation;
        std::string name;
    private:
        Camera &camera;
        btDiscreteDynamicsWorld *world;
        bool interactRequested;
        bool controlsDisabled;
        UIMaster &uiCallback;
        bool &physDebugOn;
        bool initialized;
        GameObject* heldItem;
        bool itemInHand;
        std::shared_ptr<Model> playerModel;
        std::shared_ptr<Shader> playerShader;
        std::string playerModelPath;
        bool clickRequested;
        bool pauseRequested;
        float playerHeight = 1.0;
};
#endif