#include <world_render.h>

WorldRender* WorldRender::instance = NULL;

void WorldRender::Render(Shader& shader)
{
	// Draw Sands
	int sand_count = 460, index = 0;
	int vine_count = 400, index_t = 0;
	glm::vec3 sand_position[460];
	glm::vec3 vine_position[400];

	for(int i = -10; i < 10; i++) {
		for(int j = -10; j < 10; j++) {
			if((i > 7 || i < 0 || j != -5) && (i != -5 || j < 0 || j > 4)) {
				bool enable = true;
				if (i % 5 == 1 || j % 3 == 1 || i * j % 7 == 1 || i * j % 9 == 1) {
					enable = false;
				}
				glm::vec3 pos = glm::vec3((float)i, 0.0f, (float)j);
				if (enable) {
					sand_position[index++] = pos;
				} 
				else {
					vine_position[index_t++] = pos;
				}
			}
		}
	}

	for (int i = -5; i < 5; i++) {
        for (int j = 6; j < 7; j++) {
            glm::vec3 pos = glm::vec3((float)i, 1.0f, (float)j);
            sand_position[index++] = pos;
        }
        for (int j = 7; j < 8; j++) {
            if (i > 2) {
                glm::vec3 pos = glm::vec3((float)i, 2.0f, (float)j);
                sand_position[index++] = pos;
            }
                
            else {
                glm::vec3 pos = glm::vec3((float)i, 3.0f, (float)j);
                sand_position[index++] = pos;
            }
        }
        for (int j = 8; j < 9; j++) {
            glm::vec3 pos = glm::vec3((float)i, 3.0f, (float)j);
            sand_position[index++] = pos;
        }
    }
    SingleRender(shader, sand_count, sand_position, renderer->SAND);
	SingleRender(shader, vine_count, vine_position, renderer->VINE);
}

void WorldRender::SingleRender(Shader& shader, int count, glm::vec3* position, CubeRender::BlockType block)
{
    glBindBuffer(GL_ARRAY_BUFFER, renderer->getInstanceVBO());
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * count, &position[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    renderer->RenderScene(shader, block, count);
}