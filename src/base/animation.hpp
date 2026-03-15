#ifndef ANIMATION_HPP
#define ANIMATION_HPP
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "model.hpp"
#include "bone.hpp"

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childCount;
    std::vector<AssimpNodeData> children;
};

class Animation
{
    public:
        Animation() = default;
        Animation(const std::string& animationPath, Model* model, int animIndex = 0);
        Bone* findBone(const std::string& name);

        inline float getTicksPerSecond() { return mTicksPerSecond; }
        inline float getDuration() { return mDuration;}

        inline const AssimpNodeData& getRootNode() { return mRootNode; }

        inline const std::map<std::string,BoneInfo>& getBoneIDMap() 
        { 
            return mBoneInfoMap;
        }

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
    private:
        void readMissingBones(const aiAnimation* animation, Model& model);
        void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
        float mDuration;
        int mTicksPerSecond;
        std::vector<Bone> mBones;
        AssimpNodeData mRootNode;
        std::map<std::string, BoneInfo> mBoneInfoMap;
};

#endif