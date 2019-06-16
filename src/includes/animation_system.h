#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H

#include <animated_model.h>

class AnimationController
{
public:
	AnimationController();

	~AnimationController() {
		for(auto ele : this->animation_models){
			delete ele;
		}
	}

	void InitController();

	void InitShader(glm::mat4 lightSpaceMatrix, unsigned int diffuseTexture, unsigned int shadowMap,
					glm::vec3 lightPos, glm::vec3 viewPos, glm::mat4 projection, glm::mat4 view);

	void Render();

private:
	vector<AnimationModel*> animation_models;

	Shader depth_shader;
	Shader shader;
};


#endif