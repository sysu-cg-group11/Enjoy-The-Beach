#ifndef ANIMATED_MODEL_H
#define ANIMATED_MODEL_H

#include <animated_mesh.h>

#include <shader.h>

#define MAX_BONES 100

class AnimationModel {
public:
    AnimationModel(const char *path, glm::vec3 scale = glm::vec3(1.0f),
                   glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotate = glm::vec3(0.0f));

    void Draw(Shader &shader);

private:
    AnimationMesh mesh;
    float start_time;

    // Model attributes;
    glm::vec3 scale;
    glm::vec3 position;
    glm::vec3 rotate;

    glm::mat4 model;

};


#endif