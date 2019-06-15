#ifndef ENJOY_BEACH_SHADOW_H
#define ENJOY_BEACH_SHADOW_H


#include <glad/glad.h>
#include "shader.h"
#include <array>

class Shadow {
public:
    static Shadow &getInstance();

    Shader shadowShader;
    GLuint depthMap;
    GLuint height = 4096 * 2, width = 4096 * 2;

    Shadow(const Shadow &ano) = delete;

    std::array<GLint, 4> bind();

    void unbind(const std::array<GLint, 4> &prevViewPort);

private:

    GLuint depthMapFBO;

    Shadow();
};


#endif //ENJOY_BEACH_SHADOW_H
