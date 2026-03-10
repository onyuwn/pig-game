#ifndef MODEL_HPP
#define MODEL_HPP
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <vector>
#include "shader.hpp"
#include "mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"
#include <string>
#include <map>

struct BoneInfo {
    int id;
    glm::mat4 offset;
};

class Model
{
    public:
        Model(char *path);
        Model(const Model& x);
        Model& operator=(const Model& x);
        void draw(Shader &shader, float curTime = 0.0f);
        void drawOutline(Shader &shader, float curTime = 0.0f);
        Mesh getMesh();
        std::vector<Mesh> getMeshes();
        bool isLoaded();
        inline auto& getBoneInfoMap() { return m_BoneInfoMap; }
        inline int& getBoneCount() { return m_BoneCounter; }
    private:
        std::vector<Mesh> meshes;
        std::string directory;
        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

        std::map<std::string, BoneInfo> m_BoneInfoMap;
        int m_BoneCounter = 0;

        void setVertexBoneDataToDefault(Vertex &vertex);
        void setVertexBoneData(Vertex& vertex, int boneId, float weight);
        void extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh* mesh, const aiScene* scene);

        inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4& from)
        {
            glm::mat4 to;
            //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
            to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
            to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
            to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
            to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
            return to;
        }
};

#endif