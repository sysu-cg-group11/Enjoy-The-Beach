#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

#include "MainLoop.h"
#include "Utils/Utils.h"

using namespace std;

const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 600;


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lighting", NULL, NULL);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int w, int h) {
        glViewport(0, 0, w, h);
        Utils::setScreenSize(w, h);
    });
    Utils::setScreenSize(SCR_WIDTH, SCR_HEIGHT);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    try {
        MainLoop::loop(window);
    } catch (exception &err) {
        cerr << err.what() << endl;
        exit(1);
    }

    glfwDestroyWindow(window);
    // clear GLFW resources
    glfwTerminate();
    return 0;
}
