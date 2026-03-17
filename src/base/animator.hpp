#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "animation.hpp"

class Animator {
    public:
        Animator(Animation* animation);
        void updateAnimation(float dt);
        void playAnimation(Animation* pAnimation);
        void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
        glm::mat4 calculateTransformFromAnimation();
        std::vector<glm::mat4> getFinalBoneMatrices();
        int findBoneIndex(std::string boneName);    
        glm::mat4 getGlobalBoneTransform(std::string boneName);
    private:
        std::vector<glm::mat4> mFinalBoneMatrices;
        std::vector<glm::mat4> mGlobalBoneMatrices;
        Animation* mCurrentAnimation;
        float mCurrentTime;
        float mDeltaTime;
        bool isSkeletal;
};

#endif