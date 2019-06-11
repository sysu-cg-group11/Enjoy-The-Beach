#include <world_render.h>

WorldRender* WorldRender::instance = NULL;

void WorldRender::Render(Shader& shader)
{
	// Draw Sands
	int sand_count = 400, index = 0;
	glm::vec3 sand_position[400];

	for(int i = -10; i < 10; i++) {
		for(int j = -10; j < 10; j++) {
			glm::vec3 pos = glm::vec3((float)i, 0.0f, (float)j);
			sand_position[index++] = pos;
		}
	}

    SingleRender(shader, sand_count, sand_position, renderer->SAND);

	// Render Water
	glm::vec3 water_position[100];
	int water_count = 100;
	index = 0;
	for (int i = -20; i < -10; i++) {
		for (int j = -20; j < -10; j++) {
			glm::vec3 pos = glm::vec3((float)i, 0.0f, (float)j);
			water_position[index++] = pos;
		}
	}

	
	SingleRender(shader, water_count, water_position, renderer->WATER);

	// Render Tree
	DrawTree(renderer->TREE_OAK, renderer->LEAVE_OAK, glm::vec3(-3.0f, 1.0f, 0.0f), shader);
}

void WorldRender::SingleRender(Shader& shader, int count, glm::vec3* position, CubeRender::BlockType block)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer->getInstanceVBO());
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * count, &position[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    renderer->RenderScene(shader, block, count);
}

void WorldRender::DrawTree(CubeRender::BlockType tree_type, CubeRender::BlockType leave_type, glm::vec3 pos, Shader& shader) {
	glm::vec3 tree[5];
	int count = 5;
	int index = 0;
	for (int i = 0; i < count; i++) {
		glm::vec3 tmp = glm::vec3(pos.x, pos.y + (float)i, pos.z);
		tree[index++] = tmp;
	}
	SingleRender(shader, count, tree, tree_type);


	glm::vec3 leaves[35];
	count = 35;
	index = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = -2 + i; j <= 2 - i; j++) {
			for (int k = -2 + i; k <= 2 - i; k++) {
				glm::vec3 tmp = glm::vec3(pos.x + (float)j, pos.y + (float)(i + 4), pos.z + (float)k);
				leaves[index++] = tmp;
			}
		}
	}
	SingleRender(shader, count, leaves, leave_type);
}