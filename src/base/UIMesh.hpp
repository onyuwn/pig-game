#ifndef UIMESH_HPP
#define UIMESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include "shader.hpp"

class UIMesh {
    public:
        UIMesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs = {});
        void initMesh();
        void draw(Shader &shader);
    private:
        unsigned int vertexBufferID;
        unsigned int uvBufferID;
        unsigned int vaoId;
        unsigned int eboId;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
};

#endif