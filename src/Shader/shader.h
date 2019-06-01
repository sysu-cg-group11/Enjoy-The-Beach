//
// Created by Miguel Chan on 2019-03-06.
//

#ifndef HELLO_SHADER_H
#define HELLO_SHADER_H

#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "shaderPaths.h"


class Shader {
private:
    GLuint programId;

    static GLuint createShader(const char *name, const char *code, GLenum ShaderType);

public:
    Shader(const char *vertexPath, const char *fragPath);

    Shader(const ShaderPath &path);

    void use() const;

    GLuint getProgram() const;

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setVec2(const std::string &name, float x, float y) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec3(const std::string &name, float x, float y, float z) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec4(const std::string &name, float x, float y, float z, float w);

    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

};


#endif //HELLO_SHADER_H
