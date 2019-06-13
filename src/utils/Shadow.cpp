
#include "Shadow.h"


Shadow::Shadow() {
    shadowShader = Shader("../src/shaders/shadow_vert.glsl", "../src/shaders/shadow_frag.glsl");


    glGenTextures(1, &depthMap);
    glGenFramebuffers(1, &depthMapFBO);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Shadow &Shadow::getInstance() {
    static Shadow singleton;
    return singleton;
}


std::array<GLint, 4> Shadow::bind() {
    glCullFace(GL_FRONT);
    std::array<GLint, 4> prevViewPort{};
    glGetIntegerv(GL_VIEWPORT, prevViewPort.data());
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    shadowShader.Use();

    return prevViewPort;
}

void Shadow::unbind(const std::array<GLint, 4> &prevViewPort) {
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(prevViewPort[0], prevViewPort[1], prevViewPort[2], prevViewPort[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, depthMap);
}

