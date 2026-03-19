#ifndef HOURGLASSBOMP_HPP
#define HOURGLASSBOMP_HPP
#define GLM_ENABLE_EXPIREMENTAL

#include "base/item.hpp"

class HourGlassBomb : public Item {
    public:
        HourGlassBomb(std::string name, glm::vec3 position, std::shared_ptr<Model> itemModel,
            std::shared_ptr<Shader> itemShader, float scale, std::shared_ptr<Shader> outlineShader,
            glm::vec3 holdingScaleFactor = glm::vec3(1.0));
        void use() override;
        ItemUseType getItemUseType() override;
        void render(float deltaTime, glm::mat4 model = glm::mat4(1.0),
            glm::mat4 view = glm::mat4(1.0), glm::mat4 projection = glm::mat4(1.0),
            float curTime = 0.0, glm::vec3 sceneLightPos = glm::vec3(0.0)) override;
        void initializeAOE();
    private:
        RigidBodyEntity* itemRigidBody;
        btSphereShape* bombAreaOfEffectShape;
        btCollisionObject* areaOfEffect;
        float explosionDamage;
        float timeToExplosion;
        float explosionStart;
        float range;
        bool bombActive;
};

#endif