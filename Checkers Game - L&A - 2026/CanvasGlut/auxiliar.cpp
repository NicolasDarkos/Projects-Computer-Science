#include "auxiliar.h"

bool _jogoComecou = false;
bool _jogoFinalizado = false;

float _offsetXMenu = 150.0f;
float _offsetYMenu = 20.0f;

int _numeroDeCasasSelecionado = 8;

int _numeroDeCasas = 8;
int _tamanhoCasa = 60;

int _linhasComPecas = (_numeroDeCasas / 2) - 1;
int _numeroDePecasPorJogador = (_numeroDeCasas / 2) * _linhasComPecas;

int _screenWidth = 1000;
int _screenHeight = 800;

int _mouseX, _mouseY;

int _tamanhoTabuleiro = _numeroDeCasas * _tamanhoCasa;

int _offsetX, _offsetY;
int _selecionandoDiagonal;

int _moveX = 0;
int _moveY = 0;

bool _jogoInicializado = false;
bool _temCasaSelecionada = false;

bool _aindaPodePegar = false;

int _indiceDiagonalSelecionada = 0;
int _quantidadeDiagonaisDama = 0;

Selecao casaSelecionada;
Selecao casaDiagonais[2];
Selecao diagonalSelecionada;

Selecao *diagonaisDama = NULL;

Jogador jogadores[2];
Jogador jogadorAtual;

Tabuleiro** tabuleiro = NULL;

void RecalcularTamanhos()
{
    _linhasComPecas = (_numeroDeCasas / 2) - 1;

    _numeroDePecasPorJogador = (_numeroDeCasas / 2) * _linhasComPecas;

    _tamanhoTabuleiro = _numeroDeCasas * _tamanhoCasa;
}

void AlocarTabuleiro()
{
    tabuleiro = (Tabuleiro**)malloc(_numeroDeCasas * sizeof(Tabuleiro*));

    for (int i = 0; i < _numeroDeCasas; i++)
    {
        tabuleiro[i] = (Tabuleiro*)malloc(_numeroDeCasas * sizeof(Tabuleiro));
    }
}

void LiberarTabuleiro()
{
    if (tabuleiro == NULL)
    {
        return;
    }

    for (int i = 0; i < _numeroDeCasas; i++)
    {
        free(tabuleiro[i]);
    }

    free(tabuleiro);

    tabuleiro = NULL;
}

void AlocarPecas()
{
    for (int i = 0; i < 2; i++)
    {
        jogadores[i].j_pecas = (Pecas*)malloc(_numeroDePecasPorJogador * sizeof(Pecas));

        jogadores[i].j_pecasVivas = _numeroDePecasPorJogador;
        jogadores[i].j_id = i;
    }
}

void LiberarPecas()
{
    for (int i = 0; i < 2; i++)
    {
        if (jogadores[i].j_pecas != NULL)
        {
            free(jogadores[i].j_pecas);
            jogadores[i].j_pecas = NULL;
        }
    }
}

void AlocarDiagonaisDama()
{
    int quantidadeMaxima = (_numeroDeCasas - 1) * 4;

    diagonaisDama = (Selecao*)malloc(quantidadeMaxima * sizeof(Selecao));
}

void LiberarDiagonaisDama()
{
    if (diagonaisDama != NULL)
    {
        free(diagonaisDama);
        diagonaisDama = NULL;
    }
}

void PrepararJogo(int numeroDeCasas)
{
    _numeroDeCasas = numeroDeCasas;

    RecalcularTamanhos();

    AlocarTabuleiro();

    AlocarPecas();

    AlocarDiagonaisDama();

    _jogoInicializado = false;
    _temCasaSelecionada = false;

    casaSelecionada.s_selected = false;
    casaSelecionada.s_pecaSelecionada = -1;

    casaSelecionada.s_indiceX = 0;
    casaSelecionada.s_indiceY = 1;
}


bool DentroTabuleiro(int x, int y)
{
    return x >= 0 && x < _numeroDeCasas && y >= 0 && y < _numeroDeCasas;
}

bool CasaVazia(int x, int y)
{
    return !tabuleiro[x][y].t_temPeca;
}

bool EhInimigo(int x, int y)
{
    return tabuleiro[x][y].t_temPeca && tabuleiro[x][y].t_jogadorDono != jogadorAtual.j_id;
}

void LimparCasa(int x, int y)
{
    tabuleiro[x][y].t_temPeca = false;
    tabuleiro[x][y].t_pecaRelacionada = -1;
    tabuleiro[x][y].t_jogadorDono = -1;
}

void PosicionarPeca(int jogador, int indice, int x, int y)
{
    Pecas& peca = jogadores[jogador].j_pecas[indice];

    peca.p_indiceX = x;
    peca.p_indiceY = y;

    peca.p_posX = tabuleiro[x][y].t_posX + _tamanhoCasa / 2;
    peca.p_posY = tabuleiro[x][y].t_posY + _tamanhoCasa / 2;

    tabuleiro[x][y].t_temPeca = true;
    tabuleiro[x][y].t_pecaRelacionada = indice;
    tabuleiro[x][y].t_jogadorDono = jogador;
}

void PintarSelecao(int x, int y, float r1, float g1, float b1, float r2, float g2, float b2)
{
    CV::color(r1, g1, b1);

    CV::rectFill(x, y, x + _tamanhoCasa, y + _tamanhoCasa);

    CV::color(r2, g2, b2);

    for (float m = 5; m > 0; m -= 0.1f)
    {
        CV::rect(x - m, y - m, x + _tamanhoCasa + m, y + _tamanhoCasa + m);
    }
}

void DestacarCasa(int x, int y, bool selecionada)
{
    int posX = tabuleiro[x][y].t_posX;
    int posY = tabuleiro[x][y].t_posY;

    if (selecionada)
    {
        PintarSelecao(posX, posY, 0.5, 0.85, 0.5, 0, 0.75, 0);
    }
    else
    {
        PintarSelecao(posX, posY, 1.0, 0.534, 0.479, 1, 0, 0);
    }
}

void MoverPecaPara(int jogador, int indice, int origemX, int origemY, int destinoX, int destinoY)
{
    LimparCasa(origemX, origemY);

    PosicionarPeca(jogador, indice, destinoX, destinoY);
}

void InicializarTabuleiro()
{
    if (_jogoInicializado) return;

    _offsetX = (_screenWidth - _tamanhoTabuleiro) / 2;
    _offsetY = (_screenHeight - _tamanhoTabuleiro) / 2;

    for (int i = 0; i < _numeroDeCasas; i++)
    {
        for (int j = 0; j < _numeroDeCasas; j++)
        {
            Tabuleiro& casa = tabuleiro[i][j];

            casa.t_posX = _offsetX + j * _tamanhoCasa;
            casa.t_posY = _offsetY + i * _tamanhoCasa;

            casa.t_ultimaLinha = i == 0 || i == _numeroDeCasas - 1;

            casa.t_temPeca = (i + j) % 2 != 0 && (i < _linhasComPecas || i >= _numeroDeCasas - _linhasComPecas);
        }
    }

    InicializarPecas();
    InicializarJogador();

    _jogoInicializado = true;
}

void InicializarPecas()
{
    for (int jogador = 0; jogador < 2; jogador++)
    {
        int indice = 0;

        for (int i = 0; i < _numeroDeCasas; i++)
        {
            for (int j = 0; j < _numeroDeCasas; j++)
            {
                bool linhaJogador1 = jogador == 0 && i < _linhasComPecas;

                bool linhaJogador2 = jogador == 1 && i >= _numeroDeCasas - _linhasComPecas;

                bool casaValida = (i + j) % 2 != 0;

                if (!(casaValida && (linhaJogador1 || linhaJogador2)))
                {
                    continue;
                }

                Pecas& peca = jogadores[jogador].j_pecas[indice];

                peca.p_id = indice;
                peca.p_viva = true;
                peca.p_ehDama = false;

                PosicionarPeca(jogador, indice, i, j);

                indice++;
            }
        }
    }
}

void RelacionarPecasTabuleiro(int jogador, int indice)
{
    int indiceX = jogadores[jogador].j_pecas[indice].p_indiceX;

    int indiceY = jogadores[jogador].j_pecas[indice].p_indiceY;

    tabuleiro[indiceX][indiceY].t_pecaRelacionada = jogadores[jogador].j_pecas[indice].p_id;

    tabuleiro[indiceX][indiceY].t_jogadorDono = jogador;
}

void InicializarJogador()
{
    for (int i = 0; i < 2; i++)
    {
        jogadores[i].j_id = i;
    }

    jogadorAtual = jogadores[0];

    SelecionarPrimeiraPecaViva();
}

void SelecionarPrimeiraPecaViva()
{
    for (int i = 0; i < _numeroDePecasPorJogador; i++)
    {
        if (jogadores[jogadorAtual.j_id].j_pecas[i].p_viva)
        {
            casaSelecionada.s_pecaSelecionada = i;

            casaSelecionada.s_indiceX =
                jogadores[jogadorAtual.j_id].j_pecas[i].p_indiceX;

            casaSelecionada.s_indiceY =
                jogadores[jogadorAtual.j_id].j_pecas[i].p_indiceY;

            casaSelecionada.s_posX =
                tabuleiro[casaSelecionada.s_indiceX][casaSelecionada.s_indiceY].t_posX;

            casaSelecionada.s_posY =
                tabuleiro[casaSelecionada.s_indiceX][casaSelecionada.s_indiceY].t_posY;

            _indiceDiagonalSelecionada = 0;

            MoverDiagonais(jogadorAtual);

            return;
        }
    }

    casaSelecionada.s_pecaSelecionada = -1;
}

void TrocarJogodorAtual()
{
    if (jogadorAtual.j_id == 0)
    {
        jogadorAtual = jogadores[1];
    }
    else
    {
        jogadorAtual = jogadores[0];
    }

    SelecionarPrimeiraPecaViva();
}

void ChecarSeGanhou()
{
    if (jogadorAtual.j_pecasVivas == 0)
    {
        TrocarJogodorAtual();

		_jogoFinalizado = true;
    }
}

void PintarTabuleiro()
{
    for (int i = 0; i < _numeroDeCasas; i++)
    {
        for (int j = 0; j < _numeroDeCasas; j++)
        {
            if ((i + j) % 2 == 0)
            {
                CV::color(1, 1, 1);
            }
            else
            {
                CV::color(0.25, 0.25, 0.25);
            }

            CV::rectFill(tabuleiro[i][j].t_posX, tabuleiro[i][j].t_posY, tabuleiro[i][j].t_posX + _tamanhoCasa, tabuleiro[i][j].t_posY + _tamanhoCasa);
        }
    }

    CV::color(0, 0, 0);

    for (int i = 0; i < 4; i++)
    {
        CV::rect(_offsetX - i, _offsetY - i, _offsetX + _tamanhoTabuleiro + i, _offsetY + _tamanhoTabuleiro + i);
    }
}

void PintarPecas(float raioPeca)
{
    for (int k = 0; k < 2; k++)
    {
        if (k == 0)
        {
            CV::color(0.5, 0.5, 1);
        }
        else
        {
            CV::color(1, 1, 0.75);
        }

        for (int i = 0; i < _numeroDePecasPorJogador; i++)
        {
            if (jogadores[k].j_pecas[i].p_viva)
            {
                int posX = jogadores[k].j_pecas[i].p_posX;
                int posY = jogadores[k].j_pecas[i].p_posY;

                CV::circleFill(posX, posY, raioPeca, 30);

                if (jogadores[k].j_pecas[i].p_ehDama)
                {
                    CV::color(0, 0, 0);

                    CV::text(posX - 5, posY + 5, "D");

                    if (k == 0)
                    {
                        CV::color(0.5, 0.5, 1);
                    }
                    else
                    {
                        CV::color(1, 1, 0.75);
                    }
                }
            }
        }
    }
}

void PintarCasaSelecionado()
{
    if (!_temCasaSelecionada)
    {
        PintarSelecao(casaSelecionada.s_posX, casaSelecionada.s_posY, 1.0, 0.534, 0.479, 1, 0, 0);

        return;
    }

    PintarSelecao(casaSelecionada.s_posX, casaSelecionada.s_posY, 0.75, 0.5, 0.5, 0.5, 0, 0);
}
void SelecionarCasa()
{
    if (!casaSelecionada.s_selected) return;

    Tabuleiro& casa = tabuleiro[casaSelecionada.s_indiceX][casaSelecionada.s_indiceY];

    casaSelecionada.s_posX = casa.t_posX;
    casaSelecionada.s_posY = casa.t_posY;

    PintarCasaSelecionado();
}

void MoverDiagonais(Jogador jogadorAtual)
{
    int frente;

    if (jogadorAtual.j_id == 0)
    {
        frente = 1;
    }
    else
    {
        frente = -1;
    }

    casaDiagonais[0].s_indiceX = casaSelecionada.s_indiceX + frente;
    casaDiagonais[0].s_indiceY = casaSelecionada.s_indiceY - 1;

    casaDiagonais[1].s_indiceX = casaSelecionada.s_indiceX + frente;
    casaDiagonais[1].s_indiceY = casaSelecionada.s_indiceY + 1;

    for (int i = 0; i < 2; i++)
    {
        bool dentroDoTabuleiro = casaDiagonais[i].s_indiceX >= 0 && casaDiagonais[i].s_indiceX < _numeroDeCasas && casaDiagonais[i].s_indiceY >= 0 && casaDiagonais[i].s_indiceY < _numeroDeCasas;

        if (dentroDoTabuleiro && !tabuleiro[casaDiagonais[i].s_indiceX][casaDiagonais[i].s_indiceY].t_temPeca)
        {
            casaDiagonais[i].s_valida = true;
        }
        else
        {
            casaDiagonais[i].s_valida = false;
        }

        if (dentroDoTabuleiro)
        {
            casaDiagonais[i].s_posX = tabuleiro[casaDiagonais[i].s_indiceX][casaDiagonais[i].s_indiceY].t_posX;

            casaDiagonais[i].s_posY = tabuleiro[casaDiagonais[i].s_indiceX][casaDiagonais[i].s_indiceY].t_posY;
        }
    }
}
void MoverSelecao(int direcao, Jogador jogador)
{
    int melhorIndice = -1;

    int atualX = jogador.j_pecas[casaSelecionada.s_pecaSelecionada].p_indiceX;
    int atualY = jogador.j_pecas[casaSelecionada.s_pecaSelecionada].p_indiceY;

    for (int i = 0; i < _numeroDePecasPorJogador; i++)
    {
        if (!jogador.j_pecas[i].p_viva)
        {
            continue;
        }

        int x = jogador.j_pecas[i].p_indiceX;
        int y = jogador.j_pecas[i].p_indiceY;

        bool encontrou = false;

        if (direcao > 0)
        {
            if (x > atualX || (x == atualX && y > atualY))
            {
                encontrou = true;
            }
        }
        else
        {
            if (x < atualX || (x == atualX && y < atualY))
            {
                encontrou = true;
            }
        }

        if (!encontrou)
        {
            continue;
        }

        if (melhorIndice == -1)
        {
            melhorIndice = i;
            continue;
        }

        int melhorX = jogador.j_pecas[melhorIndice].p_indiceX;
        int melhorY = jogador.j_pecas[melhorIndice].p_indiceY;

        if (direcao > 0)
        {
            if (x < melhorX || (x == melhorX && y < melhorY))
            {
                melhorIndice = i;
            }
        }
        else
        {
            if (x > melhorX || (x == melhorX && y > melhorY))
            {
                melhorIndice = i;
            }
        }
    }

    if (melhorIndice == -1)
    {
        for (int i = 0; i < _numeroDePecasPorJogador; i++)
        {
            if (!jogador.j_pecas[i].p_viva)
            {
                continue;
            }

            if (melhorIndice == -1)
            {
                melhorIndice = i;
                continue;
            }

            int x = jogador.j_pecas[i].p_indiceX;
            int y = jogador.j_pecas[i].p_indiceY;

            int melhorX = jogador.j_pecas[melhorIndice].p_indiceX;
            int melhorY = jogador.j_pecas[melhorIndice].p_indiceY;

            if (direcao > 0)
            {
                if (x < melhorX || (x == melhorX && y < melhorY))
                {
                    melhorIndice = i;
                }
            }
            else
            {
                if (x > melhorX || (x == melhorX && y > melhorY))
                {
                    melhorIndice = i;
                }
            }
        }
    }

    casaSelecionada.s_pecaSelecionada = melhorIndice;

    Pecas& peca = jogador.j_pecas[melhorIndice];

    casaSelecionada.s_indiceX = peca.p_indiceX;
    casaSelecionada.s_indiceY = peca.p_indiceY;

    _indiceDiagonalSelecionada = 0;

    MoverDiagonais(jogadorAtual);
}

void MostrarJogadas()
{
    if (!_temCasaSelecionada) return;

    for (int i = 0; i < 2; i++)
    {
        if (!casaDiagonais[i].s_valida) continue;

        DestacarCasa(casaDiagonais[i].s_indiceX, casaDiagonais[i].s_indiceY, false);
    }
}

void SelecionarDiagonal()
{
    if (!_temCasaSelecionada)
    {
        return;
    }

    if (jogadorAtual
        .j_pecas[casaSelecionada.s_pecaSelecionada]
        .p_ehDama)
    {
        if (_indiceDiagonalSelecionada < 0 || _indiceDiagonalSelecionada >= _quantidadeDiagonaisDama)
        {
            diagonalSelecionada.s_valida = false;
            return;
        }

        diagonalSelecionada = diagonaisDama[_indiceDiagonalSelecionada];

        diagonalSelecionada.s_valida = true;

        return;
    }

    if (!casaDiagonais[_indiceDiagonalSelecionada].s_valida)
    {
        diagonalSelecionada.s_valida = false;
        return;
    }

    diagonalSelecionada = casaDiagonais[_indiceDiagonalSelecionada];

    diagonalSelecionada.s_valida = true;

    PintarDiagonalSelecionado();
}

void PintarDiagonalSelecionado()
{
    PintarSelecao(diagonalSelecionada.s_posX, diagonalSelecionada.s_posY, 0.5, 0.85, 0.5, 0, 0.75, 0);
}

void MostrarJogadasDama()
{
    _quantidadeDiagonaisDama = 0;

    int direcoesX[4] = { -1, -1, 1, 1 };
    int direcoesY[4] = { -1, 1, -1, 1 };

    for (int d = 0; d < 4; d++)
    {
        int x = casaSelecionada.s_indiceX;
        int y = casaSelecionada.s_indiceY;

        bool encontrouInimigo = false;

        while (true)
        {
            x += direcoesX[d];
            y += direcoesY[d];

            if (x < 0 || x >= _numeroDeCasas || y < 0 || y >= _numeroDeCasas)
            {
                break;
            }

            if (tabuleiro[x][y].t_temPeca)
            {
                if (tabuleiro[x][y].t_jogadorDono == jogadorAtual.j_id)
                {
                    break;
                }

                if (encontrouInimigo)
                {
                    break;
                }

                encontrouInimigo = true;

                continue;
            }

            diagonaisDama[_quantidadeDiagonaisDama].s_indiceX = x;
            diagonaisDama[_quantidadeDiagonaisDama].s_indiceY = y;

            diagonaisDama[_quantidadeDiagonaisDama].s_posX = tabuleiro[x][y].t_posX;

            diagonaisDama[_quantidadeDiagonaisDama].s_posY = tabuleiro[x][y].t_posY;

            diagonaisDama[_quantidadeDiagonaisDama].s_valida = true;

            bool selecionada = (_indiceDiagonalSelecionada == _quantidadeDiagonaisDama);

            if (selecionada)
            {
                CV::color(0.5, 0.85, 0.5);

                CV::rectFill(tabuleiro[x][y].t_posX, tabuleiro[x][y].t_posY, tabuleiro[x][y].t_posX + _tamanhoCasa, tabuleiro[x][y].t_posY + _tamanhoCasa);

                CV::color(0, 0.75, 0);
            }
            else
            {
                CV::color(1.0, 0.534, 0.479);

                CV::rectFill(tabuleiro[x][y].t_posX, tabuleiro[x][y].t_posY, tabuleiro[x][y].t_posX + _tamanhoCasa, tabuleiro[x][y].t_posY + _tamanhoCasa);

                CV::color(1, 0, 0);
            }

            float margin = 5;

            for (float m = margin; m > 0; m -= 0.1f)
            {
                CV::rect(tabuleiro[x][y].t_posX - m, tabuleiro[x][y].t_posY - m, tabuleiro[x][y].t_posX + _tamanhoCasa + m, tabuleiro[x][y].t_posY + _tamanhoCasa + m);
            }

            _quantidadeDiagonaisDama++;

            if (encontrouInimigo)
            {
                break;
            }
        }
    }
}
void MoverDiagonalSelecionada(int direcao)
{
    bool ehDama = jogadorAtual.j_pecas[casaSelecionada.s_pecaSelecionada].p_ehDama;

    if (ehDama)
    {
        _indiceDiagonalSelecionada += direcao;

        if (_indiceDiagonalSelecionada >= _quantidadeDiagonaisDama)
        {
            _indiceDiagonalSelecionada = 0;
        }

        if (_indiceDiagonalSelecionada < 0)
        {
            _indiceDiagonalSelecionada = _quantidadeDiagonaisDama - 1;
        }

        return;
    }

    _indiceDiagonalSelecionada += direcao;

    if (_indiceDiagonalSelecionada > 1)
    {
        _indiceDiagonalSelecionada = 0;
    }

    if (_indiceDiagonalSelecionada < 0)
    {
        _indiceDiagonalSelecionada = 1;
    }
}

void MoverPeca()
{
    if (!diagonalSelecionada.s_valida) return;

    int indice = casaSelecionada.s_pecaSelecionada;

    int origemX = casaSelecionada.s_indiceX;
    int origemY = casaSelecionada.s_indiceY;

    int destinoX = diagonalSelecionada.s_indiceX;
    int destinoY = diagonalSelecionada.s_indiceY;

    MoverPecaPara(jogadorAtual.j_id, indice, origemX, origemY,destinoX, destinoY);

    jogadores[jogadorAtual.j_id].j_pecas[indice].p_indiceX = destinoX;
    jogadores[jogadorAtual.j_id].j_pecas[indice].p_indiceY = destinoY;

    casaSelecionada.s_indiceX = destinoX;
    casaSelecionada.s_indiceY = destinoY;

    _temCasaSelecionada = false;

    TornarDama();

    TrocarJogodorAtual();
}

void TornarDama()
{
    Pecas& peca = jogadores[jogadorAtual.j_id].j_pecas[casaSelecionada.s_pecaSelecionada];

    if (!peca.p_viva || peca.p_ehDama) return;

    if (jogadorAtual.j_id == 0 && peca.p_indiceX == _numeroDeCasas - 1)
    {
        peca.p_ehDama = true;
    }

    if (jogadorAtual.j_id == 1 && peca.p_indiceX == 0)
    {
        peca.p_ehDama = true;
    }
}

void MoverDama()
{
    if (!diagonalSelecionada.s_valida)
    {
        return;
    }

    int indicePeca = casaSelecionada.s_pecaSelecionada;

    int origemX = casaSelecionada.s_indiceX;

    int origemY = casaSelecionada.s_indiceY;

    int destinoX = diagonalSelecionada.s_indiceX;

    int destinoY = diagonalSelecionada.s_indiceY;

    int dirX = (destinoX > origemX) ? 1 : -1;

    int dirY = (destinoY > origemY) ? 1 : -1;

    int jogadorOponente = jogadorAtual.j_id == 0 ? 1 : 0;

    int x = origemX + dirX;
    int y = origemY + dirY;

    bool encontrouInimigo = false;

    int inimigoX = -1;
    int inimigoY = -1;

    while (x != destinoX && y != destinoY)
    {
        if (tabuleiro[x][y].t_temPeca)
        {
            if (tabuleiro[x][y].t_jogadorDono == jogadorAtual.j_id)
            {
                return;
            }

            if (encontrouInimigo)
            {
                return;
            }

            encontrouInimigo = true;

            inimigoX = x;
            inimigoY = y;
        }

        x += dirX;
        y += dirY;
    }

    if (encontrouInimigo)
    {
        int casaObrigatoriaX = inimigoX + dirX;
        int casaObrigatoriaY = inimigoY + dirY;

        if (destinoX != casaObrigatoriaX || destinoY != casaObrigatoriaY)
        {
            return;
        }

        int indiceInimigo = tabuleiro[inimigoX][inimigoY].t_pecaRelacionada;

        jogadores[jogadorOponente].j_pecas[indiceInimigo].p_viva = false;

        jogadores[jogadorOponente].j_pecasVivas--;

        tabuleiro[inimigoX][inimigoY].t_temPeca = false;

        tabuleiro[inimigoX][inimigoY].t_pecaRelacionada = -1;

        tabuleiro[inimigoX][inimigoY].t_jogadorDono = -1;
    }

	MoverPecaPara(jogadorAtual.j_id, indicePeca, origemX, origemY, destinoX, destinoY);

    casaSelecionada.s_indiceX = destinoX;
    casaSelecionada.s_indiceY = destinoY;

    casaSelecionada.s_posX = tabuleiro[destinoX][destinoY].t_posX;

    casaSelecionada.s_posY = tabuleiro[destinoX][destinoY].t_posY;

    ChecarSeGanhou();

    if (encontrouInimigo)
    {
        _quantidadeDiagonaisDama = 0;

        MostrarJogadasDama();

        bool podeContinuar = false;

        for (int i = 0; i < _quantidadeDiagonaisDama; i++)
        {
            int dx = diagonaisDama[i].s_indiceX - destinoX;

            int dy = diagonaisDama[i].s_indiceY - destinoY;

            int stepX = (dx > 0) ? 1 : -1;
            int stepY = (dy > 0) ? 1 : -1;

            int tx = destinoX + stepX;
            int ty = destinoY + stepY;

            bool encontrouOutroInimigo = false;

            while (tx != diagonaisDama[i].s_indiceX && ty != diagonaisDama[i].s_indiceY)
            {
                if (tabuleiro[tx][ty].t_temPeca)
                {
                    if (tabuleiro[tx][ty].t_jogadorDono != jogadorAtual.j_id)
                    {
                        encontrouOutroInimigo = true;
                    }

                    break;
                }

                tx += stepX;
                ty += stepY;
            }

            if (encontrouOutroInimigo)
            {
                podeContinuar = true;
                break;
            }
        }

        if (podeContinuar)
        {
            _temCasaSelecionada = true;
            return;
        }
    }

    _temCasaSelecionada = false;

    TrocarJogodorAtual();
}

void DescobrirProximaDiagonal()
{
    if (jogadorAtual.j_id == 0)
    {
        _moveX = 1;

        if (_indiceDiagonalSelecionada == 0)
        {
            _moveY = -1;
        }
        else
        {
            _moveY = 1;
        }
    }
    else
    {
        _moveX = -1;

        if (_indiceDiagonalSelecionada == 0)
        {
            _moveY = -1;
        }
        else
        {
            _moveY = 1;
        }
    }
}

bool PodeCapturar()
{
    int frente;

    if (jogadorAtual.j_id == 0)
    {
        frente = 1;
    }
    else
    {
        frente = -1;
    }

    int direcoes[2] = { -1, 1 };

    for (int i = 0; i < 2; i++)
    {
        int inimigoX =
            casaSelecionada.s_indiceX + frente;

        int inimigoY =
            casaSelecionada.s_indiceY + direcoes[i];

        int destinoX =
            inimigoX + frente;

        int destinoY =
            inimigoY + direcoes[i];

        if (inimigoX < 0 || inimigoX >= _numeroDeCasas || inimigoY < 0 || inimigoY >= _numeroDeCasas)
        {
            continue;
        }

        if (destinoX < 0 || destinoX >= _numeroDeCasas || destinoY < 0 || destinoY >= _numeroDeCasas)
        {
            continue;
        }

        if (!tabuleiro[inimigoX][inimigoY].t_temPeca)
        {
            continue;
        }

        if (tabuleiro[inimigoX][inimigoY].t_jogadorDono
            == jogadorAtual.j_id)
        {
            continue;
        }

        if (tabuleiro[destinoX][destinoY].t_temPeca)
        {
            continue;
        }

        return true;
    }

    return false;
}

void PegarPeca()
{
    DescobrirProximaDiagonal();

    int jogadorOponenteId = jogadorAtual.j_id == 0 ? 1 : 0;

    int inimigoX = casaDiagonais[_indiceDiagonalSelecionada].s_indiceX;

    int inimigoY = casaDiagonais[_indiceDiagonalSelecionada].s_indiceY;

    int origemX = casaSelecionada.s_indiceX;

    int origemY = casaSelecionada.s_indiceY;

    int indicePeca = casaSelecionada.s_pecaSelecionada;

    if (inimigoX < 0 || inimigoX >= _numeroDeCasas || inimigoY < 0 || inimigoY >= _numeroDeCasas)
    {
        return;
    }

    if (!tabuleiro[inimigoX][inimigoY].t_temPeca)
    {
        return;
    }

    if (tabuleiro[inimigoX][inimigoY].t_jogadorDono == jogadorAtual.j_id)
    {
        return;
    }

    int destinoX = inimigoX + _moveX;
    int destinoY = inimigoY + _moveY;

    if (destinoX < 0 || destinoX >= _numeroDeCasas || destinoY < 0 || destinoY >= _numeroDeCasas)
    {
        return;
    }

    if (tabuleiro[destinoX][destinoY].t_temPeca)
    {
        return;
    }

    int indicePecaInimiga = tabuleiro[inimigoX][inimigoY].t_pecaRelacionada;

    int indiceMinhaPeca = casaSelecionada.s_pecaSelecionada;

    jogadores[jogadorOponenteId].j_pecas[indicePecaInimiga].p_viva = false;

    jogadores[jogadorOponenteId].j_pecasVivas--;

    tabuleiro[inimigoX][inimigoY].t_temPeca = false;
    tabuleiro[inimigoX][inimigoY].t_pecaRelacionada = -1;
    tabuleiro[inimigoX][inimigoY].t_jogadorDono = -1;

    MoverPecaPara(jogadorAtual.j_id, indicePeca, origemX, origemY, destinoX, destinoY);

    casaSelecionada.s_indiceX = destinoX;
    casaSelecionada.s_indiceY = destinoY;

    casaSelecionada.s_posX = tabuleiro[destinoX][destinoY].t_posX;

    casaSelecionada.s_posY = tabuleiro[destinoX][destinoY].t_posY;

    MoverDiagonais(jogadorAtual);

    ChecarSeGanhou();

    TornarDama();

    if (!jogadores[jogadorAtual.j_id]
        .j_pecas[casaSelecionada.s_pecaSelecionada]
        .p_ehDama && PodeCapturar())
    {
        _temCasaSelecionada = true;
    }
    else
    {
        _temCasaSelecionada = false;
        TrocarJogodorAtual();
    }
}

void MostrarCaptura()
{
    if (!_temCasaSelecionada) return;

    int frente = jogadorAtual.j_id == 0 ? 1 : -1;

    int direcoes[2] = { -1, 1 };

    for (int i = 0; i < 2; i++)
    {
        int inimigoX = casaSelecionada.s_indiceX + frente;

        int inimigoY = casaSelecionada.s_indiceY + direcoes[i];

        int destinoX = inimigoX + frente;
        int destinoY = inimigoY + direcoes[i];

        bool inimigoDentro = inimigoX >= 0 && inimigoX < _numeroDeCasas && inimigoY >= 0 && inimigoY < _numeroDeCasas;

        bool destinoDentro = destinoX >= 0 && destinoX < _numeroDeCasas && destinoY >= 0 && destinoY < _numeroDeCasas;

        if (!inimigoDentro || !destinoDentro)
        {
            continue;
        }

        bool temInimigo = tabuleiro[inimigoX][inimigoY].t_temPeca && tabuleiro[inimigoX][inimigoY].t_jogadorDono != jogadorAtual.j_id;

        bool destinoLivre = !tabuleiro[destinoX][destinoY].t_temPeca;

        if (!temInimigo || !destinoLivre)
        {
            continue;
        }

        int posX = tabuleiro[destinoX][destinoY].t_posX;
        int posY = tabuleiro[destinoX][destinoY].t_posY;

        bool selecionada = _indiceDiagonalSelecionada == i;

        if (selecionada)
        {
            CV::color(0.5, 0.85, 0.5);

            CV::rectFill(posX, posY, posX + _tamanhoCasa, posY + _tamanhoCasa);

            CV::color(0, 0.75, 0);
        }
        else
        {
            CV::color(1.0, 0.534, 0.479);

            CV::rectFill(posX, posY, posX + _tamanhoCasa, posY + _tamanhoCasa);

            CV::color(1, 0, 0);
        }

        for (float m = 5; m > 0; m -= 0.1f)
        {
            CV::rect(posX - m, posY - m, posX + _tamanhoCasa + m, posY + _tamanhoCasa + m);
        }
    }
}

void MostrarJogadasAtuais()
{
    if (!_temCasaSelecionada)
    {
        return;
    }

    if (jogadorAtual.j_pecas[casaSelecionada.s_pecaSelecionada].p_ehDama)
    {
        MostrarJogadasDama();
    }
    else
    {
        MostrarJogadas();
        MostrarCaptura();
    }
}

void MostrarCenaFinal()
{
    char mensagem[100];

    sprintf(mensagem, "JOGADOR %d VENCEU!", jogadorAtual.j_id + 1);

    CV::color(0, 0, 0);

    CV::rectFill(_screenWidth / 2 - 180, _screenHeight / 2 - 80, _screenWidth / 2 + 180, _screenHeight / 2 + 80);

    CV::color(1, 1, 0);

    CV::text(_screenWidth / 2 - 70, _screenHeight / 2 + 30, "PARABENS!");

    CV::color(1, 1, 1);

    CV::text(_screenWidth / 2 - 95, _screenHeight / 2, mensagem);

    CV::text(_screenWidth / 2 - 95, _screenHeight / 2 - 30, "Aperte ESC para sair");
}

void RenderizarCena()
{
    if(_jogoFinalizado)
    {
        MostrarCenaFinal();
        return;
	}

    PintarTabuleiro();

    SelecionarCasa();

    MostrarJogadasAtuais();

    SelecionarDiagonal();

    PintarPecas(_tamanhoCasa / 2 - 10);

    DesenharManual();
}

void RenderizarDebug()
{
    DrawMouseScreenCoords();

    CV::color(0, 1, 0);

    CV::circleFill(_mouseX, _mouseY, 10, 20);
}

void AtualizarJogo()
{
    InicializarTabuleiro();

    ChecarSeGanhou();
}

void DesenharManual()
{
    int painelX = _offsetX + _tamanhoTabuleiro + 10;
    int painelY = _offsetY;

    int largura = 260;
    int altura = 320;

    CV::color(0.15, 0.15, 0.15);

    CV::rectFill(painelX, painelY - 50, painelX + largura - 10, painelY + altura);

    CV::color(1, 1, 1);

    for (int i = 0; i < 3; i++)
    {
        CV::rect(painelX - i, painelY - i  - 50, painelX + largura + i - 10, painelY + altura + i);
    }

    CV::color(1, 1, 0.75);

    int linha = altura - 40;
    int espacamento = 35;
	int margin = 10;

    CV::text(painelX + margin, painelY + linha, "CONTROLES: ");

    linha -= 20;

    CV::color(1, 1, 1);

    CV::text(painelX + margin, painelY + linha, "<- / -> = Trocar peca");

    linha -= espacamento;

    CV::text(painelX + margin, painelY + linha, "e jogada selecionada;");

    linha -= espacamento;

    CV::text(painelX + margin, painelY + linha, "ESPACO = Selecionar e");
    linha -= espacamento;

    CV::text(painelX + margin, painelY + linha, "confirmar acao;");

    linha -= espacamento;

    CV::text(painelX + margin, painelY + linha, "X = Cancelar selecao;");
    linha -= espacamento;

    CV::text(painelX + margin, painelY + linha, "ESC = Sair;");
    linha -= espacamento + margin;

    CV::color(1, 1, 0.75);

    CV::text(painelX + margin, painelY + linha, "OBJETIVO");
    linha -= espacamento;

    CV::color(1, 1, 1);

    CV::text(painelX + margin, painelY + linha, "Capture todas as");

    linha -= 25;

    CV::text(painelX + margin, painelY + linha, "pecas inimigas.");
}
void MostrarTelaInicial()
{
    CV::color(0, 0, 0);

    CV::text(_screenWidth / 2 - _offsetXMenu + 50, _screenHeight / 2 + _offsetYMenu + 20, "SUPER DAMAS 2026");

    CV::text(_screenWidth / 2 - _offsetXMenu, _screenHeight / 2, "Pressione ESPACO para iniciar");

    CV::text(_screenWidth / 2 - _offsetXMenu, _screenHeight / 2 - _offsetYMenu - 20, "Escolha o tamanho do tabuleiro:");

    char mensagem[50];

    sprintf(mensagem, "%dX%d", _numeroDeCasasSelecionado, _numeroDeCasasSelecionado);

    CV::text(_screenWidth / 2 - _offsetXMenu, _screenHeight / 2 - _offsetYMenu - 40, mensagem);

    CV::text(_screenWidth / 2 - _offsetXMenu, _screenHeight / 2 - _offsetYMenu - 70, "Use SETA CIMA / BAIXO");
}

void NaoSairMouse()
{
    if (_mouseX < 0)
    {
        _mouseX = 0;
    }

    if (_mouseX > _screenWidth)
    {
        _mouseX = _screenWidth;
    }

    if (_mouseY < 0)
    {
        _mouseY = 0;
    }

    if (_mouseY > _screenHeight)
    {
        _mouseY = _screenHeight;
    }
}

void keyboard(int key)
{
    switch (key)
    {
    case 27: // ESC
        exit(0);
        break;

    case 32: // SPACE
        if (!_jogoComecou)
        {
            PrepararJogo(_numeroDeCasasSelecionado);

            _jogoComecou = true;

            return;
        }

        if (!casaSelecionada.s_selected)
        {
            casaSelecionada.s_selected = true;
            return;
        }

        if (!_temCasaSelecionada)
        {
            _temCasaSelecionada = true;
            return;
        }

        if (!diagonalSelecionada.s_valida)
        {
            _aindaPodePegar = true;
            PegarPeca();
            return;
        }

        if (jogadorAtual.j_pecas[casaSelecionada.s_pecaSelecionada].p_ehDama)
        {
            MoverDama();
        }
        else
        {
            MoverPeca();
        }

        break;

    case 120: // X
        if (_temCasaSelecionada)
        {
            _temCasaSelecionada = false;
            return;
        }

        if (casaSelecionada.s_selected)
        {
            casaSelecionada.s_selected = false;
        }

        break;

    case 202: // seta direita
        if (!_jogoComecou)
        {
            return;
        }

        if (!casaSelecionada.s_selected)
        {
            return;
        }

        if (!_temCasaSelecionada)
        {
            MoverSelecao(1, jogadorAtual);
            return;
        }

        MoverDiagonalSelecionada(1);
        return;

    case 200: // seta esquerda
        if (!_jogoComecou)
        {
            return;
        }

        if (!casaSelecionada.s_selected)
        {
            return;
        }

        if (!_temCasaSelecionada)
        {
            MoverSelecao(-1, jogadorAtual);
            return;
        }

        MoverDiagonalSelecionada(-1);
        return;

    case 201: // seta cima
        if (_jogoComecou)
        {
            return;
        }

        _numeroDeCasasSelecionado += 2;

        if (_numeroDeCasasSelecionado > 12)
        {
            _numeroDeCasasSelecionado = 12;
        }

        break;

    case 203: // seta baixo
        if (_jogoComecou)
        {
            return;
        }

        _numeroDeCasasSelecionado -= 2;

        if (_numeroDeCasasSelecionado < 8)
        {
            _numeroDeCasasSelecionado = 8;
        }

        break;
    }
}

void keyboardUp(int key)
{
}

void DrawMouseScreenCoords()
{
}

void mouse(int button, int state, int wheel, int direction, int x, int y)
{
    _mouseX = x;
    _mouseY = y;

    NaoSairMouse();
}