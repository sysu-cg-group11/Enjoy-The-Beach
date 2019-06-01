//
// Created by Miguel Chan on 2019-03-22.
//

#include "MainLoop.h"

#include <array>
#include <ctime>
#include <thread>
#include <glm/glm.hpp>

#include "Shader/shaderPaths.h"
#include "Shader/shader.h"
#include "Utils/Utils.h"

using namespace std;

const char *glsl_version = "#version 150";

void MainLoop::loop(GLFWwindow *window) {

    int w, h;
    tie(w, h) = Utils::getScreenSize();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //Preparations here
    

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Main render loop here

        {
            ImGui::Begin("window");

            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::End();
        }


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);

        glfwPollEvents();

    }
}

