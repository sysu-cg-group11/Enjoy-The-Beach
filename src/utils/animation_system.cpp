#include <animation_system.h>

#include <Shadow.h>

AnimationController::AnimationController() {

}

void AnimationController::InitController() {
    AnimationModel *model = new AnimationModel("../resources/animation/Character Running.fbx",
                                               glm::vec3(0.005f), glm::vec3(5.0f, 0.8f, 5.0f),
                                               glm::vec3(0.0f, 0.0f, 0.0f));

    animation_models.push_back(model);

    // Init shader
    this->depth_shader = Shader("../src/shaders/animation/shadow_mapping_depth_dynamic.vs",
                                "../src/shaders/shadow_frag.glsl");
    this->shader = Shader("../src/shaders/animation/shadow_mapping_dynamic.vs",
                          "../src/shaders/animation/shadow_mapping_dynamic.fs");
}

void AnimationController::InitDepthShader(glm::mat4 lightSpaceMatrix) {
    //Shadow map
    this->depth_shader.Use();
    this->depth_shader.SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
}

void AnimationController::InitShader(glm::mat4 lightSpaceMatrix, unsigned int diffuseTexture, unsigned int shadowMap,
                                     glm::vec3 lightPos, glm::vec3 viewPos, glm::mat4 projection, glm::mat4 view) {

    this->shader.Use();
    this->shader.SetInteger("diffuseTexture", diffuseTexture);
    this->shader.SetInteger("shadowMap", shadowMap);
    this->shader.SetVector3f("lightPos", lightPos);
    this->shader.SetVector3f("viewPos", viewPos);
    this->shader.SetMatrix4("projection", projection);
    this->shader.SetMatrix4("view", view);
}

void AnimationController::RenderDepth() {
    for (auto ele : this->animation_models) {
        ele->Draw(this->depth_shader);
    }
}

void AnimationController::Render() {
    for (auto ele : this->animation_models) {
        ele->Draw(this->shader);
    }

}