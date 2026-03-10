#ifndef DIALOGUE_HPP
#define DIALOGUE_HPP
#include "uitextelement.hpp"
#include "uispriteanim.hpp"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "spriteanimationtype.hpp"

class DialogueElement {
    public:
        DialogueElement();
        void activate();
        void advance();
        void reset();
        void render(float x, float y, float scale, glm::vec3 color, float deltaTime, glm::vec2 padding, float curTime);
        void updateText(std::string newText);
        bool showing;
        bool closing;
        bool open;
    private:
        std::vector<std::string> lines;
        std::shared_ptr<UITextElement> textElement;
        std::shared_ptr<UISpriteAnim> dialogBox;
        unsigned int lineIdx;
};

#endif