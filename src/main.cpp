#include <iostream>
#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <thread>

#include <shader.h>
#include <data.h>
#include <camera.h>
#include "font_render.h"
#include "world_render.h"
#include "model.h"
#include "Shadow.h"

#include "particle_system.h"
#include "animation_system.h"
#include "water_render.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void drawModel(Shader &model_shader, Model &model, glm::vec3 position, glm::vec3 size = glm::vec3(1.0f),
               glm::vec3 rotate = glm::vec3(0.0f), glm::vec3 rotate2 = glm::vec3(0.0f));

void checkCollision();

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

//camera

Camera camera(glm::vec3(0.0f, 2.0f, 0.0f));
float lastX = (float) SCR_WIDTH / 2.0;
float lastY = (float) SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// View position and light position
float eyeValue[3] = {5.0, 5.0, 10.0}, lightValue[3] = {-12.0f, 25.0f, -12.0f};

// Head themes location
glm::vec2 header(SCR_WIDTH / 2 - 200, SCR_HEIGHT - 80);

int scene_mode = 0;
int snowman_frame = 0.0f, dir = 1.0f;
// Set player location
glm::vec3 player_pos(0.0f);
glm::vec3 player_angle(0.0f);

int have_snowman = 0, have_penguin = 0, current_cold_index = 0, current_hot_index = 0;
int have_fire = 0, have_cactus = 0;
float current_z = 0.0f;

enum GammaMode {
    ShaderGamma = 0, OpenGLGamma = 1, NoGamma = 2
} GAMMA_MODE = ShaderGamma;

bool stage_mode = false, view_mode = false;
bool player_dirs[4] = {false, false, false, false};

float shark_x = 0.0f, shark_y = 0.0f, shark_radius = 65.0f;
float prev_shark_x = 0.0f, prev_shark_y = 0.0f, shark_angle = 0.0f;

unsigned int cubemapTexture;
unsigned int cubemapTexture2;

vector<glm::vec2> snow_elements_pos;
vector<glm::vec3> fire_elements_pos;

std::vector<std::string> faces{
	"../resources/textures/skybox/right.jpg",
	"../resources/textures/skybox/left.jpg",
	"../resources/textures/skybox/top.jpg",
	"../resources/textures/skybox/bottom.jpg",
	"../resources/textures/skybox/back.jpg",
	"../resources/textures/skybox/front.jpg"
};

std::vector<std::string> night_faces{
	"../resources/textures/nightSky/right.jpg",
	"../resources/textures/nightSky/left.jpg",
	"../resources/textures/nightSky/top.jpg",
	"../resources/textures/nightSky/bottom.jpg",
	"../resources/textures/nightSky/back.jpg",
	"../resources/textures/nightSky/front.jpg"
};

std::vector<std::string> hot_faces{
    "../resources/textures/skybox/sky-hot.jpg",
    "../resources/textures/skybox/sky-hot.jpg",
	"../resources/textures/skybox/hot-top.jpg",
	"../resources/textures/skybox/hot-bottom.jpg",
	"../resources/textures/skybox/sky-hot.jpg",
	"../resources/textures/skybox/sky-hot.jpg"
};

std::vector<std::string> cold_faces{
	"../resources/textures/skybox/sky-cold.jpg",
	"../resources/textures/skybox/sky-cold.jpg",
	"../resources/textures/skybox/cold-top.jpg",
	"../resources/textures/skybox/cold-bottom.jpg",
	"../resources/textures/skybox/sky-cold.jpg",
	"../resources/textures/skybox/sky-cold.jpg"
};

const char *glsl_version = "#version 330";

const float ROTATE_SPEED = 0.55f;
float rotation = 0;
float timeing = 0;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Anti-aliasing
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif


    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Enjoy The Beach", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    //setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    //style
    ImGui::StyleColorsClassic();
    //binding
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Init shaders
    Shader shader("../src/shaders/shader.vs", "../src/shaders/shader.fs");
    Shader model_shader("../src/shaders/model_shader.vs", "../src/shaders/model_shader.fs");
    Shader font_shader("../src/shaders/font_shader.vs", "../src/shaders/font_shader.fs");

    WaterShader water_shader("../src/shaders/water_vert.glsl", "../src/shaders/water_frag.glsl");

    //water
    WaterFrameBuffers fbos;
    WaterRender waterRender(&water_shader,
                            glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                             500.0f), &fbos, lightValue);
    std::vector<WaterTile> waters;
    waters.push_back(WaterTile(75, -75, 0));


    // Set font infos
    font_shader.SetMatrix4("projection", glm::ortho(0.0f, (float) SCR_WIDTH, 0.0f, (float) SCR_HEIGHT), true);
    // Create font renderer
    FontRender render(&font_shader, "../resources/Font/TimesNewRoman.ttf");

    Snow snow;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    // build and compile our shader program
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);

    cubemapTexture = CubeRender::loadCubemap(faces);
	cubemapTexture2 = CubeRender::loadCubemap(night_faces);

    shader.Use();
	shader.SetInteger("skybox", 0);
	shader.SetInteger("skybox2", 1);

    // Load all .obj models
    Model sun("../resources/sun/Sun_01.obj");
    Model beach("../resources/beach/model.obj");
    Model player("../resources/birds/bird2/NOVELO_PARROT.obj");
    Model seagull("../resources/seagull/Flying gull Texture 2.obj");
	Model lightFish("../resources/fishes/model.obj");
	Model crocodile("../resources/fishes/Crocodile_01.obj");
	Model dolphin("../resources/fishes/Dolphin_01.obj");
	Model narwhal("../resources/fishes/Narwhal.obj");
	Model shark("../resources/fishes/shark.obj");

    Model snowMan("../resources/snowman/model.obj");
    Model chair1("../resources/chair1/Beach Chair .obj");
    Model chair2("../resources/chair2/Beach Chairs and Umbrella.obj");
    Model umbrella1("../resources/umbrella1/model.obj");
    Model umbrella2("../resources/umbrella2/model.obj");
    Model umbrella3("../resources/umbrella3/model.obj");
    
	Model coconut("../resources/coconut/Coconut.obj");
	Model coconut1("../resources/coconut/model.obj");

	Model coconut_tree1("../resources/coconut_tree/palm_tree_02.obj");
	Model coconut_tree2("../resources/coconut_tree/PalmyraPalmTree.obj");
	Model coconut_tree3("../resources/coconut_tree/PUSHILIN_palm_tree.obj");

    Model penguin("../resources/penguin/Mesh_Penguin.obj");
    Model volcano("../resources/volcano/Volcano.obj");

    // Winter Models
    Model snowMountain("../resources/mountain/WinterScene.obj");
    Model fire("../resources/fires/PUSHILIN_campfire.obj");
    Model cactus("../resources/fires/PUSHILIN_cactus.obj");

    auto &world = WorldRender::getInstance();

    snow_elements_pos.push_back(glm::vec2(10.0f, 10.0f));
    snow_elements_pos.push_back(glm::vec2(-10.0f, 14.0f));
    snow_elements_pos.push_back(glm::vec2(10.0f, -15.0f));
    snow_elements_pos.push_back(glm::vec2(-10.0f, -10.0f));

    fire_elements_pos.push_back(glm::vec3(50.0f, 7.0f, 0.0f));
    fire_elements_pos.push_back(glm::vec3(-42.0f, 9.0f, -11.0f));
    fire_elements_pos.push_back(glm::vec3(1.0f, 9.0f, -34.0f));
    fire_elements_pos.push_back(glm::vec3(-65.5f, 27.25f, -85.0f));

    AnimationController animations;
    animations.InitController();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // render loop
    while (!glfwWindowShouldClose(window)) {
        //Anti-aliasing
        glEnable(GL_MULTISAMPLE);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		//Calculating day & night
		timeing += deltaTime * 1000;
		timeing = fmod(timeing, 24000);
		int texture1;
		int texture2;
		float blendFactor;
		if (timeing >= 0 && timeing < 5000) {
			texture1 = cubemapTexture2;
			texture2 = cubemapTexture2;
			blendFactor = (timeing - 0) / float((5000 - 0));
		}
		else if (timeing >= 5000 && timeing < 8000) {
			texture1 = cubemapTexture2;
			texture2 = cubemapTexture;
			blendFactor = (timeing - 5000) / float((8000 - 5000));
		}
		else if (timeing >= 8000 && timeing < 21000) {
			texture1 = cubemapTexture;
			texture2 = cubemapTexture;
			blendFactor = (timeing - 8000) / float((21000 - 8000));
		}
		else {
			texture1 = cubemapTexture;
			texture2 = cubemapTexture2;
			blendFactor = (timeing - 21000) / float((24000 - 21000));
		}

		// Calculating frames && locations
        if (snowman_frame == 100)
            dir = -1.0f;
        else if (snowman_frame == 0)
            dir = 1.0f;

        snowman_frame += dir;
        current_z = snowman_frame * 0.02f + 1.0f;

        player_pos = glm::vec3(camera.Position.x + (view_mode ? 0.5 : 6) * camera.Front.x,
                               camera.Position.y + (view_mode ? 0.5 : 6) * camera.Front.y,
                               camera.Position.z + (view_mode ? 0.5 : 6) * camera.Front.z);
		float currentTime = glfwGetTime();
		shark_x = sin(currentTime / 10.0f) * shark_radius;
		shark_y = cos(currentTime / 10.0f) * shark_radius;

		int shark_x_mode = shark_x > 30 ? 0 : ((shark_x < -30) ? 1 : 2),
			shark_prev_x_mode = prev_shark_x > 30 ? 0 : ((prev_shark_x < -30) ? 1 : 2),
			shark_y_mode = shark_y > 30 ? 0 : ((shark_y < -30) ? 1 : 2),
			shark_prev_y_mode = prev_shark_y > 30 ? 0 : ((prev_shark_y < -30) ? 1 : 2);

		if (shark_x_mode != shark_prev_x_mode || shark_y_mode != shark_prev_y_mode)
			shark_angle += 45.0f;

		prev_shark_x = shark_x;  prev_shark_y = shark_y;

        // input
        processInput(window);
        checkCollision();

        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static int mode = 0;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Beach\n");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        ImGui::Text("%.3f", deltaTime);
        ImGui::End();

        glm::mat3 temp(camera.GetViewMatrix());
        glm::mat4 view = glm::mat4(temp);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f,
                                                500.0f);

        GLfloat near_plane = 1.00f, far_plane = 45.0f;
        glm::mat4 lightProjection = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(glm::vec3(lightValue[0], lightValue[1], lightValue[2]), glm::vec3(0.0f),
                                          glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        //Shadow map
        auto &shadow = Shadow::getInstance();
        auto prevViewport = shadow.bind();
        shadow.shadowShader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
        animations.InitDepthShader(lightSpaceMatrix);

        float v_offset = 0.0f, h_offset = 0.0f;
        if (player_dirs[0]) {
            v_offset = 0.1f;
        } else if (player_dirs[2]) {
            v_offset = -0.1f;
        }

        if (player_dirs[1]) {
            h_offset = 0.3f;
        } else if (player_dirs[3]) {
            h_offset = -0.3f;
        }

        if (!stage_mode) {
            //Draw scene from light's perspective
            shadow.shadowShader.SetInteger("useOffset", true);
            world.drawScene(shadow.shadowShader);
            //world.drawObject(shadow.shadowShader);
            shadow.shadowShader.SetInteger("useOffset", false);

            animations.RenderDepth();
            drawModel(shadow.shadowShader, player, player_pos, glm::vec3(0.01f),
                      glm::vec3(0.3f, 1.5f + h_offset, v_offset), player_angle);
            drawModel(shadow.shadowShader, beach, glm::vec3(18.0f, 2.0f, 18.0f), glm::vec3(3.0f));
            drawModel(shadow.shadowShader, seagull, glm::vec3(0.0f, 8.0f, 0.0f), glm::vec3(0.02f));
            drawModel(shadow.shadowShader, seagull, glm::vec3(3.0f, 7.0f, 0.0f), glm::vec3(0.02f),
                      glm::vec3(0.0f, 180.0f, 0.0f));

			drawModel(shadow.shadowShader, lightFish, glm::vec3(30.0f, -1.0f, 0.0f), glm::vec3(1.5f));
			drawModel(shadow.shadowShader, lightFish, glm::vec3(35.0f, -2.0f, -8.0f), glm::vec3(1.5f), glm::vec3(0.0f, 45.0f, 0.0f));
			drawModel(shadow.shadowShader, lightFish, glm::vec3(28.0f, -3.0f, -12.0f), glm::vec3(1.5f), glm::vec3(0.0f, 180.0f, 0.0f));
			drawModel(shadow.shadowShader, crocodile, glm::vec3(-22.0f, -0.5f, 15.0f), glm::vec3(0.005f), glm::vec3(0.0f, 140.0f, 0.0f));

			drawModel(shadow.shadowShader, dolphin, glm::vec3(35.0f, -2.0f, -35.0f), glm::vec3(0.5f), glm::vec3(glfwGetTime(), 0.0f, 0.0f));
			drawModel(shadow.shadowShader, narwhal, glm::vec3(0.0f, -1.0f, -25.0f), glm::vec3(1.0f), glm::vec3(0.0f, 80.0f, 0.0f));
			drawModel(shadow.shadowShader, shark, glm::vec3(shark_x, -1.5f, shark_y), glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(0.0f, shark_angle, 0.0f));

            if (have_snowman < 2) {
                drawModel(shadow.shadowShader, snowMan,
                          glm::vec3(snow_elements_pos[have_snowman].x, snowman_frame * 0.02f + 1.2f,
                                    snow_elements_pos[have_snowman].y), glm::vec3(1.0f));
            } else if (have_penguin < 2) {
                drawModel(shadow.shadowShader, penguin,
                          glm::vec3(snow_elements_pos[have_penguin + 2].x, snowman_frame * 0.02f + 1.5f,
                                    snow_elements_pos[have_penguin + 2].y), glm::vec3(0.01f));
            }

            drawModel(shadow.shadowShader, chair1, glm::vec3(7.0f, 2.0f, 15.0f), glm::vec3(0.02f));
            drawModel(shadow.shadowShader, chair1, glm::vec3(0.0f, 2.0f, 15.0f), glm::vec3(0.02f));
            drawModel(shadow.shadowShader, chair1, glm::vec3(-7.0f, 2.0f, 15.0f), glm::vec3(0.02f));

            drawModel(shadow.shadowShader, chair2, glm::vec3(-5.0f, 4.8f, -14.0f), glm::vec3(0.02f),
                      glm::vec3(0.0f, 180.0f, 0.0f));
            drawModel(shadow.shadowShader, chair2, glm::vec3(5.0f, 4.8f, -14.0f), glm::vec3(0.02f),
                      glm::vec3(0.0f, 60.0f, 0.0f));

            drawModel(shadow.shadowShader, umbrella1, glm::vec3(5.0f, 3.2f, 13.0f), glm::vec3(10.0f));
            drawModel(shadow.shadowShader, umbrella2, glm::vec3(-2.0f, 3.2f, 13.0f), glm::vec3(10.0f));
            drawModel(shadow.shadowShader, umbrella3, glm::vec3(-9.0f, 3.2f, 13.0f), glm::vec3(10.0f));

			drawModel(shadow.shadowShader, coconut, glm::vec3(0.0f, 1.0f, -15.0f), glm::vec3(0.5f));
			drawModel(shadow.shadowShader, coconut1, glm::vec3(-3.0f, 1.0f, 15.0f), glm::vec3(10.0f));

			drawModel(shadow.shadowShader, coconut_tree1, glm::vec3(10.0f, 0.0f, 11.0f), glm::vec3(0.05f));
			drawModel(shadow.shadowShader, coconut_tree1, glm::vec3(15.0f, 0.0f, -15.0f), glm::vec3(0.05f), glm::vec3(0.0f, 180.0f, 0.0f));
			drawModel(shadow.shadowShader, coconut_tree2, glm::vec3(-17.0f, 0.0f, 14.0f), glm::vec3(0.5f));
			drawModel(shadow.shadowShader, coconut_tree2, glm::vec3(-17.0f, 0.0f, -14.0f), glm::vec3(0.5f));

            drawModel(shadow.shadowShader, volcano, glm::vec3(-30.0f, -1.0f, -30.0f), glm::vec3(1.0f));
        } else {
            drawModel(shadow.shadowShader, player, player_pos, glm::vec3(0.01f),
                      glm::vec3(0.3f, 1.5f + h_offset, v_offset), player_angle);
            drawModel(shadow.shadowShader, snowMountain, glm::vec3(0.0f), glm::vec3(10.0f));
            drawModel(model_shader, player, player_pos, glm::vec3(0.01f), glm::vec3(0.3f, 1.5f + h_offset, v_offset),
                      player_angle);
            if (have_fire < 2) {
                drawModel(shadow.shadowShader, fire,
                          glm::vec3(fire_elements_pos[have_fire].x, fire_elements_pos[have_fire].y,
                                    fire_elements_pos[have_fire].z), glm::vec3(2.0f));
            } else if (have_cactus < 2) {
                drawModel(shadow.shadowShader, cactus,
                          glm::vec3(fire_elements_pos[have_cactus + 2].x, fire_elements_pos[have_cactus + 2].y,
                                    fire_elements_pos[have_cactus + 2].z), glm::vec3(2.0f));
            }
        }

        shadow.unbind(prevViewport);
		water_shader.SetVector3f("skyColour", glm::vec3(0.5, 0.5, 0.5));
        glGetIntegerv(GL_VIEWPORT, prevViewport.data());
        for (int i = 0; i < 3; ++i) {
            glEnable(GL_CLIP_DISTANCE0);
            float distance = 2 * (camera.Position.y - waters[0].getHeight());
            if (i == 0) {    //reflection
                fbos.bindReflectionFrameBuffer(prevViewport[2], prevViewport[3]);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                camera.Position.y -= distance;
                camera.Pitch = -camera.Pitch;
                shader.SetVector4f("plane", glm::vec4(0, 1, 0, -waters[0].getHeight() + 0.2f));
            } else if (i == 1) {    //refraction
                fbos.bindRefractionFrameBuffer(prevViewport[2], prevViewport[3]);

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                shader.SetVector4f("plane", glm::vec4(0, -1, 0, waters[0].getHeight()));
            } else {    //normal
                glDisable(GL_CLIP_DISTANCE0);
                fbos.unbindCurrentFrameBuffer();
            }


            //Render scene
            //Sky-box
            glDepthFunc(GL_LEQUAL);
            glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
            view = glm::mat4(temp);
            shader.Use();
			glm::mat4 tmp = view;
			rotation += ROTATE_SPEED * deltaTime;
			tmp[3] = glm::vec4(0.0);
			tmp = glm::rotate(tmp, glm::radians(rotation), glm::vec3(0.0, 1.0, 0.0));
            shader.SetMatrix4("view", tmp);
            shader.SetMatrix4("projection", projection);
			shader.SetFloat("blendFactor", blendFactor);

            glBindVertexArray(skyboxVAO);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texture2);

            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);

            glDepthFunc(GL_LESS);

            if (i == 0) {    //reflection
                model_shader.SetVector4f("plane", glm::vec4(0, 1, 0, -waters[0].getHeight()));
            } else if (i == 1) {    //refraction
                model_shader.SetVector4f("plane", glm::vec4(0, -1, 0, waters[0].getHeight()));
            }
            // Objects
            // Draw animation models
            view = camera.GetViewMatrix();

            animations.InitShader(lightSpaceMatrix, 0, 3, glm::vec3(lightValue[0], lightValue[1], lightValue[2]),
                                  camera.Position, projection, view);

            model_shader.Use();

			model_shader.SetVector3f("skyColour", glm::vec3(0.5, 0.5, 0.5));
            model_shader.SetInteger("gammaMode", GAMMA_MODE);
            model_shader.SetMatrix4("view", view);
            model_shader.SetMatrix4("projection", projection);
            model_shader.SetVector3f("viewPos", camera.Position);
            model_shader.SetVector3f("lightColor", glm::vec3(1, 1, 1));
            model_shader.SetVector3f("lightPos", glm::vec3(lightValue[0], lightValue[1], lightValue[2]));
            model_shader.SetFloat("ambientStrength", 0.45f);
            model_shader.SetFloat("shininess", 8.0f);
            model_shader.SetFloat("diffuseFactor", 0.9f);
            model_shader.SetFloat("specularStrength", 0.5f);
            model_shader.SetInteger("diffuseTexture", 0);
            model_shader.SetInteger("type", 0);
            model_shader.SetVector3f("sprite_color", glm::vec3(1, 1, 1));
            model_shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);

            model_shader.SetInteger("shadowMap", 3);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, shadow.depthMap);
            if (!stage_mode) {
                world.drawScene(model_shader);
                //world.drawObject(model_shader);

                animations.Render();
                drawModel(model_shader, player, player_pos, glm::vec3(0.01f),
                          glm::vec3(0.3f, 1.5f + h_offset, v_offset), player_angle);
                drawModel(model_shader, sun, glm::vec3(lightValue[0], lightValue[1], lightValue[2]), glm::vec3(0.005f));
                drawModel(model_shader, beach, glm::vec3(18.0f, 2.0f, 18.0f), glm::vec3(3.0f));
                drawModel(model_shader, seagull, glm::vec3(0.0f, 8.0f, 0.0f), glm::vec3(0.02f));
                drawModel(model_shader, seagull, glm::vec3(3.0f, 7.0f, 0.0f), glm::vec3(0.02f),
                          glm::vec3(0.0f, 180.0f, 0.0f));

				drawModel(model_shader, lightFish, glm::vec3(30.0f, -1.0f, 0.0f), glm::vec3(1.5f));
				drawModel(model_shader, lightFish, glm::vec3(35.0f, -2.0f, -8.0f), glm::vec3(1.5f), glm::vec3(0.0f, 45.0f, 0.0f));
				drawModel(model_shader, lightFish, glm::vec3(28.0f, -3.0f, -12.0f), glm::vec3(1.5f), glm::vec3(0.0f, 180.0f, 0.0f));
				drawModel(model_shader, crocodile, glm::vec3(-22.0f, -0.5f, 15.0f), glm::vec3(0.005f), glm::vec3(0.0f, 140.0f, 0.0f));

				drawModel(model_shader, dolphin, glm::vec3(35.0f, -2.0f, -35.0f), glm::vec3(0.5f), glm::vec3(glfwGetTime(), 0.0f, 0.0f));
				drawModel(model_shader, narwhal, glm::vec3(0.0f, -1.0f, -25.0f), glm::vec3(1.0f), glm::vec3(0.0f, 80.0f, 0.0f));
				drawModel(model_shader, shark, glm::vec3(shark_x, -1.5f, shark_y), glm::vec3(2.0f), glm::vec3(0.0f), glm::vec3(0.0f, shark_angle, 0.0f));

                if (have_snowman < 2) {
                    drawModel(model_shader, snowMan,
                              glm::vec3(snow_elements_pos[have_snowman].x, snowman_frame * 0.02f + 1.2f,
                                        snow_elements_pos[have_snowman].y), glm::vec3(1.0f));
                } else if (have_penguin < 2) {
                    drawModel(model_shader, penguin,
                              glm::vec3(snow_elements_pos[have_penguin + 2].x, snowman_frame * 0.02f + 1.5f,
                                        snow_elements_pos[have_penguin + 2].y), glm::vec3(0.01f));
                }

                drawModel(model_shader, chair1, glm::vec3(7.0f, 2.0f, 15.0f), glm::vec3(0.02f));
                drawModel(model_shader, chair1, glm::vec3(0.0f, 2.0f, 15.0f), glm::vec3(0.02f));
                drawModel(model_shader, chair1, glm::vec3(-7.0f, 2.0f, 15.0f), glm::vec3(0.02f));

                drawModel(model_shader, chair2, glm::vec3(-5.0f, 4.8f, -14.0f), glm::vec3(0.02f),
                          glm::vec3(0.0f, 180.0f, 0.0f));
                drawModel(model_shader, chair2, glm::vec3(5.0f, 4.8f, -14.0f), glm::vec3(0.02f),
                          glm::vec3(0.0f, 60.0f, 0.0f));

                drawModel(model_shader, umbrella1, glm::vec3(5.0f, 3.2f, 13.0f), glm::vec3(10.0f));
                drawModel(model_shader, umbrella2, glm::vec3(-2.0f, 3.2f, 13.0f), glm::vec3(10.0f));
                drawModel(model_shader, umbrella3, glm::vec3(-9.0f, 3.2f, 13.0f), glm::vec3(10.0f));

                drawModel(model_shader, coconut, glm::vec3(0.0f, 1.0f, -15.0f), glm::vec3(0.5f));
				drawModel(model_shader, coconut1, glm::vec3(-3.0f, 1.0f, 15.0f), glm::vec3(10.0f));

				drawModel(model_shader, coconut_tree1, glm::vec3(10.0f, 0.0f, 11.0f), glm::vec3(0.05f));
				drawModel(model_shader, coconut_tree1, glm::vec3(15.0f, 0.0f, -15.0f), glm::vec3(0.05f), glm::vec3(0.0f, 180.0f, 0.0f));
				drawModel(model_shader, coconut_tree2, glm::vec3(-17.0f, 0.0f, 14.0f), glm::vec3(0.5f));
				drawModel(model_shader, coconut_tree2, glm::vec3(-17.0f, 0.0f, -14.0f), glm::vec3(0.5f));

                drawModel(model_shader, volcano, glm::vec3(-30.0f, -1.0f, -30.0f), glm::vec3(1.0f));
            } else {
                drawModel(model_shader, sun, glm::vec3(lightValue[0], lightValue[1], lightValue[2]), glm::vec3(0.005f));
                drawModel(model_shader, snowMountain, glm::vec3(0.0f), glm::vec3(10.0f));
                drawModel(model_shader, player, player_pos, glm::vec3(0.01f),
                          glm::vec3(0.3f, 1.5f + h_offset, v_offset), player_angle);
                if (have_fire < 2) {
                    drawModel(model_shader, fire,
                              glm::vec3(fire_elements_pos[have_fire].x, fire_elements_pos[have_fire].y,
                                        fire_elements_pos[have_fire].z), glm::vec3(2.0f));
                } else if (have_cactus < 2) {
                    drawModel(model_shader, cactus,
                              glm::vec3(fire_elements_pos[have_cactus + 2].x, fire_elements_pos[have_cactus + 2].y,
                                        fire_elements_pos[have_cactus + 2].z), glm::vec3(2.0f));
                }
            }

            if (scene_mode == 1) {
                glm::mat4 model(1.0f);
                projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / SCR_HEIGHT, 0.1f, 500.f);
                // Render snow
                snow.Render(deltaTime, model, view, projection);
            }

            if (i == 0) {
                camera.Position.y += distance;
                camera.Pitch = -camera.Pitch;
            }

        }
        //water

        waterRender.render(waters, camera, deltaTime);

        //Render text
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        if (stage_mode) {
            render.RenderText("Enjoy The Snow Mountain", header,
                              1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            render.RenderText("Fire: " + to_string(have_fire), glm::vec2(SCR_WIDTH / 2 + 380, SCR_HEIGHT - 60),
                              0.5f, glm::vec3(1.0f));
            render.RenderText("Cactus: " + to_string(have_cactus), glm::vec2(SCR_WIDTH / 2 + 380, SCR_HEIGHT - 90),
                              0.5f, glm::vec3(1.0f));
        } else {
            render.RenderText("Enjoy The Beach", header,
                              1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            render.RenderText("SnowMan: " + to_string(have_snowman), glm::vec2(SCR_WIDTH / 2 + 330, SCR_HEIGHT - 60),
                              0.5f, glm::vec3(1.0f));
            render.RenderText("Penguin: " + to_string(have_penguin), glm::vec2(SCR_WIDTH / 2 + 330, SCR_HEIGHT - 90),
                              0.5f, glm::vec3(1.0f));
        }

        // render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (GAMMA_MODE == OpenGLGamma) {
            glEnable(GL_FRAMEBUFFER_SRGB);
        } else {
            glDisable(GL_FRAMEBUFFER_SRGB);
        }
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // de-allocate all resources once they've outlived their purpose:
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);
    waterRender.clean();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);

    player_angle = glm::vec3(camera.Pitch, -90 - camera.Yaw, 0.0f);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// process all input
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        scene_mode = 0;
        //view_mode = !view_mode;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        scene_mode = 1;
    static bool lightPressed = false;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        lightPressed = true;
    }
    if (lightPressed && glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
        lightValue[0] = -lightValue[0];
        lightValue[2] = -lightValue[2];
        lightPressed = false;
    }
    static bool gammaPressed = false;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        gammaPressed = true;
    }
    if (gammaPressed && glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
        switch (GAMMA_MODE) {
            case ShaderGamma:
                GAMMA_MODE = OpenGLGamma;
                break;
            case OpenGLGamma:
                GAMMA_MODE = NoGamma;
                break;
            case NoGamma:
                GAMMA_MODE = ShaderGamma;
                break;
        }
        gammaPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        player_dirs[0] = true;
    } else {
        player_dirs[0] = false;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        player_dirs[1] = true;
    } else {
        player_dirs[1] = false;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        player_dirs[2] = true;
    } else {
        player_dirs[2] = false;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        player_dirs[3] = true;
    } else {
        player_dirs[3] = false;
    }


}

//window size changed
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


void drawModel(Shader &model_shader, Model &modelObj, glm::vec3 position, glm::vec3 size, glm::vec3 rotate,
               glm::vec3 rotate2) {
    model_shader.Use();

    glm::mat4 model(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotate2.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(rotate2.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotate2.x), glm::vec3(1.0f, 0.0f, 0.0f));

    model = glm::rotate(model, rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotate.z, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, size);
    model_shader.SetMatrix4("model", model);
    modelObj.Draw(&model_shader);
}

void checkCollision() {
    int border_box = 1.0f;
    // Check beach objects collision
    if ((player_pos.x >= snow_elements_pos[current_hot_index].x - border_box &&
         player_pos.x <= snow_elements_pos[current_hot_index].x + border_box) &&
        (player_pos.y >= current_z - border_box && player_pos.y <= current_z + border_box) &&
        (player_pos.z >= snow_elements_pos[current_hot_index].y - border_box &&
         player_pos.z <= snow_elements_pos[current_hot_index].y + border_box)) {
        // Collision
        if (current_hot_index < 2) {
            have_snowman += 1;
            current_hot_index += 1;
        } else if (current_hot_index < 4) {
            have_penguin += 1;
            current_hot_index += 1;
        }
        if (current_hot_index == 4) {
            // Set jump into snow mountains
            camera.Position = glm::vec3(52.0f, 18.0f, 66.0f);
            stage_mode = true;
            scene_mode = 1;
            current_hot_index = 0;
            have_penguin = 0;
            have_snowman = 0;
            cubemapTexture = CubeRender::loadCubemap(faces);
			cubemapTexture2 = CubeRender::loadCubemap(night_faces);
        }
    }
    // Check mountain objects collision
    if ((player_pos.x >= fire_elements_pos[current_cold_index].x - border_box &&
         player_pos.x <= fire_elements_pos[current_cold_index].x + border_box) &&
        (player_pos.z >= fire_elements_pos[current_cold_index].z - border_box &&
         player_pos.z <= fire_elements_pos[current_cold_index].z + border_box) &&
        (player_pos.y >= fire_elements_pos[current_cold_index].y - border_box &&
         player_pos.y <= fire_elements_pos[current_cold_index].y + border_box)) {
        // Collision
        if (current_cold_index < 2) {
            have_fire += 1;
            current_cold_index += 1;
        } else if (current_cold_index < 4) {
            have_cactus += 1;
            current_cold_index += 1;
        }
        if (current_cold_index == 4) {
            // Set jump into snow mountains
            camera.Position = glm::vec3(0.0f, 2.0f, 0.0f);
            stage_mode = false;
            scene_mode = 0;
            current_cold_index = 0;
            have_fire = 0;
            have_cactus = 0;
            cubemapTexture = CubeRender::loadCubemap(hot_faces);
        }
    }
}