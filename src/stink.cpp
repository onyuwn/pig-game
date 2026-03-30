#include "stink.hpp"

StinkItem::StinkItem(std::string name, glm::vec3 position, std::shared_ptr<Model> itemModel,
                         std::shared_ptr<Shader> itemShader, float scale, std::shared_ptr<Shader> outlineShader,
                         glm::vec3 holdingScaleFactor)
                         : Item(name, position, itemModel, itemShader, scale, outlineShader, holdingScaleFactor) {
    this->selected = false;
    this->range = 100;
    this->damage = 10;
    this->shootingSpeed = .5;
    this->scale = scale;
}

void StinkItem::use() {

}

ItemUseType StinkItem::getItemUseType() {
    return ItemUseType::THROW;
}