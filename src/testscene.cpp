#include "testscene.hpp"

TestScene::TestScene(
    std::string name,
    Camera &camera) : initialized(false), camera(camera) {

}

void TestScene::render(float deltaTime, float curTime, GLFWwindow *window) {
    if(this->initialized) {
        this->postProcessor->begin();
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        this->world->stepSimulation(deltaTime * 3.0f, 7);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom * 2.0f), (float)800 / (float)600, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix(glm::vec3(0,1,0));

        this->sceneShader->use();
        sceneShader->setMat4("projection", projection);
        sceneShader->setMat4("view", view);

        this->sceneShader->setVec3("lightPos", glm::vec3(0,1,0));
        this->sceneShader->setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, -10, 0));
        sceneShader->setMat4("model", model);

        this->terrain->render(*this->sceneShader);
        // this->skybox->render(glm::mat4(glm::mat3(view)), projection);
        this->postProcessor->render(curTime);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void TestScene::initialize(std::function<void(float, std::string)> progressCallback) {
    progressCallback(.25f, "loading shaders...");
    this->sceneShader = std::make_shared<Shader>("src/shaders/basic.vs", "src/shaders/basic.fs");
    progressCallback(.25f, "initializing physics...");
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    this->world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    this->postProcessor = std::make_shared<PostProcessor>();

    stbi_set_flip_vertically_on_load(false);
    world->setGravity(btVector3(0,-9.81f,0));
    progressCallback(.25f, "initializing terrain...");
    this->sceneTerrainModel = std::make_shared<Model>((char*)"resources/buildings/test/testhall.obj");
    this->terrain = std::make_shared<Terrain>(*this->sceneTerrainModel);
    this->terrain->initTerrain();
    this->terrain->addToWorld(world);
    progressCallback(.25f, "Done!");
    this->initialized = true;
}