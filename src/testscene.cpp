#include "testscene.hpp"

TestScene::TestScene(std::string name, Camera &camera, UIMaster &ui) : initialized(false), camera(camera), ui(ui), physDebugOn(false), paused(false) {
        this->lastPigSpawnTime = 0.0f;
        this->pigSpawnFrequency = 2.0f;
}

void TestScene::render(float deltaTime, float curTime, GLFWwindow *window, glm::vec2 windowDims) {
    if(this->initialized) {
        this->postProcessor->begin();
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        this->world->stepSimulation(deltaTime * 5.0f, 7);
        this->player->UpdatePlayer(curTime, deltaTime, window, this->paused);
        // this->ui.updateWindowSize(windowDims.x, windowDims.y);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom * 2.0f), (float)windowDims.x / (float)windowDims.y, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix(player->getPlayerPos() + glm::vec3(0,1,0));

        this->sceneShader->use();
        sceneShader->setMat4("projection", projection);
        sceneShader->setMat4("view", view);
        sceneShader->setFloat("amgientStrength", .25);
        sceneShader->setFloat("opacity", 1.0);
        this->sceneShader->setVec3("lightPos", this->player->getPlayerHandPos());

        this->sceneShader->setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, -10, 0));
        sceneShader->setMat4("model", model);
        this->player->render(curTime, deltaTime, windowDims);
        for(int i = 0; i < this->gameObjects.size(); i++) {
            if(!this->gameObjects[i]->shouldBeDestroyed) {
                this->gameObjects[i]->render(deltaTime, model, view, projection, curTime, this->player->getPlayerHandPos());
            }
        }
        
        if(curTime - lastPigSpawnTime > pigSpawnFrequency) {
            this->spawnNewPig(curTime / pigSpawnFrequency); // todo store models and shaders in scene, game object instanced with pointers to those?
            lastPigSpawnTime = curTime;
        }

        this->terrain->render(*this->sceneShader);
        this->ui.gamePaused = this->paused;
        this->ui.render(deltaTime, curTime, windowDims);
        // this->skybox->render(glm::mat4(glm::mat3(view)), projection);
        this->postProcessor->render(curTime, windowDims);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void TestScene::initialize(std::function<void(float, std::string)> progressCallback) {
    progressCallback(.25f, "loading shaders...");
    this->sceneShader = std::make_shared<Shader>("src/shaders/basic.vs", "src/shaders/basic.fs");
    this->paused = false;
    this->piggyModel = std::make_shared<Model>((char*)"resources/piggyiso.obj");
    this->shatteredPigModel1 = std::make_shared<Model>((char*)"resources/pig/pigshatter3ahh.gltf");
    this->pigShader = std::make_shared<Shader>("src/shaders/basic.vs", "src/shaders/basic.fs");
    progressCallback(.25f, "initializing physics...");
    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    this->world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    this->postProcessor = std::make_shared<PostProcessor>(glm::vec2(800, 600));

    stbi_set_flip_vertically_on_load(false);
    world->setGravity(btVector3(0,-9.81f,0));
    progressCallback(.25f, "initializing terrain...");
    this->sceneTerrainModel = std::make_shared<Model>((char*)"resources/testfloor.obj");
    std::shared_ptr<GameObject> piggyGameObject =
        std::make_shared<Piggy>("piggy", glm::vec3(0, 8, -20), 2.0, this->piggyModel, this->shatteredPigModel1, this->pigShader
    );
    piggyGameObject->initialize();
    if (auto piggyPtr = std::dynamic_pointer_cast<Piggy>(piggyGameObject)) {
        piggyPtr->addToWorld(this->world);
    }
    this->addGameObject(piggyGameObject);
    this->terrain = std::make_shared<Terrain>(*this->sceneTerrainModel);
    this->terrain->initTerrain();
    this->terrain->addToWorld(world);

    this->player = std::make_shared<Player>(camera, this->world, ui, physDebugOn, "resources/character/player1test.gltf");
    player->initialize();
    this->player->addToWorld(this->world);

    progressCallback(.25f, "Done!");
    this->initialized = true;
}

void TestScene::addGameObject(std::shared_ptr<GameObject> gameObject) {
    this->gameObjects.push_back(gameObject);
}

void TestScene::spawnNewPig(int pigIdx) {
    int xRand = (rand() % 100) - 50;
    int yRand = (rand() % 25);
    int zRand = (rand() % 100) - 50;
    std::shared_ptr<GameObject> piggyGameObject = 
        std::make_shared<Piggy>(
            "piggy", glm::vec3(xRand, yRand, zRand), 2.0, this->piggyModel, this->shatteredPigModel1, this->pigShader
        );
    piggyGameObject->initialize();
    if (auto piggyPtr = std::dynamic_pointer_cast<Piggy>(piggyGameObject)) {
        piggyPtr->addToWorld(this->world);
    }
    this->addGameObject(piggyGameObject);
    this->gameObjects.push_back(piggyGameObject);
}

void TestScene::updateWindowSize(glm::vec2 windowDims) {
    this->ui.updateWindowSize(windowDims.x, windowDims.y);
    this->postProcessor->setScreenSize(windowDims);
}