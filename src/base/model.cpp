#include "model.hpp"

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);

Model::Model(char *path) {
    std::cout << "loading..." << path << std::endl;
    loadModel(path);
}

Model::Model(const Model& x) : meshes(x.meshes) {
    std::cout << "copy" << x.meshes.size() << std::endl;
}

void Model::setVertexBoneDataToDefault(Vertex& vertex) {
    for(int i = 0; i < 4; i++) {
        vertex.boneIdxs[i] = -1;
        vertex.boneWeights[i] = 0.0f;
    }
}

void Model::draw(Shader &shader, float curTime) {
    for(unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].draw(shader);
    }
}

void Model::drawOutline(Shader &shader, float curTime) {
    // glEnable(GL_STENCIL_TEST);
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // glStencilFunc(GL_ALWAYS, 1, 0xFF);
    // glStencilMask(0xFF);

    // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // glStencilMask(0x00);
    // glDisable(GL_DEPTH_TEST);

    // todo draw outline again?
    for(unsigned int i = 0; i < meshes.size(); i++) {
        // TODO: model, view, proj matrices not making it in herre
        meshes[i].draw(shader);
    }

    // glStencilMask(0xFF);
    // glStencilFunc(GL_ALWAYS, 0, 0xFF);
    // glDisable(GL_STENCIL_TEST);
    // glEnable(GL_DEPTH_TEST);
}

std::vector<Mesh> meshes;
std::string directory;

void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs); 

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

Mesh Model::getMesh() {
    return meshes[0];
}
std::vector<Mesh> Model::getMeshes() {
    return meshes;
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
        meshes.push_back(processMesh(mesh, scene));			
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        setVertexBoneDataToDefault(vertex);
        vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        
        if(mesh->mTextureCoords[0]) {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        } else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j=0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, 
                                            aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, 
                                            aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    } 

    extractBoneWeightForVertices(vertices, mesh, scene);

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void Model::setVertexBoneData(Vertex& vertex, int boneId, float weight) {
    for(int i = 0; i < 4; ++i) {
        if(vertex.boneIdxs[i] < 0) {
            vertex.boneWeights[i] = weight;
            vertex.boneIdxs[i] = boneId;
        }
    }
}

void Model::extractBoneWeightForVertices(std::vector<Vertex> &vertices, aiMesh* mesh, const aiScene* scene) {
    for(int boneIdx = 0; boneIdx < mesh->mNumBones; ++boneIdx) {
        int boneId = -1;
        std::string boneName = mesh->mBones[boneIdx]->mName.C_Str();
        if(m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
            BoneInfo newBone;
            newBone.id = m_BoneCounter;
            newBone.offset = aiMatrix4x4ToGlm(mesh->mBones[boneIdx]->mOffsetMatrix);
            m_BoneInfoMap[boneName] = newBone;
            boneId = m_BoneCounter;
            m_BoneCounter++;
        }
        else
        {
            boneId = m_BoneInfoMap[boneName].id;
        }

        auto weights = mesh->mBones[boneIdx]->mWeights;
        int numWeights = mesh->mBones[boneIdx]->mNumWeights;

        for(int weightIdx = 0; weightIdx < numWeights; ++weightIdx) {
            int vertexId = weights[weightIdx].mVertexId;
            float weight = weights[weightIdx].mWeight;
            assert(vertexId <= vertices.size());
            setVertexBoneData(vertices[vertexId], boneId, weight);
        }
    }
}

bool Model::isLoaded() {
    return !this->meshes.empty();
}