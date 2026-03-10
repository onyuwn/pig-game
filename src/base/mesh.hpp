#ifndef MESH_HPP
#define MESH_HPP
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shader.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    int boneIdxs[4];
    float boneWeights[4];
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void draw(Shader &shader);
    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh();
};

#endif