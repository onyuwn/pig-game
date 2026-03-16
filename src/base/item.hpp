#ifndef ITEM_HPP
#define ITEM_HPP
#include "gameobject.hpp"
#include "gameobjectinteractiontype.hpp"
#include "model.hpp"
#include "shader.hpp"

class Item : GameObject {
    public:
        void use();
        std::string name;
        void render(float deltaTime, glm::mat4 model = glm::mat4(1.0), glm::mat4 view = glm::mat4(1.0), glm::mat4 projection = glm::mat4(1.0), float curTime = 0.0, glm::vec3 sceneLightPos = glm::vec3(0.0));
        void initialize();
        void setPos(std::function<glm::vec3()> posCallback) = 0;
        std::string getHelpText();
        glm::vec3 position;
        glm::vec3 rotation;
    private:
        float scale;
        std::shared_ptr<Model> itemModel;
        std::shared_ptr<Shader> itemShader;
};

#endif