#include "Sprite.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#pragma once

using namespace std;

class Layer
{
public:

	void update();
	void addObject(float posX, float posY, float posZ, float dimX, float dimY, float dimZ, Shader* shader);
	void setDesloc(float deslocamento);
	float getDesloc();

	float desloc=0;

	Sprite* sprite;
	vector <Sprite*> objects;

};