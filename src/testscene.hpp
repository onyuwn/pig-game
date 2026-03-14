#ifndef TESTSCENE_HPP
#define TESTSCENE_HPP

#include "base/scene.hpp"
#include "base/stb_image.h"
#include "base/postprocessor.hpp"
#include "base/player.hpp"
#include "base/uimaster.hpp"
#include "piggy.hpp"

class TestScene : public Scene {
    public:
        TestScene(std::string name, Camera &camera, UIMaster &ui);
        void render(float deltaTime, float curTime, GLFWwindow *window, glm::vec2 windowDims);
        void initialize(std::function<void(float, std::string)> progressCallback);
        void addGameObject(std::shared_ptr<GameObject> gameObject);
        void updateWindowSize(glm::vec2 windowDims);
    private:
        std::vector<std::shared_ptr<GameObject>> gameObjects;
        std::shared_ptr<PostProcessor> postProcessor;
        std::shared_ptr<Terrain> terrain;
        std::shared_ptr<Model> sceneTerrainModel;
        std::shared_ptr<Model> piggyModel;
        btDiscreteDynamicsWorld *world;
        std::shared_ptr<Shader> sceneShader;
        std::shared_ptr<Player> player;
        std::string name;
        void spawnNewPig(int pigIdx);
        Camera &camera;
        UIMaster &ui;
        bool initialized;
        bool paused;
        bool physDebugOn;
        float lastPigSpawnTime;
        float pigSpawnFrequency;
};

#endif