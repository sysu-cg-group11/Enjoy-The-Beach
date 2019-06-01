//
// Created by Miguel Chan on 2019-03-22.
//

#ifndef DRAWLINE_MAINLOOP_H
#define DRAWLINE_MAINLOOP_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace MainLoop {

    void loop(GLFWwindow *window);

};


#endif //DRAWLINE_MAINLOOP_H
