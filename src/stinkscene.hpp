#ifndef STINKSCENE_HPP
#define STINKSCENE_HPP
#include "base/scene.hpp"
#include "base/stb_image.h"
#include "base/postprocessor.hpp"
#include "base/player.hpp"
#include "base/uimaster.hpp"
#include "base/item.hpp"
#include "stink.hpp"
#include "piggy.hpp"

class StinkScene : public Scene {
      public:
        StinkScene(std::string name, Camera &camera, UIMaster &ui);
        void render(float deltaTime, float curTime, GLFWwindow *window, glm::vec2 windowDims);
        void initialize(std::function<void(float, std::string)> progressCallback);
        void addGameObject(std::shared_ptr<GameObject> gameObject);
        void updateWindowSize(glm::vec2 windowDims);
    private:
        std::vector<std::shared_ptr<GameObject>> gameObjects;
        std::shared_ptr<PostProcessor> postProcessor;
        std::shared_ptr<Terrain> terrain;
        std::shared_ptr<Model> sceneTerrainModel;
        std::shared_ptr<Model> grassModel;
        btDiscreteDynamicsWorld *world;
        std::shared_ptr<Shader> sceneShader;
        std::shared_ptr<Shader> pigShader;
        std::shared_ptr<Shader> shellTxShader;
        std::shared_ptr<Player> player;
        std::string name;
        std::shared_ptr<Model> stinkHeadModel;
        std::shared_ptr<Shader> outlineShader;
        BillboardEntity* grassBillboard;
        Camera &camera;
        UIMaster &ui;
        bool initialized;
        bool paused;
        bool physDebugOn;
        std::shared_ptr<Skybox> skybox;
};

#endif