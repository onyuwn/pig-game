#ifndef BILLBOARDENTITY_HPP
#define BILLBOARDENTITY_HPP
#include "model.hpp"
#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

class BillboardEntity {
    public:
        BillboardEntity(Model& entityModel, glm::vec3 position, glm::vec2 size);
        void render(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, glm::vec3 up, glm::vec3 right, glm::vec3 playerPos = glm::vec3(0.0, 0.0, 0.0));
        void init();
        void setPos(glm::vec3 pos);
    private:
        Model& entityModel;
        Shader billboardShader;
        glm::vec3 position;
        glm::vec2 size;
};

#endif