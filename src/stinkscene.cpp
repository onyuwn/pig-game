#include "stinkscene.hpp"

StinkScene::StinkScene(std::string name, Camera &camera, UIMaster &ui)
    : initialized(false), camera(camera), ui(ui), physDebugOn(false), paused(false)
{
        this->lastPigSpawnTime = 0.0f;
        this->pigSpawnFrequency = 2.0f;
}

void StinkScene::render(float deltaTime, float curTime, GLFWwindow *window, glm::vec2 windowDims) {
    if(this->initialized) {
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        this->world->stepSimulation(deltaTime * 5.0f, 7);
        this->player->UpdatePlayer(curTime, deltaTime, window, this->paused);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom * 2.0f), (float)windowDims.x / (float)windowDims.y, 0.1f, 500.0f);
        glm::mat4 view = camera.GetViewMatrix(player->getPlayerPos() + glm::vec3(0,1,0));

        this->sceneShader->use();
        sceneShader->setMat4("projection", projection);
        sceneShader->setMat4("view", view);
        sceneShader->setFloat("amgientStrength", .25);
        sceneShader->setFloat("opacity", 1.0);
        this->sceneShader->setVec3("lightPos", this->player->getPlayerRightHandPos());
        this->sceneShader->setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, -10, 0));
        sceneShader->setMat4("model", model);

        this->player->render(curTime, deltaTime, windowDims);

        for(int i = 0; i < this->gameObjects.size(); i++) {
            if(!this->gameObjects[i]->shouldBeDestroyed) {
                this->gameObjects[i]->render(deltaTime, model, view, projection, curTime, this->player->getPlayerRightHandPos());
            }
        }

        if(curTime - lastPigSpawnTime > pigSpawnFrequency) {
            this->spawnNewPig(curTime / pigSpawnFrequency); // todo store models and shaders in scene, game object instanced with pointers to those?
            this->spawnNewStink(curTime / pigSpawnFrequency);
            lastPigSpawnTime = curTime;
        }

        this->shellTxShader->use();
        this->shellTxShader->setMat4("projection", projection);
        this->shellTxShader->setMat4("view", view);
        this->shellTxShader->setVec3("lightPos", this->player->getPlayerRightHandPos());
        this->shellTxShader->setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));
        //this->grassBillboard->render(view, projection, this->camera.Up, this->camera.Right, this->player->getPlayerRightHandPos());
        this->terrain->render(*this->shellTxShader, glm::mat4(1.0), 50.0);
        //this->skybox->render(glm::mat4(glm::mat3(view)), projection);
        //this->terrain->render(*this->sceneShader, glm::mat4(1.0), 0.0);
        this->ui.render(deltaTime, curTime, windowDims);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void StinkScene::initialize(std::function<void(float, std::string)> progressCallback) {
    this->paused = false;
    this->sceneShader = std::make_shared<Shader>("src/shaders/basic.vs", "src/shaders/basic.fs");
    this->pigShader = std::make_shared<Shader>("src/shaders/basic.vs", "src/shaders/basic.fs");
    this->outlineShader = std::make_shared<Shader>("src/shaders/outline.vs", "src/shaders/outline.fs");
    this->shellTxShader = std::make_shared<Shader>("src/shaders/shelltx.vs", "src/shaders/shelltx.fs");
    this->stinkHeadModel = std::make_shared<Model>((char*)"resources/stink/stinkhead1.gltf");
    this->stinkHead2Model = std::make_shared<Model>((char*)"resources/stink/stinkhead2.gltf");
    this->piggyModel = std::make_shared<Model>((char*)"resources/pig/basepig/pigwalking2.gltf");
    this->shatteredPigModel1 = std::make_shared<Model>((char*)"resources/pig/shatters/shatteredpig5.gltf");

    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    this->world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    this->postProcessor = std::make_shared<PostProcessor>(glm::vec2(800, 600));
    stbi_set_flip_vertically_on_load(false);
    world->setGravity(btVector3(0,-9.81f,0));
    this->sceneTerrainModel = std::make_shared<Model>((char*)"resources/stink/humancarpet1.gltf");

    std::shared_ptr<GameObject> stinkHeadItem =
        std::make_shared<StinkItem>("stinkcur", glm::vec3(-10, 10, -10), stinkHeadModel, sceneShader, 4.0, outlineShader, glm::vec3(.125));
    
    // std::shared_ptr<GameObject> stinkHead2Item =
    //     std::make_shared<StinkItem>("stinkcur2", glm::vec3(-10, 10, -10), stinkHead2Model, sceneShader, 4.0, outlineShader, glm::vec3(.125));
    std::shared_ptr<GameObject> hourglassGameObject =
        std::make_shared<HourGlassBomb>("HOURGLASS", glm::vec3(10, 10, -10), stinkHead2Model, sceneShader, 4.0, outlineShader, glm::vec3(.125));
    //(dynamic_cast<StinkItem*>(stinkHeadItem.get()))->initialize();
    (dynamic_cast<HourGlassBomb*>(hourglassGameObject.get()))->initialize();
    // if (auto stinkPtr = std::dynamic_pointer_cast<Item>(stinkHeadItem)) {
    //     stinkPtr->addToWorld(this->world);
    // }
    (dynamic_cast<HourGlassBomb*>(hourglassGameObject.get()))->initialize();
    if (auto stinkPtr2 = std::dynamic_pointer_cast<Item>(hourglassGameObject)) {
        stinkPtr2->addToWorld(this->world);
    }
    (dynamic_cast<HourGlassBomb*>(hourglassGameObject.get()))->initializeAOE();
    //this->addGameObject(stinkHeadItem);
    this->addGameObject(hourglassGameObject);
    this->terrain = std::make_shared<Terrain>(*this->sceneTerrainModel);
    this->terrain->initTerrain();
    this->terrain->addToWorld(world);

    // std::shared_ptr<GameObject> stinkItem =
    //     std::make_shared<StinkItem>("stinkcur", glm::vec3(-10, 10, -10), stinkHeadModel, pigShader, 2.0, outlineShader, glm::vec3(.05));

    this->player = std::make_shared<Player>(camera, this->world, ui, physDebugOn, "resources/character/arms2.gltf");
    player->initialize();
    this->player->addToWorld(this->world);

    std::vector<std::string> skyboxFaces = { // rgba
        "resources/skybooxes/one/sky.png",
        "resources/skybooxes/one/sky.png",
        "resources/skybooxes/one/sky.png",
        "resources/skybooxes/one/sky.png",
        "resources/skybooxes/one/sky.png",
        "resources/skybooxes/one/sky.png",
    };
    this->skybox = std::make_shared<Skybox>(skyboxFaces);
    this->grassModel = std::make_shared<Model>((char*)"resources/stink/testbillboard.gltf");
    this->grassBillboard = new BillboardEntity(*this->grassModel, glm::vec3(0.0), glm::vec2(2.0, 2.0));
    this->grassBillboard->init();
    this->initialized = true;
}

void StinkScene::updateWindowSize(glm::vec2 windowDims) {
    this->ui.updateWindowSize(windowDims.x, windowDims.y);
    this->postProcessor->setScreenSize(windowDims);
}

void StinkScene::spawnNewPig(int pigIdx) {
    int xRand = (rand() % 100) - 50;
    int yRand = (rand() % 25) + 5;
    int zRand = (rand() % 100) - 50;
    std::shared_ptr<GameObject> piggyGameObject = 
        std::make_shared<Piggy>(
            "piggy", glm::vec3(xRand, yRand, zRand), 2.0, this->piggyModel, this->shatteredPigModel1, this->pigShader, pigIdx, this->outlineShader
        );
    piggyGameObject->initialize();
    if (auto piggyPtr = std::dynamic_pointer_cast<Piggy>(piggyGameObject)) {
        piggyPtr->addToWorld(this->world);
    }
    this->addGameObject(piggyGameObject);
    this->gameObjects.push_back(piggyGameObject);
}

void StinkScene::spawnNewStink(int stinkIdx) {
    int xRand = (rand() % 100) - 50;
    int yRand = (rand() % 25) + 5;
    int zRand = (rand() % 100) - 50;
    std::shared_ptr<GameObject> stinkGameObject = 
        std::make_shared<StinkItem>("stinkcur", glm::vec3(-10, 10, -10), this->stinkHead2Model, sceneShader, 4.0, outlineShader, glm::vec3(.125));
    stinkGameObject->initialize();
    if (auto piggyPtr = std::dynamic_pointer_cast<StinkItem>(stinkGameObject)) {
        piggyPtr->addToWorld(this->world);
    }
    this->addGameObject(stinkGameObject);
    this->gameObjects.push_back(stinkGameObject);
}

void StinkScene::addGameObject(std::shared_ptr<GameObject> gameObject) {
    this->gameObjects.push_back(gameObject);
}