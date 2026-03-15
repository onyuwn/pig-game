#include "animation.hpp"

Animation::Animation(const std::string& animationPath, Model* model, int animIndex) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    auto animation = scene->mAnimations[animIndex];
    mDuration = animation->mDuration;
    mTicksPerSecond = animation->mTicksPerSecond;
    std::cout << "ANIMATION:" << animation->mName.C_Str() << " " << animIndex << std::endl;
    readHierarchyData(mRootNode, scene->mRootNode);
    readMissingBones(animation, *model);
}

Bone* Animation::findBone(const std::string& name) {
    auto iter = std::find_if(mBones.begin(), mBones.end(),
        [&](const Bone& Bone)
        {
            return Bone.getBoneName() == name;
        });
    if (iter == mBones.end()) return nullptr;
    else return &(*iter);
}

void Animation::readMissingBones(const aiAnimation* animation, Model& model) {
    int size = animation->mNumChannels;
    std::cout << "NUM CHANNELS " << size << std::endl;
    auto& boneInfoMap = model.getBoneInfoMap();
    int& boneCount = model.getBoneCount();

    for(int i = 0; i< size; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if(boneInfoMap.find(boneName) == boneInfoMap.end()) {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        mBones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
    }

    mBoneInfoMap = boneInfoMap;
}

void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src) {
    dest.name = src->mName.data;
    dest.transformation = aiMatrix4x4ToGlm(src->mTransformation);
    dest.childCount = src->mNumChildren;

    for(int i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;
        readHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}