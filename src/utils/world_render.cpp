#include <world_render.h>


void WorldRender::SingleRender(Shader &shader, int count, glm::vec3 *position, CubeRender::BlockType block) {
    glBindBuffer(GL_ARRAY_BUFFER, renderer->getInstanceVBO());
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * count, &position[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    renderer->RenderScene(shader, block, count);
}

void WorldRender::DrawTree(CubeRender::BlockType tree_type, CubeRender::BlockType leave_type, glm::vec3 pos,
                           Shader &shader) {
    glm::vec3 tree[5];
    int count = 5;
    int index = 0;
    for (int i = 0; i < count; i++) {
        glm::vec3 tmp = glm::vec3(pos.x, pos.y + (float) i, pos.z);
        tree[index++] = tmp;
    }
    SingleRender(shader, count, tree, tree_type);


    glm::vec3 leaves[35];
    count = 35;
    index = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = -2 + i; j <= 2 - i; j++) {
            for (int k = -2 + i; k <= 2 - i; k++) {
                glm::vec3 tmp = glm::vec3(pos.x + (float) j, pos.y + (float) (i + 4), pos.z + (float) k);
                leaves[index++] = tmp;
            }
        }
    }
    SingleRender(shader, count, leaves, leave_type);
}

//Objects will be used in shadow map
void WorldRender::drawObject(Shader &shader) {
    DrawTree(renderer->TREE_OAK, renderer->LEAVE_OAK, glm::vec3(-15.0f, 1.0f, 12.0f), shader);
	DrawTree(renderer->TREE_BIRCH, renderer->LEAVE_BIRCH, glm::vec3(15.0f, 1.0f, 12.0f), shader);
	DrawTree(renderer->TREE_JUNGLE, renderer->LEAVE_JUNGLE, glm::vec3(15.0f, 1.0f, -12.0f), shader);
	DrawTree(renderer->TREE_JUNGLE, renderer->LEAVE_JUNGLE, glm::vec3(-15.0f, 1.0f, -12.0f), shader);
}

void WorldRender::drawScene(Shader &shader) {
    // Draw Sands
	
    int sand_count = 1600, index = 0;
    glm::vec3 sand_position[1600];

    for (int i = -20; i < 20; i++) {
        for (int j = -20; j < 20; j++) {
            glm::vec3 pos = glm::vec3((float) i, 0.0f, (float) j);
            sand_position[index++] = pos;
        }
    }

    SingleRender(shader, sand_count, sand_position, renderer->SAND);
	

    // Render Water
    glm::vec3 water_position[6400];
    int water_count = 6400;
    index = 0;
    for (int i = -40; i < 40; i++) {
        for (int j = -40; j < 40; j++) {
            glm::vec3 pos = glm::vec3((float) i, 0.0f, (float) j);
            water_position[index++] = pos;
        }
    }


    SingleRender(shader, water_count, water_position, renderer->WATER);
	
}
