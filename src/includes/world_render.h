#ifndef WORLD_RENDER_H
#define WORLD_RENDER_H

#include <shader.h>
#include <cube_render.h>

class WorldRender
{
public:
	WorldRender() {
		this->renderer = new CubeRender;
		renderer->initTextures();
		renderer->InitCube();
	}
	~WorldRender() {
		delete this->renderer;
	}

	static WorldRender* getInstance() {
		if (instance == NULL){
			instance = new WorldRender;
		}
		return instance;
	}

	void Render(Shader& shader);
	void SingleRender(Shader& shader, int count, glm::vec3* position, CubeRender::BlockType block);

private:
	static WorldRender* instance;
	CubeRender* renderer;
};


#endif