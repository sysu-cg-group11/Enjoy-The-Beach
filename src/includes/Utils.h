//
// Created by Miguel Chan on 2019-04-02.
//

#ifndef TRANSFORMATION_DEPTHTEST_H
#define TRANSFORMATION_DEPTHTEST_H

#include <exception>
#include <glad/glad.h>
#include <iostream>
#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

namespace Utils {
    void disableDepthTest();

    void enableDepthTest();

    std::pair<GLuint, GLuint> getScreenSize();

    void setScreenSize(GLuint w, GLuint h);

    void printMat4(const glm::mat4 &mat);

    void printVec3(const glm::vec3 &vec);

    struct OrthoParam {
        float left, right, bottom, top, zNear, zFar;
        glm::mat4 mat();
    };

    struct PerspectiveParam {
        float fovy, aspect, zNear, zFar;
        glm::mat4 mat();
    };

    struct VertexBufferObject {
        VertexBufferObject(int size, float *data);

        GLuint VBO;

        void bind();

        void unbind();

        ~VertexBufferObject();
    };

    struct VertexArrayObject {
        VertexArrayObject();

        GLuint VAO;

        void bind();

        void unbind();

        ~VertexArrayObject();

        void setAttrib(const std::function<void()> &vertexAttribCallback);
    };

    struct ElementBuffer {
        GLuint EBO;

        ElementBuffer(GLuint VAO, int size, unsigned int *data);

        ElementBuffer(const ElementBuffer &) = delete;

        ~ElementBuffer();
    };

    struct VertexArrayBuffer {
        VertexArrayBuffer(int size, float *data,
                          const std::function<void()> &vertexAttribCallback);

        VertexArrayBuffer(std::shared_ptr<VertexBufferObject> vbo,
                          const std::function<void()> &vertexAttribCallback);

        std::shared_ptr<VertexBufferObject> VBO;
        std::shared_ptr<VertexArrayObject> VAO;

        void bind();

        GLuint getVAO();

        GLuint getVBO();
    };



    struct FrameBuffer {
        GLuint FBO;

        FrameBuffer();

        void bind();

        void unbind();

        FrameBuffer(const FrameBuffer &) = delete;

        ~FrameBuffer();
    };





    class GLToolsException : public std::exception {
    private:
        //Use std::string to copy msg data in order to
        // prevent memory safety issue due to the destruction of original message data
        std::string msg;
    public:
        explicit GLToolsException(const char *msg);

        const char *what() const noexcept override;
    };
};


#endif //TRANSFORMATION_DEPTHTEST_H
