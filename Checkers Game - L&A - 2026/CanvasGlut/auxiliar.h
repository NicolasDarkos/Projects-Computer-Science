#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <GL/freeglut_ext.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl_canvas2d.h"

extern bool _jogoComecou;
extern bool _jogoFinalizado;

extern float _offsetXMenu;
extern float _offsetYMenu;

extern int _numeroDeCasasSelecionado;

extern int _numeroDeCasas;
extern int _tamanhoCasa;

extern int _linhasComPecas;
extern int _numeroDePecasPorJogador;

struct Selecao
{
    int s_posX = 0;
    int s_posY = 0;

    int s_indiceX = 0;
    int s_indiceY = 1;

    bool s_selected = false;

    int s_pecaSelecionada = -1;

    bool s_valida = false;
};

struct Tabuleiro
{
    int t_posX = 0;
    int t_posY = 0;

    bool t_temPeca = false;
    bool t_ultimaLinha = false;

    int t_pecaRelacionada = -1;
    int t_jogadorDono = -1;
};

struct Pecas
{
    int p_id = 0;

    int p_posX = 0;
    int p_posY = 0;

    int p_indiceX = 0;
    int p_indiceY = 0;

    bool p_ehDama = false;
    bool p_viva = false;
};

struct Jogador
{
    Pecas* j_pecas = NULL;

    int j_pecasVivas = 0;

    int j_id = 0;
};

extern int _screenWidth;
extern int _screenHeight;

extern int _mouseX;
extern int _mouseY;

extern int _tamanhoTabuleiro;

extern int _offsetX;
extern int _offsetY;

extern int _selecionandoDiagonal;

extern int _moveX;
extern int _moveY;

extern bool _jogoInicializado;
extern bool _temCasaSelecionada;

extern bool _aindaPodePegar;

extern int _indiceDiagonalSelecionada;

extern int _quantidadeDiagonaisDama;

extern Selecao casaSelecionada;

extern Selecao casaDiagonais[2];

extern Selecao diagonalSelecionada;

extern Selecao *diagonaisDama;

extern Jogador jogadores[2];

extern Jogador jogadorAtual;

extern Tabuleiro **tabuleiro;

void RecalcularTamanhos();
void AlocarTabuleiro();
void LiberarTabuleiro();
void AlocarPecas();
void LiberarPecas();
void AlocarDiagonaisDama();
void LiberarDiagonaisDama();

void PrepararJogo(int tamanhoEscolhido);

bool DentroTabuleiro(int x, int y);

bool CasaVazia(int x, int y);
bool EhInimigo(int x, int y);

void LimparCasa(int x, int y);
void PosicionarPeca(int jogador, int indice, int x, int y);
void PintarSelecao(int x, int y, float r1, float g1, float b1, float r2, float g2, float b2);

void DestacarCasa(int x, int y, bool selecionada);
void MoverPecaPara(int jogador, int indice, int origemX, int origemY, int destinoX, int destinoY);

void InicializarTabuleiro();
void InicializarPecas();
void RelacionarPecasTabuleiro(int jogador, int indice);
void InicializarJogador();

void TrocarJogodorAtual();

void PintarTabuleiro();
void PintarPecas(float raioPeca);
void PintarCasaSelecionado();

void SelecionarPrimeiraPecaViva();
void SelecionarCasa();
void MoverSelecao(int direcao, Jogador jogador);

void MoverDiagonais(Jogador jogador);

void MostrarJogadas();

void SelecionarDiagonal();
void MoverDiagonalSelecionada(int direcao);
void PintarDiagonalSelecionado();
void DescobrirProximaDiagonal();

void DrawMouseScreenCoords();

bool PodeCapturar();
void ChecarSeGanhou();
void MostrarCenaFinal();

void TornarDama();
void MoverDama();

void PegarPeca();

void MoverPeca();

void MostrarCaptura();

void MostrarJogadasAtuais();

void RenderizarCena();

void RenderizarDebug();

void AtualizarJogo();

void DesenharManual();

void MostrarTelaInicial();

void NaoSairMouse();

void keyboard(int key);

void keyboardUp(int key);

void mouse(int button, int state, int wheel, int direction, int x, int y);

void DrawMouseScreenCoords();