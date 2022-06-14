/* :: ATIVIDADE PRATICA DE PROCESSAMENTO GRAFICO
 * :: 2022/01
 * :: 
 * :: RAFAELA CUNHA WERLE
 * ::
 * :: JOGO POMO DE OURO
 * :: 
 * :: OBJETIVO DO JOGO: CAPTURAR 5 POMOS DE OURO E DESVIAR DOS BALAÇOS.
 * ::
 */


using namespace std;

#include "Game.h"

int main()
{
	Game* game = new Game;
	game->inicia();
	game->start();
	game->finaliza();

	return 0;
}


