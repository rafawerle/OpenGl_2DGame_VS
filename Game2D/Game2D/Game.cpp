#include "Game.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

//VARIÁVEIS ESTÁTICAS
static const float larguraTela = 800.0f, alturaTela = 600.0f;
static const float repeteFundo = 10;
static float meioTela = 0.5;
static GLuint width = larguraTela, height = alturaTela;
static bool resized;

static Layer* fundo = new Layer[5];
static Layer* objetos = new Layer[3];

//INDICE DOS LAYERS
//background
const int camada0 = 0;
const int camada1 = 1;
const int camada2 = 2;
const int camada3 = 3;
const int camada4 = 4;
//objetos
const int personagem = 0;
const int pomo = 1;
const int balaco = 2;

//VARIÁVEIS DE MOVIMENTO
enum Direcao{LEFT, STOP, RIGHT, JUMPING};
static Direcao direcao = STOP;
int personagemType = 0;
float personagemY = 0.0;
const float velocidadePersonagem = 0.5f;

//VARIÁVEIS DE CONTROLE
bool inGame = true;
bool pular = false;
bool subindo = true;
bool subindoPomo = true;
bool colide = false;
//CONTADOR DE COLETAVEIS
int coletaveis = 0;


void Game::inicia()
{
	inicializaGrafricos();

	inicializaCenario();
}

void Game::inicializaGrafricos()
{
	glfwInit();

	window = glfwCreateWindow(width, height, "PG - Trabalho Pratico Grau A - Rafaela Cunha Werle", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetWindowSizeCallback(window, resize);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	shader = new Shader(((string)"../shaders/sprite.vs").c_str(), ((string)"../shaders/sprite.fs").c_str());

	resized = true;

	glEnable(GL_DEPTH);
}

void Game::inicializaCenario()
{
	//BACKGROUND
	//carregamento das texturas do Background
	unsigned int texBackground[5];
	texBackground[0] = loadTexture("../textures/fundo.png");
	texBackground[1] = loadTexture("../textures/camada1.png");
	texBackground[2] = loadTexture("../textures/camada2.png");
	texBackground[3] = loadTexture("../textures/camada3.png");
	texBackground[4] = loadTexture("../textures/camada4.png");
	//CRIAÇÃO DO FUNDO	
	fundo[camada0].addObject(larguraTela /2, alturaTela / 2, 0.0, larguraTela, alturaTela, 0.0f, shader);
	fundo[camada0].objects[0]->setTexture(texBackground[0]);
	//CRIAÇÃO DAS CAMADAS 1, 2, 3, 4
	for (int camada = 1; camada <= 4; camada++) {
		for (int aux = 0; aux < repeteFundo; aux++) {
			fundo[camada].addObject((larguraTela * (aux + 1)) - 400, alturaTela / 2, 0.0, larguraTela, alturaTela, 0.0f, shader);
			fundo[camada].objects[aux]->setTexture(texBackground[camada]);
		}
	}
	//fator de deslocamento das camadas
	fundo[camada2].setDesloc(-20.0f);
	fundo[camada1].setDesloc(-15.0f);
	fundo[camada3].setDesloc(-5.0f);
	fundo[camada4].setDesloc(-5.0f);
	
	//PERSONAGEM
	objetos[personagem].addObject(50.0f, 180.0f, 0.0f, 100.0f, 120.0f, 0.0f, shader);
	objetos[personagem].setDesloc(0.2f);
	unsigned int texID = loadTexture("../textures/hpesq.png");
	objetos[personagem].objects[0]->setTexture(texID);
	objetos[personagem].objects[0]->tempo = 100.0f;
	personagemY = 180.0f;
	//
	objetos[personagem].addObject(-100.0f, 180.0f, 0.0f, 100.0f, 120.0f, 0.0f, shader);
	objetos[personagem].setDesloc(0.2f);
	texID = loadTexture("../textures/hpdir.png");
	objetos[personagem].objects[1]->setTexture(texID);
	objetos[personagem].objects[1]->tempo = 100.0f;

	//COLETAVEIS
	texID = loadTexture("../textures/pomo.png");
	for (int aux = 0; aux < repeteFundo *2; aux++) {
		//sorteia posição
		for (int l = 0; l < 1; l++) {
			float drawnNumber = rand() % 100;
			float width = (drawnNumber / 100.0f) * larguraTela;
			objetos[pomo].addObject((larguraTela * aux) + width, 400.0f, 0.0f, 40.0f, 30.0f, 0.0f, shader);
			objetos[pomo].objects[aux]->setTexture(texID);
		}
	}
	objetos[pomo].setDesloc(-40.0f);

	
	//OBSTACULO
	texID = loadTexture("../textures/balaco.png");	
	//posições definidas
	int posicao[3] = { 150, 400, 700 };
	for (int i = 0; i < 3; i++) {			
		objetos[balaco].addObject(posicao[i], alturaTela + 150*i, 0.0f, 50.0f, 50.0f, 0.0f, shader);
		objetos[balaco].objects[i]->setTexture(texID);		
	}
	objetos[balaco].setDesloc(-25.0f);	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Game::start()
{
	while (!glfwWindowShouldClose(window) && inGame)
	{
		if (coletaveis == 5) {
			gameWin();
		}
		else 
		if (colide) {
			gameOver();
		}
		else {
			glfwPollEvents();

			atualizaSprites();
			desenhaSprites();

			glfwSwapBuffers(window);
		}
	}
}

void Game::atualizaSprites()
{
	//IDENTIFICAÇÃO DE QUAL TIPO DE TEXTURA USAR PARA O PERSONAGEM (DIREITA - ESQUERDA)
	if (personagemType == 0) {
		objetos[personagem].objects[0]->setPosition(glm::vec3(objetos[personagem].objects[0]->getPosX(), personagemY, objetos[personagem].objects[0]->getPosZInicial()));
		objetos[personagem].objects[1]->setPosition(glm::vec3(objetos[personagem].objects[1]->getPosX(), -100.0f, objetos[personagem].objects[1]->getPosZInicial()));
	}
	if (personagemType == 1) {
		objetos[personagem].objects[0]->setPosition(glm::vec3(objetos[personagem].objects[0]->getPosX(), -100.0f, objetos[personagem].objects[0]->getPosZInicial()));
		objetos[personagem].objects[1]->setPosition(glm::vec3(objetos[personagem].objects[0]->getPosX(), personagemY, objetos[personagem].objects[1]->getPosZInicial()));
	}

	//IDENTIFICAÇÃO DE QUAL MOVIMENTO O PERSONAGEM ESTA REALIZANDO NO MOMENTO
	switch (direcao)
	{
	case LEFT:
		if (objetos[personagem].objects[personagemType]->getPosX() > 50) {
			objetos[personagem].objects[0]->setPosX(objetos[personagem].objects[0]->getPosX() - velocidadePersonagem);
			objetos[personagem].objects[1]->setPosX(objetos[personagem].objects[1]->getPosX() - velocidadePersonagem);
		}
		break;
	case RIGHT:
		if (objetos[personagem].objects[personagemType]->getPosX() < larguraTela - 50) {
			objetos[personagem].objects[0]->setPosX(objetos[personagem].objects[0]->getPosX() + velocidadePersonagem);
			objetos[personagem].objects[1]->setPosX(objetos[personagem].objects[1]->getPosX() + velocidadePersonagem);
		}
		break;
	case JUMPING:
		if (!pular) {
			pular = true;
		}
	default:
		break;
	}

	//MOVIMETO DE PULAR DO PERSONAGEM
	if (pular && objetos[personagem].objects[personagemType]->tempo >= 5.0) {

		if (subindo) {
			objetos[personagem].objects[personagemType]->setPosY(objetos[personagem].objects[personagemType]->getPosY() + 0.7f);
		}
		else {
			objetos[personagem].objects[personagemType]->setPosY(objetos[personagem].objects[personagemType]->getPosY() - 0.6f);
		}

		personagemY = objetos[personagem].objects[personagemType]->getPosY();

		if (objetos[personagem].objects[personagemType]->getPosY() > 350) {
			subindo = false;
		}
		else if (objetos[personagem].objects[personagemType]->getPosY() < 180) {
			pular = false;
			subindo = true;
			objetos[personagem].objects[0]->tempo = 0.0f;
			objetos[personagem].objects[1]->tempo = 0.0f;
		}
	}

	//MOVIMENTO DO CENÁRIO
	//mesmo padrão de movimento para as 4 camadas de fundo.
	//mudando apenas o fator de deslocamento especificado na criação.
	for (int camada = 1; camada <= 4; camada++) {  
		for (int aux = 0; aux < repeteFundo; aux++) {
			fundo[camada].objects[aux]->setPosition(glm::vec3(fundo[camada].objects[aux]->getPosXInicial() + fundo[camada].objects[aux]->tempo * fundo[camada].getDesloc(), alturaTela / 2, 0.0f));
			fundo[camada].objects[aux]->tempo += 0.01f;
		}
	}

	//MOVIMENTO DOS COLETÁVEIS
	for (int aux = 0; aux < repeteFundo*2; aux++) {
		
		if (!objetos[pomo].objects[aux]->capturado) {

			float bombY = (int)objetos[pomo].objects[aux]->getPosY();
			float bombX = (int)objetos[pomo].objects[aux]->getPosX();
			float characterX = (int)objetos[personagem].objects[personagemType]->getPosX();
			float characterY = (int)objetos[personagem].objects[personagemType]->getPosY();

			if (verificaColisao(bombX, bombY, characterX, characterY)) {
				coletaveis++;
				objetos[pomo].objects[aux]->capturado = true;				
			}

			if (subindoPomo)
				objetos[pomo].objects[aux]->setPosY(objetos[pomo].objects[aux]->getPosY() + 0.3f);
			else
				objetos[pomo].objects[aux]->setPosY(objetos[pomo].objects[aux]->getPosY() - 0.3f);
			if (objetos[pomo].objects[aux]->getPosY() > 450)
				subindoPomo = false;
			else if (objetos[pomo].objects[aux]->getPosY() < 350)
				subindoPomo = true;

			objetos[pomo].objects[aux]->setPosition(glm::vec3(objetos[pomo].objects[aux]->getPosXInicial() + objetos[pomo].objects[aux]->tempo * objetos[pomo].getDesloc(), objetos[pomo].objects[aux]->getPosY(), objetos[pomo].objects[aux]->getPosZInicial()));
			objetos[pomo].objects[aux]->tempo += 0.01f;

		}
	}

	//MOVIMENTO DOS OBSTACULOS
	for (int aux = 0; aux < 3; aux++) {
		float objY = (int)objetos[balaco].objects[aux]->getPosY();
		float objX = (int)objetos[balaco].objects[aux]->getPosX();
		float characterX = (int)objetos[personagem].objects[personagemType]->getPosX();
		float characterY = (int)objetos[personagem].objects[personagemType]->getPosY();

		if (verificaColisao(objX, objY, characterX, characterY)) {
			colide = true;
		}

		if (objetos[balaco].objects[aux]->getPosY() < -50 )
			objetos[balaco].objects[aux]->tempo = 0.0f;

		objetos[balaco].objects[aux]->setPosition(glm::vec3(objetos[balaco].objects[aux]->getPosX(), objetos[balaco].objects[aux]->getPosYInicial() + objetos[balaco].objects[aux]->tempo * objetos[balaco].getDesloc(), objetos[balaco].objects[aux]->getPosZInicial()));
		objetos[balaco].objects[aux]->tempo += 0.01f;
	}

	//ATUALIZANDO MOVIMENTO DO PERSONAGEM
	for (int aux = 0; aux < 2; aux++) {
		objetos[personagem].objects[aux]->setPosition(glm::vec3(objetos[personagem].objects[aux]->getPosX(), objetos[personagem].objects[aux]->getPosY(), objetos[personagem].objects[aux]->getPosZInicial()));
		objetos[personagem].objects[aux]->tempo += 0.025f;
	}
}

void Game::desenhaSprites()
{
	glClearColor(0.89f, 0.89f, 0.87f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (resized) {
		setupCamera2D();
		resized = false;
	}

	for (int aux = 0; aux < 5; aux++) {
		fundo[aux].update();
	}

	for (int aux = 0; aux < 3; aux++) {
		objetos[aux].update();
	}
}

bool Game::verificaColisao(float bombX, float bombY, float characterX, float characterY) {

	if ((bombX + 40 > characterX && bombX < characterX + 50) &&
		(bombY + 20 > characterY - 50 && bombY - 20 < characterY + 50)) {
		return true;
	}
	else return false;
}

void Game::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		direcao = RIGHT;
		personagemType = 0;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		direcao = LEFT;
		personagemType = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		direcao = JUMPING;
		personagemType = 0;
	}
	else
	{
		direcao = STOP;
		personagemType = 0;
	}

}

void Game::resize(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	resized = true;
	glViewport(0, 0, width, height);
}

void Game::setupCamera2D()
{
	projection = glm::ortho(0.0f, larguraTela, 0.0f, alturaTela, -1.0f, 1.0f);
	GLint projLoc = glGetUniformLocation(shader->ID, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

unsigned int Game::loadTexture(string filename)
{
	unsigned int texture;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	//Ajusta os parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	return texture;
}

void Game::gameOver(){
	
	//mensagem de gamer over na tela
	
	inGame = false;
}

void Game::gameWin() {
	
	//mensagem de vitória na tela

	inGame = false;
}

void Game::finaliza()
{
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela.
	glfwTerminate();
}