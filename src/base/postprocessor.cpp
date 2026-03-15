#include "postprocessor.hpp"

PostProcessor::PostProcessor(glm::vec2 windowDims) {
    initialize(windowDims);
}

void PostProcessor::initialize(glm::vec2 windowDims) {
    std::cout << "postprocessor initialized." << std::endl;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    // create a color attachment texture
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowDims.x * 2, windowDims.y * 2, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // TODO: idk why but need to set this to double screen dims lol
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowDims.x * 2, windowDims.y * 2);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::vector<glm::vec3> vertices = {
        glm::vec3(-1.0, 1.0, 0),
        glm::vec3(-1.0, -1.0, 0),
        glm::vec3(1.0, -1.0, 0),
        glm::vec3(-1.0, 1.0, 0),
        glm::vec3(1.0, -1.0, 0),
        glm::vec3(1.0, 1.0, 0),
    };

    std::vector<glm::vec2> uvs = {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f) 
    };

    this->quad = new UIMesh(vertices, uvs);
    // float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    //     // positions   // texCoords
    //     -1.0f,  1.0f,  0.0f, 1.0f,
    //     -1.0f, -1.0f,  0.0f, 0.0f,
    //      1.0f, -1.0f,  1.0f, 0.0f,

    //     -1.0f,  1.0f,  0.0f, 1.0f,
    //      1.0f, -1.0f,  1.0f, 0.0f,
    //      1.0f,  1.0f,  1.0f, 1.0f
    // };

    // glGenVertexArrays(1, &quadVAO);
    // glGenBuffers(1, &quadVBO);
    // glBindVertexArray(quadVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    this->screenShader = new Shader("src/shaders/screenshader.vs", "src/shaders/screenshader.fs");
    this->screenShader->use();
    this->screenShader->setInt("screenTexture",0);

    this->initialized = true;
}

void PostProcessor::begin() {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glEnable(GL_DEPTH_TEST);
}

void PostProcessor::render(float dt, glm::vec2 windowDims) { // split up first and second render passes somehow -- another method?
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    this->screenShader->use();
    this->screenShader->setFloat("time", dt);
    this->screenShader->setVec2("window", windowDims);
    //glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    this->quad->draw(*this->screenShader);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
}

void PostProcessor::setScreenSize(glm::vec2 windowDims) {
    this->initialize(screenSize);
}