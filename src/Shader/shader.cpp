//
// Created by Miguel Chan on 2019-03-06.
//

#include "shader.h"

#include "../Utils/Utils.h"
#include <glm/ext.hpp>

using namespace Utils;
using namespace std;

Shader::Shader(const ShaderPath &path) : Shader(path.vert, path.frag) {}

Shader::Shader(const char *vertexPath, const char *fragPath) {
    ifstream vFile(vertexPath), fFile(fragPath);

    if (vFile.fail()) {
        throw GLToolsException("cannot open vertex shader file");
    }
    if (fFile.fail()) {
        throw GLToolsException("cannot open fragment shader file");
    }

    stringstream vStream, fStream;
    vStream << vFile.rdbuf();
    fStream << fFile.rdbuf();

    string vCode(vStream.str()), fCode(fStream.str());
    vFile.close();
    fFile.close();

    GLuint vertex = createShader(vertexPath, vCode.c_str(), GL_VERTEX_SHADER);
    GLuint fragment = createShader(fragPath, fCode.c_str(), GL_FRAGMENT_SHADER);

    programId = glCreateProgram();
    glAttachShader(programId, vertex);
    glAttachShader(programId, fragment);
    glLinkProgram(programId);

    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(programId, 512, nullptr, log);
        stringstream errStream;
        errStream << "link error: " << log;
        string x = errStream.str();
        throw GLToolsException(x.c_str());
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}


GLuint Shader::createShader(const char *name, const char *code, GLenum shaderType) {
    GLint success;
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        stringstream errStream;
        errStream << name << " compile error: " << log;
        string x = errStream.str();
        throw GLToolsException(x.c_str());
    }
    return shader;
}

void Shader::use() const {
    glUseProgram(programId);
}

GLuint Shader::getProgram() const {
    return programId;
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(programId, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(programId, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(programId, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}


