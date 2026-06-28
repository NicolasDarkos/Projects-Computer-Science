/*
    Nome: Nícolas Focking da Rosa
    Matricula: 202610268

	Quesitos implementados:
	    Obrigatórios:
	    - Renderização do tabuleiro
	    - Alternancia entre os jogadores
		- Movimentação das peças, seguindo as regras do jogo
	    - Seleção de peças e casas via teclado
		- Alocação de forma dinamica das peças e do tabuleiro

		Opcionais:
		- Detecção de peças capturadas
		- Retirada das peças capturadas

    Controles:
		seta para cima / seta para baixo : alterar tamanho do tabuleiro
		<- e -> : mover seleção
		space : selecionar peça/casa ou confirmar movimento
		x : cancelar seleção
		esc : sair do jogo
*/

#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl_canvas2d.h"
#include "auxiliar.h"

void render()
{
    CV::clear(0.5, 0.5, 0.5);

    if (!_jogoComecou)
    {
        MostrarTelaInicial();
        return;
    }

    AtualizarJogo();

    RenderizarCena();

    RenderizarDebug();
}

int main(void)
{
    CV::init(&_screenWidth, &_screenHeight, "Damas - Nícolas Focking da Rosa");

    CV::run();
}
