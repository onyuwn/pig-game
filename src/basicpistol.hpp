#ifndef BASICPISTOR_HPP
#define BASICPISTOR_HPP
#define GLM_ENABLE_EXPIREMENTAL

#include "base/item.hpp"

class BasicPistol : public Item {
    public:
        BasicPistol(std::string name, glm::vec3 position, std::shared_ptr<Model> itemModel,
            std::shared_ptr<Shader> itemShader, float scale, std::shared_ptr<Shader> outlineShader,
            glm::vec3 holdingScaleFactor = glm::vec3(1.0));
        void use() override;
        ItemUseType getItemUseType() override;
        // void initialize() override;
        // void addToWorld(btDiscreteDynamicsWorld *world) override;
    private:
        RigidBodyEntity* itemRigidBody;
        float shootingSpeed;
        float damage;
        float range;
};

#endif