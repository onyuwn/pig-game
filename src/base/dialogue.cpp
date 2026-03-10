#include "dialogue.hpp"

DialogueElement::DialogueElement() {
    this->lineIdx = 0;
    this->showing = false;
    this->dialogBox = std::make_shared<UISpriteAnim>("resources/crumpleanim/crumplesheet.png", 360, 202, 220, 10);
    this->textElement = std::make_shared<UITextElement>("resources/text/Angelic Peace.ttf", "X", 16, 225, 15);
}

void DialogueElement::activate() {
    // render first line
    std::cout << "dialogactivated\n";
    this->showing = true;
    // init dialogbox
}

void DialogueElement::render(float x, float y, float scale, glm::vec3 color, float deltaTime, glm::vec2 padding, float curTime) { // will have to come up with some sort of binary to encode all this in easily
    if(showing) {
        if(!closing) {
            this->dialogBox->render(deltaTime, ONCE_STOP);
            this->textElement->setPos(glm::vec2(this->dialogBox->getPos().x + padding.x, (this->dialogBox->getPos().y + this->dialogBox->getDims().y) - padding.y));
            this->textElement->render(scale, color, curTime);
        } else {
            this->dialogBox->render(deltaTime, REVERSE_DONE);
            if(this->dialogBox->animComplete()) {
                closing = false;
                showing = false;
            }
        }
    }
}

void DialogueElement::updateText(std::string newText) { // todo: add args for font family and size (with defaults)
    this->textElement->setText(newText);
}

void DialogueElement::advance() {
    // update text element to display next line
}

void DialogueElement::reset() {
    std::cout << "dialog reset" << std::endl;
    this->closing = true;
}