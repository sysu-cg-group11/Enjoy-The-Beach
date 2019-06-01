#include <utility>

//
// Created by Miguel Chan on 2019-04-02.
//

#include "Utils.h"
#include <shared_mutex>

using namespace std;

namespace Utils {

    void enableDepthTest() {
        glEnable(GL_DEPTH_TEST);
    }

    void disableDepthTest() {
        glDisable(GL_DEPTH_TEST);
    }

    struct ScreenSize {
        GLuint screenWidth, screenHeight;
        mutable shared_mutex mutex;
    } screenSize;

    std::pair<GLuint, GLuint> getScreenSize() {
        shared_lock<shared_mutex> guard(screenSize.mutex);
        return {screenSize.screenWidth, screenSize.screenHeight};
    }

    void setScreenSize(GLuint w, GLuint h) {
        unique_lock<shared_mutex> guard(screenSize.mutex);
        screenSize.screenWidth = w;
        screenSize.screenHeight = h;
    }

    void printMat4(const glm::mat4 &mat) {
        for (int i = 0; i < 4; i++) {
            for (int k = 0; k < 4; k++) {
                cout << mat[k][i] << ' ';
            }
            cout << endl;
        }
        cout << endl;
    }

    void printVec3(const glm::vec3 &vec) {
        cout << vec.x << ' ' << vec.y << ' ' << vec.z << endl;
    }


    GLToolsException::GLToolsException(const char *msg) : msg(msg) {}

    const char *GLToolsException::what() const noexcept {
        return msg.c_str();
    }

    auto vertexAttribSetter = []() {
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);
        //Color
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    };

    void VertexArrayBuffer::bind() {
        VAO->bind();
        VBO->bind();
    }

    VertexArrayBuffer::VertexArrayBuffer(int size, float *data, const std::function<void()> &vertexAttribCallback) {
        VAO = make_shared<VertexArrayObject>();
        VAO->bind();
        VBO = make_shared<VertexBufferObject>(size, data);
        VAO->setAttrib(vertexAttribCallback);
    }

    GLuint VertexArrayBuffer::getVAO() {
        return VAO->VAO;
    }

    GLuint VertexArrayBuffer::getVBO() {
        return VBO->VBO;
    }

    VertexArrayBuffer::VertexArrayBuffer(std::shared_ptr<VertexBufferObject> vbo,
                                         const std::function<void()> &vertexAttribCallback) {
        VAO = make_shared<VertexArrayObject>();
        VAO->bind();
        VBO = std::move(vbo);
        VBO->bind();
        VAO->setAttrib(vertexAttribCallback);
    }


    ElementBuffer::~ElementBuffer() {
        glDeleteBuffers(1, &EBO);
    }

    ElementBuffer::ElementBuffer(GLuint VAO, int size, unsigned int *data) {
        glBindVertexArray(VAO);
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        if (size != 0 && data != nullptr) {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        }
        glBindVertexArray(0);
    }

    VertexBufferObject::VertexBufferObject(int size, float *data) {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        if (size != 0 && data != nullptr) {
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        }
    }

    VertexBufferObject::~VertexBufferObject() {
        glDeleteBuffers(1, &VBO);
    }

    void VertexBufferObject::bind() {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
    }

    void VertexBufferObject::unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    VertexArrayObject::~VertexArrayObject() {
        glDeleteVertexArrays(1, &VAO);
    }

    VertexArrayObject::VertexArrayObject() {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

    }

    void VertexArrayObject::unbind() {
        glBindVertexArray(0);
    }

    void VertexArrayObject::bind() {
        glBindVertexArray(VAO);
    }

    void VertexArrayObject::setAttrib(const std::function<void()> &vertexAttribCallback) {
        vertexAttribCallback();
    }

    FrameBuffer::~FrameBuffer() {
        glDeleteFramebuffers(1, &FBO);
    }

    FrameBuffer::FrameBuffer() {
        glGenFramebuffers(1, &FBO);
    }

    void FrameBuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }

    void FrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glm::mat4 OrthoParam::mat() {
        return glm::ortho(left, right, bottom, top, zNear, zFar);
    }

    glm::mat4 PerspectiveParam::mat() {
        return glm::perspective(fovy, aspect, zNear, zFar);
    }
}