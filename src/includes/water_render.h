#ifndef WATER_RENDER_H
#define WATER_RENDER_H
#include "shader.h"
#include "water_tile.h"
#include "camera.h"
#include <vector>
#include "cube_render.h"

class WaterShader : public Shader{
private:
	int location_modelMatrix,
		location_viewMatrix,
		location_projectionMatrix;

public:
	WaterShader(const char *v, const char *f);
	void bindAttributes();
	void getAllUniformLocations();
	void loadProjectionMatrix(glm::mat4 projection);
	void loadViewMatrix(glm::mat4 view);
	void loadModelMatrix(glm::mat4 model);
	void connectTextureUnits();
};

class WaterFrameBuffers {
private:
	const int REFLECTION_WIDTH = 1280,
		REFLECTION_HEIGHT = 720,
		REFRACTION_WIDTH = 1280,
		REFRACTION_HEIGHT = 720;
	//reflection
	GLuint reflectionFrameBuffer;
	GLuint reflectionTexture;
	GLuint reflectionDepthBuffer;
	//refraction
	GLuint refractionFrameBuffer;
	GLuint refractionTexture;
	GLuint refractionDepthTexture;

public:
	WaterFrameBuffers();
	void cleanUp();
	void bindReflectionFrameBuffer();
	void bindRefractionFrameBuffer();
	void unbindCurrentFrameBuffer();
	int getReflectionTexture();
	int getRefractionTexture();
	int getRefractionDepthTexture();
	void initializeReflectionFrameBuffer();
	void initializeRefractionFrameBuffer();
	void bindFrameBuffer(int framebuffer, int width, int height);
};

class WaterRender {
private:
	const std::string DUDV_MAP = "../resources/textures/water/waterDUDV.png";
	const std::string NORMAL_MAP = "../resources/textures/water/normalMap.png";
	WaterShader *shader;
	GLuint quad;
	int dimensions;
	int VERTEX_COUNT;
	WaterFrameBuffers *fbos;

	int dudvTexture;
	int normalMap;
	float WAVE_SPEED = 0.03f;
	float moveFactor = 0.0f;
	float *lightPos;

public:
	WaterRender(WaterShader *shader, glm::mat4 projectionMatrix, WaterFrameBuffers *fbos, float lightPos[3]);
	void render(std::vector<WaterTile> water, Camera camera);
	void unbind();
	void clean();
	void prepareRender(Camera camera);
	void setUpVAO();
};

#endif

