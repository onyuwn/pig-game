#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "base/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"

#include "base/shader.hpp"
#include "base/scene.hpp"
#include "base/camera.hpp"
#include "base/model.hpp"
#include "base/uimaster.hpp"
#include "testscene.hpp"
#include "base/uitextelement.hpp"
#include "base/uispriteanim.hpp"
#include "base/progressbar.hpp"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

bool firstMouse = true;
// camera
Camera camera(glm::vec3(0.0f, 10.0f, 3.0f));
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

Scene *curScene = nullptr;
std::map<std::string, Scene*> sceneMap;
bool initializingScene = true;

void changeScene(std::string newSceneName) {
    curScene = sceneMap[newSceneName];
    initializingScene = true;
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ggg", NULL, NULL);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // UIMaster ui(SCR_WIDTH, SCR_HEIGHT);
    UITextElement *crosshair = new UITextElement("resources/text/Angelic Peace.ttf", "X", 48, 400, 300);

    float progress = 0.0f;
    TestScene *scene1 = new TestScene("scene1", camera);
    sceneMap.insert({"main", scene1});
    curScene = sceneMap["main"];

    ProgressBar progressBar1;
    bool closeCallback;

    UISpriteAnim loadingAnim("resources/ui/hourglasssheet.png", 256, 256, 600, 10);
    loadingAnim.initialize();

    while (!glfwWindowShouldClose(window))
    {
        if(initializingScene) {
            curScene->initialize([&progress, &window, &progressBar1, &crosshair, &closeCallback, &loadingAnim](float newProgress, std::string curProcess) {
                float curTime = static_cast<float>(glfwGetTime());
                deltaTime = curTime - lastFrame;
                lastFrame = curTime;
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                loadingAnim.render(deltaTime);
                progress += newProgress;
                crosshair->setText("Please be patient with me...");
                crosshair->setPos(glm::vec2(400 - (crosshair->getDims().x / 2), 300));
                crosshair->render(1.0, glm::vec3(0.0, 1.0, 0.0), curTime);
                crosshair->setText(curProcess);
                crosshair->setPos(glm::vec2(10, 10));
                crosshair->render(.5, glm::vec3(0.0, 1.0, 0.0), curTime);
                glfwSwapBuffers(window);
                glfwPollEvents();
            });
            crosshair->setText("x");
            crosshair->setPos(glm::vec2(400 - (crosshair->getDims().x / 2), 300 - (crosshair->getDims().y / 2)));
            initializingScene = false;
        } else {
            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            if(curScene) {
                curScene->render(deltaTime, currentFrame, window); // move to curscene var have callback to switch scene
            } else {
                std::cout << "NO SCENE ATTACHED" << std::endl;
            }
        }
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}