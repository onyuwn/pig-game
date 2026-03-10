#ifndef TESTSCENE_HPP
#define TESTSCENE_HPP

#include "base/scene.hpp"
#include "base/stb_image.h"
#include "base/postprocessor.hpp"

class TestScene : public Scene {
    public:
        TestScene(std::string name, Camera &camera);
        void render(float deltaTime, float curTime, GLFWwindow *window);
        void initialize(std::function<void(float, std::string)> progressCallback);
    private:
        std::shared_ptr<PostProcessor> postProcessor;
        std::shared_ptr<Terrain> terrain;
        std::shared_ptr<Model> sceneTerrainModel;
        btDiscreteDynamicsWorld *world;
        std::shared_ptr<Shader> sceneShader;
        std::string name;
        Camera &camera;
        bool initialized;
};

#endif