#include "uimaster.hpp"

UIMaster::UIMaster(unsigned int scrWidth, unsigned int scrHeight) {
    this->scrWidth = scrWidth;
    this->scrHeight = scrHeight;
    //this->pauseMenuPanel = std::make_shared<UIPanel>(scrWidth, scrHeight, 0, 0, scrWidth, scrHeight, glm::vec4(1.0,0.0,0.0,.5));
    this->pauseMenuPanel = std::make_shared<UIPanel>(scrWidth, scrHeight, 0, 0, scrWidth, scrHeight, "resources/ui/uipanel5.png");
}

void UIMaster::render(float deltaTime, float curTime) {
    //iterate over elements and render
    for(int i = 0; i < this->textElements.size(); i++) { // TODO: store text element pos in members
        this->textElements[i]->render(1, glm::vec3(1.0, 0.0, 0.0), curTime);
    }
    // finally show any urgent dialog:
    if(this->dialogShowing) {
        this->currentDialog->render(200, 200, 1, glm::vec3(1.0, 0, 0), deltaTime, glm::vec2(120,100), curTime);
    }

    if(gamePaused && this->pauseMenuPanel) { // show menu
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        checkHover(this->mousePos.x, this->mousePos.y);
        this->pauseMenuPanel->render(deltaTime, curTime);
    }
}

void UIMaster::showSceneLoadProgress(float progress) {
    
}

void UIMaster::addElement(UIElement* newElement) {
    
}

void UIMaster::addTextElement(UITextElement *newElement) {
    this->textElements.push_back(newElement);
}

void UIMaster::showDialog(std::string text) {
    if(this->dialogShowing) {
        // update text of current dialog
        this->currentDialog->updateText(text);
    } else {
        this->currentDialog = std::make_shared<DialogueElement>();
        this->dialogShowing = true;
        this->currentDialog->activate();
    }
}

void UIMaster::clearDialog() { // add callback to here to reset dialog showing
    this->currentDialog->reset();
}

void UIMaster::checkClick(double mouseX, double mouseY) { // get visible panel or iterate over all lol
    for(int i = 0; i < this->pauseMenuPanel->buttons.size(); i++) {
        glm::vec2 buttonPos = this->pauseMenuPanel->buttons[i]->getPos();
        glm::vec2 buttonSize = this->pauseMenuPanel->buttons[i]->getDims();
        if(mouseX > buttonPos.x && mouseX < buttonPos.x + buttonSize.x && mouseY > buttonPos.y && mouseY < buttonPos.y + buttonSize.y) {
            this->pauseMenuPanel->buttons[i]->click();
        }
    }
}

void UIMaster::checkHover(float mouseX, float mouseY) {
    for(int i = 0; i < this->pauseMenuPanel->buttons.size(); i++) {
        glm::vec2 buttonPos = this->pauseMenuPanel->buttons[i]->getPos();
        glm::vec2 buttonSize = this->pauseMenuPanel->buttons[i]->getDims();
        if(mouseX > buttonPos.x && mouseX < buttonPos.x + buttonSize.x && mouseY > buttonPos.y && mouseY < buttonPos.y + buttonSize.y) {
            this->pauseMenuPanel->buttons[i]->hovering = true;
        } else {
            this->pauseMenuPanel->buttons[i]->hovering = false;
        }
    }
}

void UIMaster::setMousePos(double mouseX, double mouseY) {
    this->mousePos = glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
}