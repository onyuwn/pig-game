#ifndef BONE_HPP
#define BONE_HPP
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <iostream>

struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

class Bone
{
    private:
        std::vector<KeyPosition> mPositions;
        std::vector<KeyRotation> mRotations;
        std::vector<KeyScale> mScales;
        int mNumPositions;
        int mNumRotations;
        int mNumScalings;

        glm::mat4 mLocalTransform;
        std::string mName;
        int mId;

        float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
        glm::mat4 interpolatePosition(float animationTime);
        glm::mat4 interpolateRotation(float animationTime);
        glm::mat4 interpolateScaling(float animationTime);

    public:
        Bone(const std::string& name, int id, const aiNodeAnim* channel);
        void update(float animationTime);
        glm::mat4 getLocalTransform();
        std::string getBoneName() const;
        int getBoneId();
        int getPositionIndex(float animationTime);
        int getRotationIndex(float animationTime);
        int getScaleIndex(float animationTime);
};

#endif