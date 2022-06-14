#pragma once

#include "Shader.h"
#include "Sprite.h" 
#include "Layer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

using namespace std;

class Game
{
public:
	void inicia();
	void start();
	void finaliza();

private:	
	void inicializaGrafricos();
	void inicializaCenario();
	void desenhaSprites();
	bool verificaColisao(float bombX, float bombY, float characterX, float characterY);
	static void resize(GLFWwindow* window, int width, int height);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	void setupCamera2D();
	void atualizaSprites();
	unsigned int loadTexture(string filename);
	void gameOver();
	void gameWin();

	GLFWwindow *window;
	Shader *shader;
	glm::mat4 projection;
};

