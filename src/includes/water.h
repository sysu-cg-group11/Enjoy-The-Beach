#ifndef WATER_H
#define WATER_H
#include "shader.h"
#include <fstream>
#include <iostream>

#define WAVE_COUNT 6
#define STRIP_COUNT 80
#define STRIP_LENGTH 50
#define DATA_LENGTH STRIP_LENGTH * 2 * (STRIP_COUNT - 1)
#define START_X -4.0
#define LENGTH_X 0.1
#define START_Y -2.5
#define LENGTH_Y 0.1
#define START_Z 0
#define HEIGHT_SCALE 1.6

const std::string fs_filename = "../src/shaders/water.fs";
const std::string vs_filename = "../src/shaders/water.vs";
const std::string diff_texture = "../resources/textures/water/water-texture-2.tga";
const std::string norm_texture = "../resources/textures/water/water-texture-2-normal.tga";

extern GLfloat pt_strip[STRIP_COUNT * STRIP_LENGTH * 3];
extern GLfloat pt_normal[STRIP_COUNT * STRIP_LENGTH * 3];
extern GLfloat vertex_data[DATA_LENGTH * 3];
extern GLfloat normal_data[DATA_LENGTH * 3];

const GLfloat wave_para[6][6] = {
	{ 1.6,	0.12,	0.9,	0.06,	0.0,	0.0 },
	{ 1.3,	0.1,	1.14,	0.09,	0.0,	0.0 },
	{ 0.2,	0.01,	0.8,	0.08,	0.0,	0.0 },
	{ 0.18,	0.008,	1.05,	0.1,	0.0,	0.0 },
	{ 0.23,	0.005,	1.15,	0.09,	0.0,	0.0 },
	{ 0.12,	0.003,	0.97,	0.14,	0.0,	0.0 }
};

const GLfloat gerstner_ptr_a[22] = {
	0.0, 0.0, 41.8, 1.4, 77.5, 5.2, 107.6, 10.9,
	132.4, 17.7, 152.3, 25.0, 167.9, 32.4, 179.8,
	39.2, 188.6, 44.8, 195.0, 48.5, 200.0, 50.0
};

const GLfloat gerstner_ptr_b[22] = {
	0.0, 0.0, 27.7, 1.4, 52.9, 5.2, 75.9, 10.8,
	97.2, 17.6, 116.8, 25.0, 135.1, 32.4, 152.4,
	39.2, 168.8, 44.8, 184.6, 48.5, 200.0, 50.0
};

static struct {
	GLfloat time;
	GLfloat wave_length[WAVE_COUNT],
		wave_height[WAVE_COUNT],
		wave_dir[WAVE_COUNT],
		wave_speed[WAVE_COUNT],
		wave_start[WAVE_COUNT * 2];
} values;

void *read_tga(const char *filename, int *width, int *height);

void initWave();
float gerstnerZ(float, float, float, const GLfloat *);
int normalizeF(float *, float *, int);
void calcuWave();
GLuint initTexture(const char *filename);
short le_short(unsigned char *bytes);
void initGlParams();

#endif
