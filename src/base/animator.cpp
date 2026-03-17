#include "animator.hpp"

Animator::Animator(Animation* animation) {
    mCurrentTime = 0.0;
    mCurrentAnimation = animation;
    this->isSkeletal = false;
    mFinalBoneMatrices.reserve(28);
    mGlobalBoneMatrices.reserve(28);
    for(int i = 0; i < 28; i++) {
        mFinalBoneMatrices.push_back(glm::mat4(1.0));
        mGlobalBoneMatrices.push_back(glm::mat4(1.0));
    }
}

void Animator::updateAnimation(float dt) {
    mDeltaTime = dt;

    if(mCurrentAnimation) {
        mCurrentTime += mCurrentAnimation->getTicksPerSecond() * dt;
        mCurrentTime = fmod(mCurrentTime, mCurrentAnimation->getDuration());
        calculateBoneTransform(&mCurrentAnimation->getRootNode(), glm::mat4(1.0));
    }
}

void Animator::playAnimation(Animation* pAnimation) {
    mCurrentAnimation = pAnimation;
    //mCurrentTime = 0.0f;
}

glm::mat4 Animator::calculateTransformFromAnimation() {
    glm::mat4 finalTransformation = glm::mat4(1.0);
    const AssimpNodeData node = this->mCurrentAnimation->getRootNode();
    glm::mat4 transformation = node.transformation;
    Bone* bone = mCurrentAnimation->findBone(node.name);
    if(bone) {
        bone->update(mCurrentTime);
        finalTransformation = bone->getLocalTransform();
    }

    return finalTransformation;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* bone = mCurrentAnimation->findBone(nodeName);

    if(bone) {
        bone->update(mCurrentTime);
        nodeTransform = bone->getLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;
    auto boneInfoMap = mCurrentAnimation->getBoneIDMap();
    if(boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int index = boneInfoMap[nodeName].id;
        this->mGlobalBoneMatrices[index] = globalTransformation;
        glm::mat4 offset = boneInfoMap[nodeName].offset;
        mFinalBoneMatrices[index] = globalTransformation * offset;
    }

    for(int i = 0; i < node->childCount; i++) {
        calculateBoneTransform(&node->children[i], globalTransformation);
    }
}

std::vector<glm::mat4> Animator::getFinalBoneMatrices() {
    return mFinalBoneMatrices;
}

int Animator::findBoneIndex(std::string boneName) {
    return this->mCurrentAnimation->findBoneIndex(boneName);
}

glm::mat4 Animator::getGlobalBoneTransform(std::string boneName) {
    int boneIndex = this->findBoneIndex(boneName);
    this->calculateTransformFromAnimation();
    return this->mGlobalBoneMatrices[boneIndex];
}