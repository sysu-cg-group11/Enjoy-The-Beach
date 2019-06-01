//
// Created by Miguel Chan on 2019-05-07.
//

#ifndef SHADOW_SHADERPATHS_H
#define SHADOW_SHADERPATHS_H

struct ShaderPath {
    const char *vert;
    const char *frag;
};

namespace ShaderPaths {

    const ShaderPath simpleShader = {"shaders/simple_vert.glsl", "shaders/simple_frag.glsl"};

    const ShaderPath simpleColor = {"shaders/color_vert.glsl", "shaders/color_frag.glsl"};

    const ShaderPath simpleTexture = {"shaders/texture_vert.glsl", "shaders/texture_frag.glsl"};

    const ShaderPath simpleDepth = {"shaders/simple_depth_vert.glsl", "shaders/simple_depth_frag.glsl"};

    const ShaderPath shadowMap = {"shaders/shadow_map_vert.glsl", "shaders/shadow_map_frag.glsl"};

    const ShaderPath phong = {"shaders/phong_light_vert.glsl", "shaders/phong_light_frag.glsl"};
    const ShaderPath gouraud = {"shaders/gouraud_light_vert.glsl", "shaders/gouraud_light_frag.glsl"};
    const ShaderPath blinnPhong = {"shaders/blinn_phong_light_vert.glsl", "shaders/blinn_phong_light_frag.glsl"};

}

#endif //SHADOW_SHADERPATHS_H
