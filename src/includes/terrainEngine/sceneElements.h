#ifndef SCENELEMENTS_H
#define SCENELEMENTS_H

#include <camera.h>
#include <glm/glm.hpp>
#include "buffers.h"
#include <random>

struct sceneElements {

	glm::vec3 lightPos, lightColor, lightDir, fogColor, seed;
	glm::mat4 projMatrix;
	Camera * cam;
	FrameBufferObject * sceneFBO;
	bool wireframe = false;
};

#endif