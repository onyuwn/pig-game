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

class Player {
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
        void render(float curTime, float deltaTime);
        void pollInteractables();
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