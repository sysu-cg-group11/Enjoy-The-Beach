#ifndef WATER_RENDER_H
#define WATER_RENDER_H

#include "shader.h"
#include "water_tile.h"
#include "camera.h"
#include <vector>
#include "cube_render.h"

class WaterShader : public Shader {
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
    //Issues with retina display
#ifdef __APPLE__
    const int REFLECTION_WIDTH = 2560,
            REFLECTION_HEIGHT = 1440,
            REFRACTION_WIDTH = 2560,
            REFRACTION_HEIGHT = 1440;
#else
    const int REFLECTION_WIDTH = 1280,
        REFLECTION_HEIGHT = 720,
        REFRACTION_WIDTH = 1280,
        REFRACTION_HEIGHT = 720;
#endif
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

    void bindReflectionFrameBuffer(int width, int height);

    void bindRefractionFrameBuffer(int width, int height);

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

    void render(std::vector<WaterTile> water, Camera camera, float speed);

    void unbind();

    void clean();

    void prepareRender(Camera camera, float speed);

    void setUpVAO();
};

#endif

