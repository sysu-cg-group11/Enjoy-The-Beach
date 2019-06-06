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
#include <model.h>
#include <font_render.h>

#include <terrain/Config.h>
#include <terrain/Simulation.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void drawModel(Shader& model_shader, Model& model, glm::vec3 position, glm::vec3 size = glm::vec3(1.0f), glm::vec3 rotate = glm::vec3(0.0f));

unsigned int loadTexture(const char *path);
unsigned int loadCubemap(std::vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;
const char *glsl_version = "#version 330";

//camera
Camera camera(glm::vec3(0.0f, 10.0f, 10.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// View position and light position
float eyeValue[3] = { 5.0, 5.0, 10.0 }, lightValue[3] = { 0.0, 30.0, 0.0 };

// Head themes location
glm::vec2 header(SCR_WIDTH / 2 - 200, SCR_HEIGHT - 80);

Simulation simulation;

void bind_mouse(GLFWwindow *window, double x, double y) { simulation.onMouseMove(window, x, y); }

void bind_resize(GLFWwindow *window, int width, int height) {
	(void)width, (void)height;
	simulation.onResize(window);
}

void bind_key(GLFWwindow *window, int key, int scancode, int action, int mods) {
	simulation.onKey(window, key, scancode, action, mods);
}

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
		"../resources/textures/skybox1/right.jpg",
		"../resources/textures/skybox1/left.jpg",
		"../resources/textures/skybox1/top.jpg",
		"../resources/textures/skybox1/bottom.jpg",
		"../resources/textures/skybox1/back.jpg",
		"../resources/textures/skybox1/front.jpg"
	};

	unsigned int cubemapTexture = loadCubemap(faces);
	shader.Use();
	shader.SetInteger("skybox", 0);

	// Load all .obj models
	Model mainBeach("../resources/beach/Beach.obj");
	Model smallBeach("../resources/beach/model.obj");
	Model seagull("../resources/seagull/Flying gull Texture 2.obj");
	Model wave("../resources/water/Wave.obj");
	// Character birds
	Model bird("../resources/birds/bird2/NOVELO_PARROT.obj");
	
	simulation.init(window);

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
		ImGui::Begin("Skybox\n");
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();


		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		shader.Use();

		shader.SetMatrix4("view", view);
		shader.SetMatrix4("projection", projection);

		// Draw all imported models
		model_shader.Use();

		model_shader.SetMatrix4("view", view);
		model_shader.SetMatrix4("projection", projection);
		model_shader.SetVector3f("viewPos", camera.Position);
		model_shader.SetVector3f("lightColor", glm::vec3(1, 1, 1));
		model_shader.SetVector3f("lightPos", glm::vec3(lightValue[0], lightValue[1], lightValue[2]));

		// Draw Models
		drawModel(model_shader, mainBeach, glm::vec3(35.0f, 6.0f, 20.0f), glm::vec3(0.000011f, 0.00002f, 0.00002f));
		drawModel(model_shader, smallBeach, glm::vec3(5.0f, 7.8f, 5.0f), glm::vec3(8.0f, 8.0f, 8.0f));
		drawModel(model_shader, wave, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.00011f, 0.00004f, 0.00011f));
		// Add seagulls
		drawModel(model_shader, seagull, glm::vec3(0.0f, 12.0f, 0.0f), glm::vec3(0.01f));
		drawModel(model_shader, seagull, glm::vec3(3.0f, 13.0f, 0.0f), glm::vec3(0.01f), glm::vec3(0.0f, 180.0f, 0.0f));
		// Add Player character bird
		//drawModel(model_shader, bird, camera.Position, glm::vec3(0.01f), camera.Front + camera.Position);


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
		
		
		//simulation.display();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	simulation.cleanUp();
	/*
	// de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &skyboxVAO); 
	glDeleteBuffers(1, &skyboxVAO);
	*/
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

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
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