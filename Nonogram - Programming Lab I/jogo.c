#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h> 

#include "util.h" 

#define LARGURA_JANELA 800
#define ALTURA_JANELA 800
#define TITULO_JANELA "Nonogram"

#define LINHAS 8
#define COLUNAS 8
#define ALTURA_CELULA 50
#define LARGURA_CELULA 50
#define LARGURA_TABULEIRO LARGURA_CELULA * COLUNAS
#define ALTURA_TABULEIRO ALTURA_CELULA * LINHAS
#define MARGIN_HORIZONTAL (int) ((LARGURA_JANELA / 2) - (LARGURA_TABULEIRO / 2))
#define MARGIN_VERTICAL  (int) ((ALTURA_JANELA / 2) - (ALTURA_TABULEIRO / 2))

int linhasPintadas[8][8] = {0};
int colunasPintadas[8][8] = {0};
int numeroDeCelulasPintadas = 0;
int numeroDeCelulasCertasSelecionadas = 0, numeroDeCelulasErradasSelecionadas = 0, numeroDeCelulasCertasSelecionadasAntigos = 0, numeroDeCelulasErradasSelecionadasAntigos = 0;
int vida = 3;
int modoJogo = 0;
char mensagem[50]; //usei o chatgpt para corrigir um problema de estouro de buffer devido ao tamanho da varivavel

int delayCliqueTabuleiro = 0; //usei chatgpt para criar um delay para evitar que o jogador clique sem querer em uma celula ao escolher o modo de jogo

bool ganhou = false;

#define MARGEM_BOTAO 200
#define NUMERO_BOTOES 3
#define POSICAO_BOTAO_X (LARGURA_JANELA / 2 - MARGEM_BOTAO * ((NUMERO_BOTOES/2.0) + 1)) // posicao do botao mais a esquerda (botao reset)
#define POSICAO_BOTAO_Y (ALTURA_JANELA - 100)

Botao botaoReset, botaoLimpar, botaoResolver, botaoClassico, botaoNormal, botaoJogarNovamente, botaoDesfazer;
bool desfez = false;

Celula tabuleiro_nonogram_antigo[LINHAS][COLUNAS];

void inicia_tabuleiro(Celula tabuleiro[][COLUNAS]);
void inicia_linha(Celula tabuleiro[][COLUNAS]);
void inicia_colunas(Celula tabuleiro[][COLUNAS]);
void inicia_jogo(Celula tabuleiro[][COLUNAS]);
void inicia_botao(Botao *b, int idBotao);
void inicia_botao_menu(Botao *b, int idBotao);
void desenhaMenu(Tela *t);
void desenhaJogo(Tela *t, Celula tabuleiro[][COLUNAS]);
void desenha_tabuleiro(Tela *t, Celula tabuleiro[][COLUNAS]);
void verifica_clique(Tela *t, Celula tabuleiro[][COLUNAS]);
void verifica_cliqueBotao(Ponto *mouse, Botao *b, Celula tabuleiro[][COLUNAS]);
void verifica_cliqueBotaoMenu(Ponto *mouse, Botao *b, Celula tabuleiro[][COLUNAS]);
void escreve_numeros(Tela *t, int *numeros, int qtd, Ponto inicio, bool horizontal);
void cicla_estado_celula(Celula *c, bool errou);
void atualizaTabuleiroAntigo(Celula tabuleiro[][COLUNAS]);

void jogarModoClassico(Tela *t, Celula tabuleiro[][COLUNAS]);
void jogarModoNormal(Tela *t, Celula tabuleiro[][COLUNAS]);
void mostrarTelaFinal(Tela *t);

int main(int argc, char **argv) {
    srand(time(NULL));
    int tecla_pressionada;
    Tela t;
    Celula tabuleiro_nonogram[LINHAS][COLUNAS];

    inicia_jogo(tabuleiro_nonogram);
    inicia_botao(&botaoReset, 1);
    inicia_botao(&botaoLimpar, 2);
    inicia_botao(&botaoResolver, 3);
    inicia_botao(&botaoDesfazer, 4);

    inicia_botao_menu(&botaoNormal, 1);
    inicia_botao_menu(&botaoClassico, 2);
    inicia_botao_menu(&botaoJogarNovamente, 3);

    modoJogo = 0; 

    inicia_tela(&t, LARGURA_JANELA, ALTURA_JANELA, TITULO_JANELA);

    while ((tecla_pressionada = codigo_tecla(&t)) != ALLEGRO_EVENT_DISPLAY_CLOSE) {
        limpa_tela(&t);

        switch (modoJogo) {
            case 0: 
                desenhaMenu(&t);
                break;

            case 1:
                jogarModoNormal(&t, tabuleiro_nonogram);
                break;

            case 2:
                jogarModoClassico(&t, tabuleiro_nonogram);
                break;

            case 3:
                mostrarTelaFinal(&t);
                break;

            default:
                desenhaJogo(&t, tabuleiro_nonogram);
                break;
        }
        
        verifica_clique(&t, tabuleiro_nonogram);

        mostra_tela();
        
        espera(30);
    }
    
    finaliza_tela(&t);
    
    return 0;
}

void jogarModoClassico(Tela *t, Celula tabuleiro[][COLUNAS])
{
    desenhaJogo(t, tabuleiro);
    sprintf (mensagem, "Celulas corretas: %d/%d", numeroDeCelulasCertasSelecionadas, numeroDeCelulasPintadas);
    escreve_texto(t, (Ponto){LARGURA_JANELA / 4 - 185, 15}, mensagem);
    escreve_texto(t, (Ponto){LARGURA_JANELA / 2 - 55, 15}, "Modo Clássico");
    desenha_botao(&botaoDesfazer, t, false);
}

void jogarModoNormal(Tela *t, Celula tabuleiro[][COLUNAS])
{
    desenhaJogo(t, tabuleiro);
    sprintf (mensagem, "Vidas restantes: %d/3", vida);
    escreve_texto(t, (Ponto){LARGURA_JANELA / 4 - 185, 15}, mensagem);
    escreve_texto(t, (Ponto){LARGURA_JANELA / 2 - 55, 15}, "Modo Normal");
}

void mostrarTelaFinal(Tela *t)
{
    escreve_texto(t, (Ponto){ganhou ? LARGURA_JANELA / 2 - 100 : LARGURA_JANELA / 2 - 125, ALTURA_JANELA / 2 - 50}, ganhou ? "Parabéns! Você venceu!" : "Você perdeu! Tente novamente.");
    desenha_botao(&botaoJogarNovamente, t, false);
    delayCliqueTabuleiro = 1;
}

void desenhaJogo(Tela *t, Celula tabuleiro[][COLUNAS])
{
    desenha_tabuleiro(t, tabuleiro);

    desenha_botao(&botaoReset, t, false);
    desenha_botao(&botaoLimpar, t, false);
    desenha_botao(&botaoResolver, t, false);
}

void desenhaMenu(Tela *t)
{
    escreve_texto(t, (Ponto){LARGURA_JANELA / 2 - 125, ALTURA_JANELA / 2 - 50}, "SUPER NONOGRAM 2000.");
    desenha_botao(&botaoClassico, t, false);
    desenha_botao(&botaoNormal, t, false);
}

void inicia_linha(Celula tabuleiro[][COLUNAS]) {

    for(int i = 0; i < LINHAS; i++) {   
        int indexLinha = 0; 
        for(int j = 0; j < COLUNAS; j++) {
            if(tabuleiro[j][i].colorido){//usei o chatgpt para entender pq tem que ser j e i nessa ordem
                linhasPintadas[i][indexLinha]++;
            } 
            else
            {   
                indexLinha++;
            }
        }

    }
}

void inicia_colunas(Celula tabuleiro[][COLUNAS]) {
    for(int j = 0; j < COLUNAS; j++) {   
        int indexColuna = 0;
        for(int i = 0; i < LINHAS; i++) {
            if(tabuleiro[j][i].colorido){//usei o chatgpt para entender pq tem que ser j e i nessa ordem
                colunasPintadas[j][indexColuna]++;
            } 
            else
            {
                indexColuna++;
            }
        }
    }
}

void inicia_tabuleiro(Celula tabuleiro[][COLUNAS]) {
    for (int i = 0; i < LINHAS; i++) {
        for (int j = 0; j < COLUNAS; j++) {
            // coordenadas de cada celula (retangulo)
            int x = MARGIN_HORIZONTAL + (LARGURA_CELULA * i);
            int y = MARGIN_VERTICAL + (ALTURA_CELULA * j);

            Celula c = { { x, y }, { ALTURA_CELULA, LARGURA_CELULA }, false, VAZIO };

            c.colorido = rand() % 2 == 0;
            if(c.colorido) numeroDeCelulasPintadas++;

            tabuleiro[i][j] = c;
        }
    }
}

void inicia_jogo(Celula tabuleiro[][COLUNAS])
{
    vida = 3;
    numeroDeCelulasCertasSelecionadas = 0;

    inicia_tabuleiro(tabuleiro);
    inicia_linha(tabuleiro);
    inicia_colunas(tabuleiro);
}

void inicia_botao(Botao *botao, int idBotao){
    int margem_botao_x = MARGEM_BOTAO + 5 * (idBotao - 1)  + 200 * (idBotao - 1);

    int x = POSICAO_BOTAO_X + margem_botao_x , y = POSICAO_BOTAO_Y;
    char const *frase;

    switch (idBotao) {
        case 1: frase = "Reiniciar Nonogram";
            break;
        case 2: frase = "Limpar Tabuleiro";
            break;
        case 3: frase = "Resolver Nonogram";
            break;
        case 4: frase = "Desfazer Jogada";
            x = LARGURA_JANELA/2 - 100;
            y = 3*ALTURA_JANELA/4 + 15;
            break;
        default: frase = "Botao";
    }

    Botao b = { { { x, y }, { 200, 50 }, false, VAZIO }, NORMAL, frase, idBotao};
    *botao = b;
}

void inicia_botao_menu(Botao *botao, int idBotao){
    int margem_botao_x = MARGEM_BOTAO + 5 * (idBotao - 1)  + 200 * (idBotao - 1);
    int x = (LARGURA_JANELA / 2 - MARGEM_BOTAO * ((2/2.0) + 1)) + margem_botao_x - 10, y = ALTURA_JANELA / 2;
    char const *frase;

    switch (idBotao) {
        case 1: frase = "Modo Normal";
            break;
        case 2: frase = "Modo Classico";
            break;
        default: frase = "Botao";
        case 3: frase = "Jogar Novamente";
            x = LARGURA_JANELA / 2 - 100;
            y = ALTURA_JANELA / 2;
            break;
    }

    Botao b = { { { x, y }, { 200, 50 }, false, VAZIO }, NORMAL, frase, idBotao};
    *botao = b;
}

void desenha_tabuleiro(Tela *t, Celula tabuleiro[][COLUNAS]) {
    Cor preto = {0, 0, 0};
    Cor vermelho = {1, 0, 0};
    define_cor(t, preto);

    for (int i = 0; i < LINHAS; i++)  {
        for (int j = 0; j < COLUNAS; j++) {
            desenha_retangulo(tabuleiro[i][j], t, tabuleiro[i][j].estado == COLORIDO);
            if (tabuleiro[i][j].estado == ALERTA) {
                define_cor(t, vermelho);
                desenha_x_dentro_ret(tabuleiro[i][j], t);
                define_cor(t, preto); 
            }

            if (tabuleiro[i][j].estado == ERRO) {
                define_cor(t, preto);
                desenha_x_dentro_ret(tabuleiro[i][j], t);
                define_cor(t, preto); 
            }
        }
    }

    for (int i = 0; i < LINHAS; i++) {
        int x = MARGIN_HORIZONTAL - 10;
        int y = MARGIN_VERTICAL + (i * ALTURA_CELULA) + 15;
        Ponto p = { x, y };

        escreve_numeros(t, linhasPintadas[i], 8, p, true);
    }

    for (int j = 0; j < COLUNAS; j++) {
        int x = MARGIN_HORIZONTAL + (j * LARGURA_CELULA) + 15;
        int y = MARGIN_VERTICAL - 10;
        Ponto p = { x, y };

        escreve_numeros(t, colunasPintadas[j], 8, p, false);
    }
}

void escreve_numeros(Tela *t, int *numeros, int qtd, Ponto inicio, bool horizontal) {
    int x = inicio.x, y = inicio.y, offset = 20;
    for (int i = qtd - 1; i >= 0; i--) {
        char buffer[10];
        if(numeros[i] == 0) continue; 
        sprintf(buffer, "%d", numeros[i]);

        if (horizontal) {
            x -= offset;
        } else y -= offset;
        
        Ponto p = { x, y };
        escreve_texto(t, p, buffer);
    }
}

void verifica_clique(Tela *t, Celula tabuleiro[][COLUNAS]) {
    Ponto mouse = posicao_mouse(t);

    if (botao_clicado(t)) { 
        switch (modoJogo) {
            case 0:
                if (delayCliqueTabuleiro > 0) {
                    delayCliqueTabuleiro = 0;
                    t->_botao = false;
                    return;
                }
                delayCliqueTabuleiro = 1;
                verifica_cliqueBotaoMenu(&mouse, &botaoClassico, tabuleiro);
                verifica_cliqueBotaoMenu(&mouse, &botaoNormal, tabuleiro);
                return;
            case 3:
                verifica_cliqueBotaoMenu(&mouse, &botaoJogarNovamente, tabuleiro);
                return;
            default:
                break;
        }

        if (delayCliqueTabuleiro > 0) { //usei o chatgpt nesse trecho para criar um pequeno delay para evitar que o jogador clique sem querer em uma celula do tabuleiro ao escolher o modo de jogo
            delayCliqueTabuleiro--;
            t->_botao = false;
            return;
        }

        int x_clique_tabuleiro = (mouse.x - MARGIN_HORIZONTAL);
        int y_clique_tabuleiro = (mouse.y - MARGIN_VERTICAL);
        
        // verifica se o clique foi dentro do tabuleiro
        if (x_clique_tabuleiro >= 0 && y_clique_tabuleiro >= 0 && !ganhou) {

            desfez = false;
            // descobre qual celula do tabuleiro foi clicada
            int i = (int) (x_clique_tabuleiro / ALTURA_CELULA);
            int j = (int) (y_clique_tabuleiro / LARGURA_CELULA);

            if (i >= 0 && i < LINHAS && j >=0 && j < COLUNAS) {

                atualizaTabuleiroAntigo(tabuleiro);

                cicla_estado_celula(&tabuleiro[i][j], false);

                if(tabuleiro[i][j].estado == COLORIDO) {
                    if(tabuleiro[i][j].colorido) {
                        numeroDeCelulasCertasSelecionadas++;
                    }
                    else if(modoJogo == 1)
                    {
                        cicla_estado_celula(&tabuleiro[i][j], true); 
                        vida--; 
                    }
                }

                if(tabuleiro[i][j].estado == COLORIDO) {
                    if(!tabuleiro[i][j].colorido) {
                        numeroDeCelulasErradasSelecionadas++;
                    }
                }
                
                if((tabuleiro[i][j].estado == VAZIO && modoJogo == 1) || (tabuleiro[i][j].estado == ALERTA && modoJogo == 2)) {
                    if(tabuleiro[i][j].colorido) {
                        numeroDeCelulasCertasSelecionadas--;
                    }
                    else
                    {
                        numeroDeCelulasErradasSelecionadas--;
                    }
                }

                if(vida == 0) {
                    modoJogo = 3;
                }

                if(numeroDeCelulasCertasSelecionadas == numeroDeCelulasPintadas && numeroDeCelulasErradasSelecionadas == 0) {
                    modoJogo = 3;
                    ganhou = true;
                }

            }
        }

        verifica_cliqueBotao(&mouse, &botaoReset, tabuleiro);
        verifica_cliqueBotao(&mouse, &botaoLimpar, tabuleiro);
        verifica_cliqueBotao(&mouse, &botaoResolver, tabuleiro);
        if(modoJogo == 2) {
            verifica_cliqueBotao(&mouse, &botaoDesfazer, tabuleiro);
        }

        t->_botao = false;
    }
}

void verifica_cliqueBotao(Ponto *mouse, Botao *b, Celula tabuleiro[][COLUNAS]) {
    if(mouse->x >= b->ret.pos.x && mouse->x <= b->ret.pos.x + b->ret.tam.larg && mouse->y >= b->ret.pos.y && mouse->y <= b->ret.pos.y + b->ret.tam.alt) {
        switch (b->id)
        {
        case 1:
            for(int i = 0; i < LINHAS; i++) {
                for(int j = 0; j < COLUNAS; j++) {
                    linhasPintadas[i][j] = 0;
                    colunasPintadas[i][j] = 0;
                }
            }
            ganhou = false;
            numeroDeCelulasPintadas = 0;
            numeroDeCelulasCertasSelecionadas = 0;
            inicia_jogo(tabuleiro);
            break;

        case 2:
            for(int i = 0; i < LINHAS; i++) {
                for(int j = 0; j < COLUNAS; j++) {
                    tabuleiro[i][j].estado = tabuleiro[i][j].estado != ERRO ? VAZIO : tabuleiro[i][j].estado;
                    numeroDeCelulasCertasSelecionadas = 0;
                }
            }
            ganhou = false;
            break;

        case 3:
            for(int i = 0; i < LINHAS; i++) {
                for(int j = 0; j < COLUNAS; j++) {
                    tabuleiro[i][j].estado = VAZIO;
                    if(tabuleiro[i][j].colorido) {
                        tabuleiro[i][j].estado = COLORIDO;
                    }
                }
            }
            numeroDeCelulasCertasSelecionadas = numeroDeCelulasPintadas;
            ganhou = true;
            break;
        
        case 4:
            if(!desfez) {
                for(int i = 0; i < LINHAS; i++) {
                    for(int j = 0; j < COLUNAS; j++) {
                        if(tabuleiro_nonogram_antigo[i][j].estado != tabuleiro[i][j].estado) {
                            tabuleiro[i][j] = tabuleiro_nonogram_antigo[i][j];
                        }
                    }
                }
                desfez = true;
                ganhou = false;
            }
            numeroDeCelulasCertasSelecionadas = numeroDeCelulasCertasSelecionadasAntigos;
            numeroDeCelulasErradasSelecionadas = numeroDeCelulasErradasSelecionadasAntigos;
            break;
        
        default:
            break;
        }
    }
}

void atualizaTabuleiroAntigo(Celula tabuleiro[][COLUNAS])
{
    numeroDeCelulasCertasSelecionadasAntigos = numeroDeCelulasCertasSelecionadas;
    numeroDeCelulasErradasSelecionadasAntigos = numeroDeCelulasErradasSelecionadas;

    for(int i = 0; i < LINHAS; i++) {
        for(int j = 0; j < COLUNAS; j++) {
            tabuleiro_nonogram_antigo[i][j] = tabuleiro[i][j];
        }
    }
}

void verifica_cliqueBotaoMenu(Ponto *mouse, Botao *b, Celula tabuleiro[][COLUNAS]) {
    if(mouse->x >= b->ret.pos.x && mouse->x <= b->ret.pos.x + b->ret.tam.larg && mouse->y >= b->ret.pos.y && mouse->y <= b->ret.pos.y + b->ret.tam.alt) {
        switch (b->id)
        {
        case 1:
            modoJogo = 1;
            break;

        case 2:
            modoJogo = 2;
            break;

        case 3:
            for(int i = 0; i < LINHAS; i++) {
                for(int j = 0; j < COLUNAS; j++) {
                    linhasPintadas[i][j] = 0;
                    colunasPintadas[i][j] = 0;
                }
            }
            ganhou = false;
            numeroDeCelulasPintadas = 0;
            numeroDeCelulasCertasSelecionadas = 0;
            inicia_jogo(tabuleiro);
            modoJogo = 0;
            break;
        default:
            break;
        }
    }
}

void cicla_estado_celula(Celula *c, bool errou) {
    if(errou) {
        c->estado = ERRO;
        return;
    }

    switch (c->estado) {
        case VAZIO: c->estado = COLORIDO;
            break;
        case COLORIDO: 
            if(modoJogo == 1)
                c->estado = VAZIO;
            else 
                c->estado = ALERTA;
            break;
        case ALERTA: c->estado = VAZIO;
            break;
    }
}
