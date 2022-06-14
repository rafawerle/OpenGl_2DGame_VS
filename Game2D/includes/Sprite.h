#pragma once

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Sprite
{
public:
	Sprite();
	~Sprite() {}

	void initialize();

	void setTexture(int texID);
	void setShader(Shader* shader) { this->shader = shader; shader->Use(); }
	void setPosition(glm::vec3 pos) { this->translation = pos; } 
	void setDimension(glm::vec3 scale) { this->scale = scale; }
	void setAngle(float angle) { this->angle = angle; } 
	
	glm::vec3 getPosition();
	void setRotation(float angle, glm::vec3 axis, bool reset = true);
	void setTranslation(glm::vec3 displacements, bool reset = true);
	void setScale(glm::vec3 scaleFactors, bool reset = true);

	float getPosXInicial();
	float getPosYInicial();
	float getPosZInicial();
	
	float getPosX();
	float getPosY();
	void setPosX(float x);
	void setPosY(float y);

	void setPosXInicial(float x);
	void setPosYInicial(float y);
	void setPosZInicial(float z);

	void draw();
	void update();

	bool capturado = false;

	float tempo = 0.0f;
protected:

	GLuint VAO; 
	glm::mat4 model; 

	unsigned int texID; 

	Shader* shader;

	glm::vec3 translation, scale;
	float angle;

	float posXInicial=0, posYInicial=0, posZInicial=0;
};

