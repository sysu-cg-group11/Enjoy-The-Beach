#include <cube_render.h>

#include <stb_image.h>

unsigned int CubeRender::loadTexture(const GLchar* path)
{
	unsigned int textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
        //glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        std::cout << "Cubemap texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

unsigned int* CubeRender::loadTextures(const GLchar* path, int count)
{
	unsigned int* textures = new unsigned int[count];
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        // 进行图片的切分
        int singleHeight = height / count;

        for (int i = 0; i < count; i++) {
            // Create Texture
            glGenTextures(1, &textures[i]);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, singleHeight, 0, format, GL_UNSIGNED_BYTE, data + (nrChannels * width * singleHeight) * i);
            //glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        stbi_image_free(data);
        return textures;
    }
    else
    {
        std::cout << "Cubemap texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return NULL;
    }
    return NULL;
}

void CubeRender::initTextures()
{
	textureID["sand"] = loadTexture("../resources/textures/blocks/sand.png");
	textureID["vine"] = loadTexture("../resources/textures/blocks/end_stone.png");
	multipleTextureID["water"].first = 32;
	multipleTextureID["water"].second = loadTextures("../resources/textures/blocks/water_still.png", 32);
	textureID["tree_oak_top"] = loadTexture("../resources/textures/blocks/log_oak_top.png");
    textureID["tree_oak_side"] = loadTexture("../resources/textures/blocks/log_oak.png");
    textureID["tree_birch_top"] = loadTexture("../resources/textures/blocks/log_birch_top.png");
    textureID["tree_birch_side"] = loadTexture("../resources/textures/blocks/log_birch.png");
    textureID["tree_jungle_top"] = loadTexture("../resources/textures/blocks/log_spruce_top.png");
    textureID["tree_jungle_side"] = loadTexture("../resources/textures/blocks/log_spruce.png");
    
    textureID["leave_birch"] = loadTexture("../resources/textures/blocks/leaves_birch.png");
    textureID["leave_jungle"] = loadTexture("../resources/textures/blocks/leaves_jungle.png");
    textureID["leave_oak"] = loadTexture("../resources/textures/blocks/leaves_oak.png");
}

// Render Cube
void CubeRender::RenderScene(Shader& shader, BlockType type, unsigned int drawCount)
{
    static unsigned int water_state = 0;
    static unsigned int frame = 0;
    glm::mat4 model = glm::mat4(1.0f);
    shader.SetMatrix4("model", model);

    switch (type)
    {
    	case SAND:
    		RenderCube(textureID["sand"], drawCount);
	        break;
	    case WATER:
	        RenderCube(multipleTextureID["water"].second[water_state], drawCount);
	        break;
	    case TREE_BIRCH:
	        RenderFace(textureID["tree_birch_top"], TOP, drawCount);
	        RenderFace(textureID["tree_birch_top"], BOTTOM, drawCount);
	        RenderFace(textureID["tree_birch_side"], LEFT, drawCount);
	        RenderFace(textureID["tree_birch_side"], RIGHT, drawCount);
	        RenderFace(textureID["tree_birch_side"], FRONT, drawCount);
	        RenderFace(textureID["tree_birch_side"], BACK, drawCount);
	        break;
	    case TREE_OAK:
	        RenderFace(textureID["tree_oak_top"], TOP, drawCount);
	        RenderFace(textureID["tree_oak_top"], BOTTOM, drawCount);
	        RenderFace(textureID["tree_oak_side"], LEFT, drawCount);
	        RenderFace(textureID["tree_oak_side"], RIGHT, drawCount);
	        RenderFace(textureID["tree_oak_side"], FRONT, drawCount);
	        RenderFace(textureID["tree_oak_side"], BACK, drawCount);
	        break;
	    case TREE_JUNGLE:
	        RenderFace(textureID["tree_jungle_top"], TOP, drawCount);
	        RenderFace(textureID["tree_jungle_top"], BOTTOM, drawCount);
	        RenderFace(textureID["tree_jungle_side"], LEFT, drawCount);
	        RenderFace(textureID["tree_jungle_side"], RIGHT, drawCount);
	        RenderFace(textureID["tree_jungle_side"], FRONT, drawCount);
	        RenderFace(textureID["tree_jungle_side"], BACK, drawCount);
	        break;
	    case LEAVE_BIRCH:
	        RenderCube(textureID["leave_birch"], drawCount);
	        break;
	    case LEAVE_JUNGLE:
	        RenderCube(textureID["leave_jungle"], drawCount);
	        break;
	    case LEAVE_OAK:
	        RenderCube(textureID["leave_oak"], drawCount);
	        break;
	    case VINE:
	    	RenderCube(textureID["vine"], drawCount);
    }
    if (frame <= 10) {
        frame++;
    }
    else {
        frame = 0;
        water_state = (water_state + 1) % 32;
    }
}

void CubeRender::RenderCube(unsigned int texture, unsigned int drawCount)
{
	RenderFace(texture, TOP, drawCount);
    RenderFace(texture, BOTTOM, drawCount);
    RenderFace(texture, LEFT, drawCount);
    RenderFace(texture, RIGHT, drawCount);
    RenderFace(texture, FRONT, drawCount);
    RenderFace(texture, BACK, drawCount);
}

void CubeRender::InitCube()
{
	glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAX_BLOCK, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    InitSingleCube(cubeVAO_top, cubeVBO_top, TOP);
    InitSingleCube(cubeVAO_bottom, cubeVBO_bottom, BOTTOM);
    InitSingleCube(cubeVAO_front, cubeVBO_front, FRONT);
    InitSingleCube(cubeVAO_back, cubeVBO_back, BACK);
    InitSingleCube(cubeVAO_left, cubeVBO_left, LEFT);
    InitSingleCube(cubeVAO_right, cubeVBO_right, RIGHT);
    InitSingleCube(crossVAO, crossVBO, CROSS);
}
// Tool functions
void CubeRender::InitSingleCube(unsigned int& VAO, unsigned int &VBO, RenderDir dir)
{
	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // 根据方向渲染
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (dir == TOP) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(top), top, GL_STATIC_DRAW);
    }
    else if (dir == BOTTOM) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(bottom), bottom, GL_STATIC_DRAW);
    }
    else if (dir == FRONT) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(front), front, GL_STATIC_DRAW);
    }
    else if (dir == BACK) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(back), back, GL_STATIC_DRAW);
    }
    else if (dir == LEFT) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(left), left, GL_STATIC_DRAW);
    }
    else if (dir == RIGHT) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(right), right, GL_STATIC_DRAW);
    }
    else if (dir == CROSS) {
        glBufferData(GL_ARRAY_BUFFER, sizeof(cross), cross, GL_STATIC_DRAW);
    }
    // Link vertex attributes
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CubeRender::RenderFace(unsigned int texture, RenderDir dir, unsigned int drawCount)
{
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (dir == TOP) {
        glBindVertexArray(cubeVAO_top);
    }
    else if (dir == BOTTOM) {
        glBindVertexArray(cubeVAO_bottom);
    }
    else if (dir == FRONT) {
        glBindVertexArray(cubeVAO_front);
    }
    else if (dir == BACK) {
        glBindVertexArray(cubeVAO_back);
    }
    else if (dir == LEFT) {
        glBindVertexArray(cubeVAO_left);
    }
    else if (dir == RIGHT) {
        glBindVertexArray(cubeVAO_right);
    }
    if (dir != CROSS) {
        // Render Cube
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, drawCount);
        glBindVertexArray(0);
    }
    else {
        glBindVertexArray(crossVAO);
        // Render Cube
        glDrawArraysInstanced(GL_TRIANGLES, 0, 12, drawCount);
        glBindVertexArray(0);
    }
}

unsigned int CubeRender::loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}