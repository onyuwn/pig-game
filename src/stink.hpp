#ifndef STINK_HPP
#define STINK_HPP
#define GLM_ENABLE_EXPIREMENTAL

#include "base/item.hpp"

class StinkItem : public Item {
    public:
        StinkItem(std::string name, glm::vec3 position, std::shared_ptr<Model> itemModel,
            std::shared_ptr<Shader> itemShader, float scale, std::shared_ptr<Shader> outlineShader,
            glm::vec3 holdingScaleFactor = glm::vec3(1.0));
        void use() override;
        ItemUseType getItemUseType() override;
        // void initialize() override;
        // void addToWorld(btDiscreteDynamicsWorld *world) override;
    private:
        float shootingSpeed;
        float damage;
        float range;
};

#endif