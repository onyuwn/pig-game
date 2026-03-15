#include "bone.hpp"

Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel)
    : mName(name), mId(id), mLocalTransform(1.0f) {
        mNumPositions = channel->mNumPositionKeys;
        for(int posIdx = 0; posIdx < mNumPositions; ++posIdx) {
            aiVector3D aiPosition = channel->mPositionKeys[posIdx].mValue;
            float timeStamp = channel->mPositionKeys[posIdx].mTime;
            KeyPosition data;
            data.position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
            data.timeStamp = timeStamp;
            mPositions.push_back(data);
        }

        mNumRotations = channel->mNumRotationKeys;
        for(int rotIdx = 0; rotIdx < mNumRotations; ++rotIdx) {
            aiQuaternion aiRotation = channel->mRotationKeys[rotIdx].mValue;
            float timeStamp = channel->mRotationKeys[rotIdx].mTime;
            KeyRotation data;
            data.orientation = glm::quat(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
            data.timeStamp = timeStamp;
            mRotations.push_back(data);
        }

        mNumScalings = channel->mNumScalingKeys;
        for(int scaleIdx = 0; scaleIdx < mNumScalings; ++scaleIdx) {
            aiVector3D aiScale = channel->mScalingKeys[scaleIdx].mValue;
            float timeStamp = channel->mScalingKeys[scaleIdx].mTime;
            KeyScale data;
            data.scale = glm::vec3(aiScale.x, aiScale.y, aiScale.z);
            data.timeStamp = timeStamp;
            mScales.push_back(data);
        }
}

void Bone::update(float animationTime) {
    glm::mat4 translation = interpolatePosition(animationTime);
    glm::mat4 rotation = interpolateRotation(animationTime);
    glm::mat4 scale = interpolateScaling(animationTime);
    mLocalTransform = translation * rotation * scale;
}

glm::mat4 Bone::getLocalTransform() {return mLocalTransform;}
std::string Bone::getBoneName() const {return mName; }
int Bone::getBoneId() {return mId;}

int Bone::getPositionIndex(float animationTime) {
    for(int idx = 0; idx < mNumPositions - 1; ++idx) {
        if(animationTime < mPositions[idx + 1].timeStamp)
            return idx;
    }
}

int Bone::getRotationIndex(float animationTime) {
    for(int idx = 0; idx < mNumRotations - 1; ++idx) {
        if(animationTime < mRotations[idx + 1].timeStamp)
            return idx;
    }
}

int Bone::getScaleIndex(float animationTime) {
    for(int idx = 0; idx < mNumScalings - 1; ++idx) {
        if(animationTime < mScales[idx + 1].timeStamp)
            return idx;
    }
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

glm::mat4 Bone::interpolatePosition(float animationTime) {
    if(1 == mNumPositions)
        return glm::translate(glm::mat4(1.0f), mPositions[0].position);

    int p0Index = getPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(mPositions[p0Index].timeStamp, mPositions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(mPositions[p0Index].position, mPositions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::interpolateRotation(float animationTime) {
    if(1 == mNumRotations)
    {
        auto rotation = glm::normalize(mRotations[0].orientation);
        return glm::toMat4(rotation);
    }

    int p0Index = getRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(mRotations[p0Index].timeStamp, mRotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(mRotations[p0Index].orientation, mRotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::interpolateScaling(float animationTime) {
    if(1 == mNumScalings)
        return glm::scale(glm::mat4(1.0f), mScales[0].scale);

    int p0Index = getScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(mScales[p0Index].timeStamp, mScales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(mScales[p0Index].scale, mScales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}
