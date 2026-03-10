#ifndef SCENE_HPP
#define SCENE_HPP
#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gameobject.hpp"
#include "skybox.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "model.hpp"
#include "terrain.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include "camera.hpp"

class Scene {
    public:
        virtual void render(float deltaTime, float curTime, GLFWwindow *window) = 0;
        virtual void initialize(std::function<void(float, std::string)> progressCallback) = 0;
    private:
        std::vector<std::shared_ptr<GameObject>> gameObjects;
        std::string name;
        std::shared_ptr<Skybox> skybox;
        btDiscreteDynamicsWorld *world;
        std::shared_ptr<Terrain> terrain;
        std::function<void(std::string)> changeSceneCallback;
        bool initialized;
        bool paused;
};

#endif