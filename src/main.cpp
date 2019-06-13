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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void drawModel(Shader& model_shader, Model& model, glm::vec3 position, glm::vec3 size = glm::vec3(1.0f), glm::vec3 rotate = glm::vec3(0.0f));

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const char *glsl_version = "#version 330";

//camera
Camera camera(glm::vec3(0.0f, 2.0f, 0.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// View position and light position
float eyeValue[3] = { 5.0, 5.0, 10.0 }, lightValue[3] = { 0.0, 50.0, 0.0 };

// Head themes location
glm::vec2 header(SCR_WIDTH / 2 - 200, SCR_HEIGHT - 80);


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

														 // glfw window creation
														 // --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Enjoy The Beach", NULL, NULL);
	if (window == NULL)
	{
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
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glEnable(GL_DEPTH_TEST);

	//setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//style
	ImGui::StyleColorsClassic();
	//binding
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Init shaders
	Shader shader("../src/shaders/shader.vs", "../src/shaders/shader.fs");
	Shader model_shader("../src/shaders/model_shader.vs", "../src/shaders/model_shader.fs");
	Shader font_shader("../src/shaders/font_shader.vs", "../src/shaders/font_shader.fs");
	

	// Set font infos
	font_shader.SetMatrix4("projection", glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT), true);
	// Create font renderer
	FontRender render(&font_shader, "../resources/Font/TimesNewRoman.ttf");
	

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces
	{
		"../resources/textures/skybox/right.jpg",
		"../resources/textures/skybox/left.jpg",
		"../resources/textures/skybox/top.jpg",
		"../resources/textures/skybox/bottom.jpg",
		"../resources/textures/skybox/back.jpg",
		"../resources/textures/skybox/front.jpg"
	};

	unsigned int cubemapTexture = CubeRender::loadCubemap(faces);
	shader.Use();
	shader.SetInteger("skybox", 0);

	// Load all .obj models
	Model sun("../resources/sun/Sun_01.obj");
	Model beach("../resources/beach/model.obj");
	Model seagull("../resources/seagull/Flying gull Texture 2.obj");

	WorldRender* world = WorldRender::getInstance();

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//start the imgui frame
		static int mode = 0;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Beach\n");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		glDepthFunc(GL_LEQUAL);
		shader.Use();

		glm::mat3 temp(camera.GetViewMatrix());
		glm::mat4 view = glm::mat4(temp);
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader.SetMatrix4("view", view);
		shader.SetMatrix4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		// Draw all imported models
		
		model_shader.Use();

		view = camera.GetViewMatrix();
		model_shader.SetMatrix4("view", view);
		model_shader.SetMatrix4("projection", projection);
		model_shader.SetVector3f("viewPos", camera.Position);
		model_shader.SetVector3f("lightColor", glm::vec3(1, 1, 1));
		model_shader.SetVector3f("lightPos", glm::vec3(lightValue[0], lightValue[1], lightValue[2]));
		model_shader.SetFloat("ambientStrength", 0.2f);
		model_shader.SetFloat("shininess", 32.0f);
		model_shader.SetFloat("diffuseFactor", 0.6f);
		model_shader.SetFloat("specularStrength", 0.8f);
		model_shader.SetInteger("diffuseTexture", 0);
		model_shader.SetInteger("shadowMap", 1);
		model_shader.SetInteger("type", 0);
		model_shader.SetVector3f("sprite_color", glm::vec3(1, 1, 1));

        GLfloat near_plane = -10.0f, far_plane = 100.0f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightSpaceMatrix = lightProjection * view;
        model_shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);

        world->drawScene(model_shader);
        world->drawObject(model_shader);

		drawModel(model_shader, sun, glm::vec3(lightValue[0], lightValue[1], lightValue[2]), glm::vec3(0.005f));
		drawModel(model_shader, beach, glm::vec3(20.0f, 2.0f, 20.0f), glm::vec3(5.0f));
        drawModel(model_shader, seagull, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.02f));
        drawModel(model_shader, seagull, glm::vec3(3.0f, 9.0f, 0.0f), glm::vec3(0.02f), glm::vec3(0.0f, 180.0f, 0.0f));

		shader.Use();
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		render.RenderText("Enjoy-The-Beach", header,
			1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

		// render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &skyboxVAO); 
	glDeleteBuffers(1, &skyboxVAO);
	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// process all input
void processInput(GLFWwindow *window)
{
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
}

//window size changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void drawModel(Shader& model_shader, Model& modelObj, glm::vec3 position, glm::vec3 size, glm::vec3 rotate)
{
	model_shader.Use();

	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, size);
	model = glm::rotate(model, rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, rotate.z, glm::vec3(0.0f, 0.0f, 1.0f));

	model_shader.SetMatrix4("model", model);
	modelObj.Draw(&model_shader);
}