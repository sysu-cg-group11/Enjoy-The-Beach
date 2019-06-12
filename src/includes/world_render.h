#ifndef WORLD_RENDER_H
#define WORLD_RENDER_H

#include <shader.h>
#include <cube_render.h>
#include <memory>


class WorldRender {
public:

    WorldRender() {
        this->renderer = new CubeRender;
        renderer->initTextures();
        renderer->InitCube();
    }

    ~WorldRender() {
        delete this->renderer;
    }

    static WorldRender *getInstance() {
        static WorldRender renderer;
        return &renderer;
    }


    void SingleRender(Shader &shader, int count, glm::vec3 *position, CubeRender::BlockType block);

    void DrawTree(CubeRender::BlockType tree_type, CubeRender::BlockType leave_type, glm::vec3 pos, Shader &shader);

    void drawObject(Shader &shader);

    void drawScene(Shader &shader);

private:
    CubeRender *renderer;
};


#endif