#include "water.h"

GLfloat pt_strip[STRIP_COUNT * STRIP_LENGTH * 3] = { 0 };
GLfloat pt_normal[STRIP_COUNT * STRIP_LENGTH * 3] = { 0 };
GLfloat vertex_data[DATA_LENGTH * 3] = { 0 };
GLfloat normal_data[DATA_LENGTH * 3] = { 0 };

short le_short(unsigned char *bytes) {
	return bytes[0] | ((char)bytes[1] << 8);
}

const GLint gerstner_sort[6] = {
	0, 0, 1, 1, 1, 1
};

void *read_tga(const char *filename, int *width, int *height) {
	struct tga_header {
		char  id_length;
		char  color_map_type;
		char  data_type_code;
		unsigned char  color_map_origin[2];
		unsigned char  color_map_length[2];
		char  color_map_depth;
		unsigned char  x_origin[2];
		unsigned char  y_origin[2];
		unsigned char  width[2];
		unsigned char  height[2];
		char  bits_per_pixel;
		char  image_descriptor;
	} header;
	int i, color_map_size, pixels_size;
	FILE *f;
	size_t read;
	void *pixels;

	fopen_s(&f, filename, "rb");

	if (!f) {
		fprintf(stderr, "Unable to open %s for reading\n", filename);
		return NULL;
	}

	read = fread(&header, 1, sizeof(header), f);

	if (read != sizeof(header)) {
		fprintf(stderr, "%s has incomplete tga header\n", filename);
		fclose(f);
		return NULL;
	}
	if (header.data_type_code != 2) {
		fprintf(stderr, "%s is not an uncompressed RGB tga file\n", filename);
		fclose(f);
		return NULL;
	}
	if (header.bits_per_pixel != 24) {
		fprintf(stderr, "%s is not a 24-bit uncompressed RGB tga file\n", filename);
		fclose(f);
		return NULL;
	}

	for (i = 0; i < header.id_length; ++i) {
		if (getc(f) == EOF) {
			fprintf(stderr, "%s has incomplete id string\n", filename);
			fclose(f);
			return NULL;
		}
	}

	color_map_size = le_short(header.color_map_length) * (header.color_map_depth / 8);
	for (i = 0; i < color_map_size; ++i) {
		if (getc(f) == EOF) {
			fprintf(stderr, "%s has incomplete color map\n", filename);
			fclose(f);
			return NULL;
		}
	}

	*width = le_short(header.width); *height = le_short(header.height);
	pixels_size = *width * *height * (header.bits_per_pixel / 8);
	pixels = malloc(pixels_size);

	read = fread(pixels, 1, pixels_size, f);
	fclose(f);

	if (read != pixels_size) {
		fprintf(stderr, "%s has incomplete image\n", filename);
		free(pixels);
		return NULL;
	}

	return pixels;
}

void initWave() {
	values.time = 0.0;
	for (int i = 0; i < WAVE_COUNT; ++i) {
		values.wave_length[i] = wave_para[i][0];
		values.wave_height[i] = wave_para[i][1];
		values.wave_dir[i] = wave_para[i][2];
		values.wave_speed[i] = wave_para[i][3];
		values.wave_start[i * 2] = wave_para[i][4];
		values.wave_start[i * 2 + 1] = wave_para[i][5];
	}

	int index = 0;
	for (int i = 0; i < STRIP_COUNT; ++i) {
		for (int j = 0; j < STRIP_LENGTH; ++j) {
			pt_strip[index] = START_X + i * LENGTH_X;
			pt_strip[index + 1] = START_Y + j * LENGTH_Y;
			index += 3;
		}
	}
}

float gerstnerZ(float w_length, float w_height, float x_in, const GLfloat gerstner[22]) {
	x_in = x_in * 400.0 / w_length;

	while (x_in < 0.0) {
		x_in += 400.0;
	}
	while (x_in > 400.0) {
		x_in -= 400.0;
	}
	if (x_in > 200.0) {
		x_in = 400.0 - x_in;
	}


	int i = 0;
	float yScale = w_height / 50.0;
	while (i < 18 && (x_in < gerstner[i] || x_in >= gerstner[i + 2])) {
		i += 2;
	}
	if (x_in == gerstner[i]) {
		return gerstner[i + 1] * yScale;
	}
	if (x_in > gerstner[i]) {
		return ((gerstner[i + 3] - gerstner[i + 1]) * (x_in - gerstner[i]) / (gerstner[i + 2] - gerstner[i]) + gerstner[i + 3]) * yScale;
	}
}

int normalizeF(float in[], float out[], int count) {
	int t = 0;
	float l = 0.0;

	if (count <= 0.0) {
		printf("normalizeF(): Number of dimensions should be larger than zero.\n");
		return 1;
	}
	while (t < count && in[t] < 0.0000001 && in[t] > -0.0000001) {
		t++;
	}
	if (t == count) {
		printf("normalizeF(): The input vector is too small.\n");
		return 1;
	}

	for (t = 0; t < count; ++t) {
		l += in[t] * in[t];
	}
	if (l < 0.0000001) {
		l = 0.0;
		for (t = 0; t < count; ++t) {
			in[t] *= 10000.0;
		}
		for (t = 0; t < count; ++t) {
			l += in[t] * in[t];
		}
	}

	l = sqrt(l);
	for (t = 0; t < count; t++) {
		out[t] /= l; 
	}

	return 0;
}

void calcuWave() {
	int index = 0;
	float d, wave;
	for (int i = 0; i < STRIP_COUNT; i++) {
		for (int j = 0; j < STRIP_LENGTH; j++) {
			wave = 0.0;
			for (int w = 0; w < WAVE_COUNT; w++) {
				d = (pt_strip[index] - values.wave_start[w * 2] + (pt_strip[index + 1] - values.wave_start[w * 2 + 1]) * tan(values.wave_dir[w])) * cos(values.wave_dir[w]);
				if (gerstner_sort[w] == 1) {
					wave += values.wave_height[w] - gerstnerZ(values.wave_length[w], values.wave_height[w], d + values.wave_speed[w] * values.time, gerstner_ptr_a);
				}
				else {
					wave += values.wave_height[w] - gerstnerZ(values.wave_length[w], values.wave_height[w], d + values.wave_speed[w] * values.time, gerstner_ptr_b);
				}
			}
			pt_strip[index + 2] = START_Z + wave * HEIGHT_SCALE;
			index += 3;
		}
	}

	index = 0;
	for (int i = 0; i < STRIP_COUNT; i++) {
		for (int j = 0; j < STRIP_LENGTH; j++) {
			int p0 = index - STRIP_LENGTH * 3, p1 = index + 3, p2 = index + STRIP_LENGTH * 3, p3 = index - 3;
			float xa, ya, za, xb, yb, zb;
			if (i > 0) {
				if (j > 0) {
					xa = pt_strip[p0] - pt_strip[index], ya = pt_strip[p0 + 1] - pt_strip[index + 1], za = pt_strip[p0 + 2] - pt_strip[index + 2];
					xb = pt_strip[p3] - pt_strip[index], yb = pt_strip[p3 + 1] - pt_strip[index + 1], zb = pt_strip[p3 + 2] - pt_strip[index + 2];
					pt_normal[index] += ya * zb - yb * za;
					pt_normal[index + 1] += xb * za - xa * zb;
					pt_normal[index + 2] += xa * yb - xb * ya;
				}
				if (j < STRIP_LENGTH - 1) {
					xa = pt_strip[p1] - pt_strip[index], ya = pt_strip[p1 + 1] - pt_strip[index + 1], za = pt_strip[p1 + 2] - pt_strip[index + 2];
					xb = pt_strip[p0] - pt_strip[index], yb = pt_strip[p0 + 1] - pt_strip[index + 1], zb = pt_strip[p0 + 2] - pt_strip[index + 2];
					pt_normal[index] += ya * zb - yb * za;
					pt_normal[index + 1] += xb * za - xa * zb;
					pt_normal[index + 2] += xa * yb - xb * ya;
				}
			}
			if (i < STRIP_COUNT - 1) {
				if (j > 0) {
					xa = pt_strip[p3] - pt_strip[index], ya = pt_strip[p3 + 1] - pt_strip[index + 1], za = pt_strip[p3 + 2] - pt_strip[index + 2];
					xb = pt_strip[p2] - pt_strip[index], yb = pt_strip[p2 + 1] - pt_strip[index + 1], zb = pt_strip[p2 + 2] - pt_strip[index + 2];
					pt_normal[index] += ya * zb - yb * za;
					pt_normal[index + 1] += xb * za - xa * zb;
					pt_normal[index + 2] += xa * yb - xb * ya;
				}
				if (j < STRIP_LENGTH - 1) {
					xa = pt_strip[p2] - pt_strip[index], ya = pt_strip[p2 + 1] - pt_strip[index + 1], za = pt_strip[p2 + 2] - pt_strip[index + 2];
					xb = pt_strip[p1] - pt_strip[index], yb = pt_strip[p1 + 1] - pt_strip[index + 1], zb = pt_strip[p1 + 2] - pt_strip[index + 2];
					pt_normal[index] += ya * zb - yb * za;
					pt_normal[index + 1] += xb * za - xa * zb;
					pt_normal[index + 2] += xa * yb - xb * ya;
				}
			}
			if (normalizeF(&pt_normal[index], &pt_normal[index], 3))
				printf("%d\t%d\n", index / 3 / STRIP_LENGTH, (index / 3) % STRIP_LENGTH);

			index += 3;
		}
	}

	//Calculate vertex_data[] according to pt_strip[], and normal_data[] according to pt_normal[]
	int pt;
	for (int c = 0; c < (STRIP_COUNT - 1); c++) {
		for (int l = 0; l < 2 * STRIP_LENGTH; l++) {
			if (l % 2 == 1) {
				pt = c * STRIP_LENGTH + l / 2;
			}
			else {
				pt = c * STRIP_LENGTH + l / 2 + STRIP_LENGTH;
			}
			index = STRIP_LENGTH * 2 * c + l;
			for (int i = 0; i < 3; i++) {
				vertex_data[index * 3 + i] = pt_strip[pt * 3 + i];
				normal_data[index * 3 + i] = pt_normal[pt * 3 + i];
			}
		}
	}
}

GLuint initTexture(const char *filename)
{
	int width, height;
	void *pixels = read_tga(filename, &width, &height);
	GLuint texture;

	if (!pixels)
		return 0;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	free(pixels);
	return texture;
}

void initGlParams() {
	GLfloat materAmbient[] = { 0.1, 0.1, 0.3, 1.0 };
	GLfloat materSpecular[] = { 0.8, 0.8, 0.9, 1.0 };
	GLfloat lightDiffuse[] = { 0.7, 0.7, 0.8, 1.0 };
	GLfloat lightAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat envirAmbient[] = { 0.1, 0.1, 0.3, 1.0 };

}
