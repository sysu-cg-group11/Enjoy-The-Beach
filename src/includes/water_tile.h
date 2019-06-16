#ifndef WATER_H
#define WATER_H

class WaterTile {
public:
	float TILE_SIZE = 60;
	float height;
	float x, z;

	WaterTile(float centerX, float centerZ, float height) {
		this->x = centerX;
		this->z = centerZ;
		this->height = height;
	}

	float getHeight() {
		return height;
	}

	float getX() {
		return x;
	}

	float getZ() {
		return z;
	}
};

#endif