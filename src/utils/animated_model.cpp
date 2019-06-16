#include <animated_model.h>

AnimationModel::AnimationModel(const char *path, glm::vec3 scale, glm::vec3 position, glm::vec3 rotate) {
    this->mesh.LoadMesh(path);
    this->start_time = glfwGetTime();
    this->scale = scale;
    this->position = position;
    this->rotate = rotate;
}

void AnimationModel::Draw(Shader &shader) {
    shader.Use();
    GLuint m_boneLocation[MAX_BONES];
    for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(m_boneLocation); i++) {
        char Name[128];
        memset(Name, 0, sizeof(Name));
        SNPRINTF(Name, sizeof(Name), "gBones[%d]", i);

        m_boneLocation[i] = glGetUniformLocation(shader.ID, Name);
    }
    float RunningTime = (float) ((double) glfwGetTime() - (double) start_time);
    vector<Matrix4f> Transforms;
    this->mesh.BoneTransform(RunningTime, Transforms);

    for (uint i = 0; i < Transforms.size(); i++) {
        glUniformMatrix4fv(m_boneLocation[i], 1, GL_TRUE, (const GLfloat *) Transforms[i]);
    }

    model = glm::mat4(1.0f);
    // Basic Attributes

    float currentTime = glfwGetTime();

    glm::vec3 translation = glm::vec3(sin(currentTime) * 5.0f, 0.0f, cos(currentTime) * 5.0f);


    model = glm::rotate(model, this->rotate.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, this->rotate.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, this->rotate.z, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::rotate(model, currentTime * glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, this->position);
    model = glm::scale(model, this->scale);

    shader.SetMatrix4("model", model);
    this->mesh.Render();
}