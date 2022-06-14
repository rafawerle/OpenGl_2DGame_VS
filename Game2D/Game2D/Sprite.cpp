#include "Sprite.h"

Sprite::Sprite()
{
	initialize();
}

void Sprite::initialize()
{
	unsigned int VBO, EBO;

	float vertices[] = {
		// posições          // cores           // textura
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0, 
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0  
	};
	
	unsigned int indices[] = {
		0, 1, 3, 
		1, 2, 3
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	model = glm::mat4(1);
	texID = -1; 
	shader = NULL; 
	translation = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	angle = 0.0f;
}

void Sprite::setTexture(int texID)
{
	this->texID = texID;
}

glm::vec3 Sprite::getPosition()
{
	return translation;
}

void Sprite::setRotation(float angle, glm::vec3 axis, bool reset)
{
	if (reset) model = glm::mat4(1);
	model = glm::rotate(model, angle, axis);
}

void Sprite::setTranslation(glm::vec3 displacements, bool reset)
{
	if (reset) model = glm::mat4(1);
	model = glm::translate(model, displacements);
}

void Sprite::setScale(glm::vec3 scaleFactors, bool reset)
{
	if (reset) model = glm::mat4(1);
	model = glm::scale(model, scaleFactors);
	scale = scaleFactors;
}

float Sprite::getPosXInicial()
{
	return posXInicial;
}

float Sprite::getPosYInicial()
{
	return posYInicial;
}

float Sprite::getPosZInicial()
{
	return posZInicial;
}

float Sprite::getPosX()
{
	return translation[0];
}

float Sprite::getPosY()
{
	return translation[1];
}

void Sprite::setPosX(float x)
{
	translation[0] = x;
}

void Sprite::setPosY(float y)
{
	translation[1] = y;
}

void Sprite::setPosXInicial(float x)
{
	posXInicial = x;
}

void Sprite::setPosYInicial(float y)
{
	posYInicial = y;
}

void Sprite::setPosZInicial(float z)
{
	posZInicial = z;
}

void Sprite::draw()
{
	//Ativando o primeiro buffer de textura (0) e conectando ao identificador gerado
	glBindTexture(GL_TEXTURE_2D, texID);
	glUniform1i(glGetUniformLocation(shader->ID, "tex_buffer"), 0);

	// Chamada de desenho - drawcall
	// Poligono Preenchido - GL_TRIANGLES
	// Observe que como  usamos EBO, agora usamos a glDrawElements!
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Sprite::update()
{
	setTranslation(translation);
	setRotation(angle, glm::vec3(0.0f, 0.0f, 1.0f), false);
	setScale(scale, false);

	GLint transformLoc = glGetUniformLocation(shader->ID, "model");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
}
