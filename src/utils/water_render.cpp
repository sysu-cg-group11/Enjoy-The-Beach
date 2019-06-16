#include "water_render.h"

WaterShader::WaterShader(const char *v, const char *f) : Shader(v, f) {
    std::cout << "WaterShader loaded" << std::endl;
}

void WaterShader::bindAttributes() {
    SetInteger("position", 0);
}

void WaterShader::getAllUniformLocations() {
    location_projectionMatrix = glGetUniformLocation(ID, "projectionMatrix");
    location_viewMatrix = glGetUniformLocation(ID, "viewMatrix");
    location_modelMatrix = glGetUniformLocation(ID, "modelMatrix");
}

void WaterShader::loadProjectionMatrix(glm::mat4 projection) {
    SetMatrix4("projectionMatrix", projection);
}

void WaterShader::loadViewMatrix(glm::mat4 view) {
    SetMatrix4("viewMatrix", view);
}

void WaterShader::loadModelMatrix(glm::mat4 model) {
    SetMatrix4("modelMatrix", model);
}

void WaterShader::connectTextureUnits() {
    SetInteger("reflectionTexture", 0);
    SetInteger("refractionTexture", 1);
    SetInteger("dudvMap", 2);
    SetInteger("normalMap", 3);
    SetInteger("depthMap", 4);
}

WaterFrameBuffers::WaterFrameBuffers() {
    initializeReflectionFrameBuffer();
    initializeRefractionFrameBuffer();
}

void WaterFrameBuffers::cleanUp() {
    glDeleteFramebuffers(1, &reflectionFrameBuffer);
    glDeleteTextures(1, &reflectionTexture);
    glDeleteRenderbuffers(1, &reflectionDepthBuffer);
    glDeleteFramebuffers(1, &refractionFrameBuffer);
    glDeleteTextures(1, &refractionTexture);
    glDeleteTextures(1, &refractionDepthTexture);
}

void WaterFrameBuffers::bindRefractionFrameBuffer(int width, int height) {
    bindFrameBuffer(refractionFrameBuffer, width, height);
}

void WaterFrameBuffers::bindReflectionFrameBuffer(int width, int height) {
    bindFrameBuffer(reflectionFrameBuffer, width, height);
}

void WaterFrameBuffers::unbindCurrentFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //Issues with retina display
#ifdef __APPLE__
    glViewport(0, 0, 2560, 1440);
#else
    glViewport(0, 0, 1280, 720);
#endif
}

int WaterFrameBuffers::getReflectionTexture() {
    return reflectionTexture;
}

int WaterFrameBuffers::getRefractionTexture() {
    return refractionTexture;
}

int WaterFrameBuffers::getRefractionDepthTexture() {
    return refractionDepthTexture;
}

void WaterFrameBuffers::initializeReflectionFrameBuffer() {
    //reflectionFrameBuffer
    glGenFramebuffers(1, &reflectionFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    //reflectionTexture
    glGenTextures(1, &reflectionTexture);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFLECTION_WIDTH, REFLECTION_HEIGHT, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, (void *) nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflectionTexture, 0);
    //reflectionDepthBuffer
    glGenRenderbuffers(1, &reflectionDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, REFLECTION_WIDTH, REFLECTION_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionDepthBuffer);

    unbindCurrentFrameBuffer();
}

void WaterFrameBuffers::initializeRefractionFrameBuffer() {
    //refractionFrameBuffer
    glGenFramebuffers(1, &refractionFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    //reflectionTexture
    glGenTextures(1, &refractionTexture);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, REFRACTION_WIDTH, REFRACTION_HEIGHT, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, (void *) nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, refractionTexture, 0);
    //refractionDepthTexture
    glGenTextures(1, &refractionDepthTexture);
    glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, REFRACTION_WIDTH, REFRACTION_HEIGHT,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, (void *) nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, refractionDepthTexture, 0);

    unbindCurrentFrameBuffer();
}

void WaterFrameBuffers::bindFrameBuffer(int framebuffer, int width, int height) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, width, height);
}

WaterRender::WaterRender(WaterShader *shader, glm::mat4 projectionMatrix, WaterFrameBuffers *fbos, float *lightPos) {
    this->shader = shader;
    this->fbos = fbos;
    this->lightPos = lightPos;
    dudvTexture = loadTexture(DUDV_MAP.c_str());
    normalMap = loadTexture(NORMAL_MAP.c_str());
    (*shader).Use();
    (*shader).connectTextureUnits();
    (*shader).loadProjectionMatrix(projectionMatrix);
    glUseProgram(0);
    setUpVAO();
}

void WaterRender::render(std::vector<WaterTile> water, Camera camera) {
    prepareRender(camera);
    for (auto tile : water) {
        glm::mat4 modelMatrix(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(tile.getX(), tile.getHeight(), tile.getZ()));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(tile.TILE_SIZE, tile.TILE_SIZE, tile.TILE_SIZE));
        (*shader).loadModelMatrix(modelMatrix);
        glDrawArrays(GL_TRIANGLES, 0, VERTEX_COUNT);
    }
    unbind();
}

void WaterRender::unbind() {
    glDisable(GL_BLEND);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void WaterRender::clean() {
    glDeleteVertexArrays(1, &quad);
    glDeleteBuffers(1, &quad);
}

void WaterRender::prepareRender(Camera camera) {
    (*shader).Use();
    (*shader).loadViewMatrix(camera.GetViewMatrix());
    (*shader).SetVector3f("cameraPosition", camera.Position);
    moveFactor += WAVE_SPEED * 0.017;
    if (moveFactor > 1) {
        moveFactor -= floor(moveFactor);
    }
    (*shader).SetFloat("moveFactor", moveFactor);
    (*shader).SetVector3f("lightPosition", glm::vec3(lightPos[0], lightPos[1], lightPos[2]));
    (*shader).SetVector3f("lightColour", glm::vec3(1.0, 1.0, 1.0));
    glBindVertexArray(quad);
    glEnableVertexAttribArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbos->getReflectionTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fbos->getRefractionTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dudvTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, fbos->getRefractionDepthTexture());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void WaterRender::setUpVAO() {
    float vertices[12] = {-2, 0, -2, 2, 0, 0, 0, 0, -2, 2, 0, 2};
    dimensions = 2;
    VERTEX_COUNT = 12 / 2;
    glGenVertexArrays(1, &quad);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindVertexArray(quad);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, dimensions, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
