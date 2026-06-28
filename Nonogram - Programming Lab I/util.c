#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

#include "util.h"

void inicia_tela(Tela *t, int larg, int alt, char *nome) {
    t->_mouse.x = t->_mouse.y = t->_tecla = 0;
    t->_botao = false;

    if (!al_init()) {
        fprintf(stderr, "%s", "falha na inicializacao do allegro\n");
        exit(1);
    }

    t->display = al_create_display(larg, alt);
    if (t->display == NULL) {
      fprintf(stderr, "%s", "falha ao criar display do allegro\n");
      exit(1);
    }

    t->janela.pos.x = t->janela.pos.y = 0;
    t->tam.larg = larg;
    t->tam.alt = alt;
    t->janela.tam.larg = t->tam.larg;
    t->janela.tam.alt = t->tam.alt;

    al_set_window_title(t->display, nome);

    t->ac_fundo = al_map_rgb(255, 255, 255);
    t->ac_cor = al_map_rgb(0, 0, 0);

    al_install_mouse();
    al_install_keyboard();
    al_init_primitives_addon();

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    t->fonte = al_load_font("data/Audiowide-Regular.ttf", 15, 0);
    if (t->fonte == NULL) {
      fprintf(stderr, "%s",  "falha ao carregar fonte do allegro\n");
      exit(1);
    }

    t->queue = al_create_event_queue();
    if (t->queue == NULL) {
      fprintf(stderr, "%s",  "falha ao buscar eventos do allegro\n");
      exit(1);
    }

    al_register_event_source(t->queue, al_get_keyboard_event_source());
    al_register_event_source(t->queue, al_get_display_event_source(t->display));
    al_register_event_source(t->queue, al_get_mouse_event_source());
}

void limpa_tela(Tela *t) {
    al_clear_to_color(t->ac_fundo);
}

void mostra_tela() {
    al_flip_display();
}

void espera(double ms) {
    al_rest(ms / 1e3);
}

void finaliza_tela(Tela *t) {
    // programa vai fechar
    al_destroy_display(t->display);
    al_destroy_event_queue(t->queue);
    al_destroy_font(t->fonte);
}

int codigo_tecla(Tela *t) {
    int tecla;
    processa_eventos(t);
    tecla = t->_tecla;
    t->_tecla = 0;
    return tecla;
}

void processa_eventos(Tela *t) {
    ALLEGRO_EVENT event;

    while (al_get_next_event(t->queue, &event)) {
        switch (event.type) {
            /* _tecla foi pressionada */
            case ALLEGRO_EVENT_KEY_DOWN: {
                t->_tecla = event.keyboard.keycode;
                break;
            }
            case ALLEGRO_EVENT_MOUSE_AXES: {
                t->_mouse.x = event.mouse.x;
                t->_mouse.y = event.mouse.y;
                break;
            }
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN: {
                if (event.mouse.button == 1)
                    t->_botao = true;
                break;
            }
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP: {
                if (event.mouse.button == 1)
                    t->_botao = false;
                break;
            }
            case ALLEGRO_EVENT_DISPLAY_CLOSE: {
                t->_tecla = ALLEGRO_EVENT_DISPLAY_CLOSE;
                break;
            }
            case ALLEGRO_EVENT_TIMER: {
                break;
            }
            default:
                break;
        }
    }
}

void desenha_retangulo(Retangulo r, Tela *t, bool filled) {
    if (filled) {
        al_draw_filled_rectangle(
            r.pos.x, r.pos.y,

            r.pos.x + r.tam.larg, r.pos.y + r.tam.alt, 

            t->ac_cor);
    } else {
        al_draw_rectangle(

            r.pos.x, r.pos.y,

            r.pos.x + r.tam.larg, r.pos.y + r.tam.alt, 

            t->ac_cor, 2);
    }
}

void desenha_botao(Botao *b, Tela *t, bool filled) {
    if (filled) {
        al_draw_filled_rectangle(

            b->ret.pos.x, b->ret.pos.y,
 
           b->ret.pos.x + b->ret.tam.larg, b->ret.pos.y + b->ret.tam.alt, 

           t->ac_cor);
    } else {
        al_draw_rectangle(

            b->ret.pos.x, b->ret.pos.y,

            b->ret.pos.x + b->ret.tam.larg, b->ret.pos.y + b->ret.tam.alt, 

            t->ac_cor, 2);
            al_draw_text(t->fonte, t->ac_cor, b->ret.pos.x + b->ret.tam.larg/2, b->ret.pos.y + b->ret.tam.alt/2 - 10, ALLEGRO_ALIGN_CENTRE, b->texto);
    }
}

void desenha_linha(Ponto p1, Ponto p2, Tela *t) {
    al_draw_line(p1.x, p1.y, p2.x, p2.y, t->ac_cor, 2);
}

void desenha_x_dentro_ret(Retangulo r, Tela *t) {
    int offset = 5;
    Tamanho tam = r.tam;

    Ponto p1 = { r.pos.x + offset, r.pos.y + offset };
    Ponto p2 = { r.pos.x + tam.larg - offset, r.pos.y + tam.alt - offset };
    desenha_linha(p1, p2, t);

    Ponto p3 = { r.pos.x + tam.larg - offset, r.pos.y + offset };
    Ponto p4 = { r.pos.x + offset, r.pos.y + tam.alt - offset };
    desenha_linha(p3, p4, t);
}

int ajeita_rgb(int x) {
  return x < 0 ? 0 : (x > 1 ? 1 : x);
}

void define_cor(Tela *t, Cor c) {
    int R, G, B;
    R = ajeita_rgb(c.r) * ((1 << 8) - 1);
    G = ajeita_rgb(c.g) * ((1 << 8) - 1);
    B = ajeita_rgb(c.b) * ((1 << 8) - 1);
    t->ac_cor = al_map_rgb(R, G, B);
}

void escreve_texto(Tela *t, Ponto p, char *s) {
    al_draw_text(t->fonte, t->ac_cor, p.x, p.y, ALLEGRO_ALIGN_LEFT, s);
}

Ponto posicao_mouse(Tela *t) {
    processa_eventos(t);
    return t->_mouse;
}

bool botao_clicado(Tela *t) {
    processa_eventos(t);
    return t->_botao;
}

Tamanho tamanho_texto(Tela *t, char *s) {
    Tamanho tam;
    int bbx, bby, bbw, bbh;
    al_get_text_dimensions(t->fonte, s, &bbx, &bby, &bbw, &bbh);
    tam.larg = bbw;
    tam.alt = bbh;
    return tam;
}

bool ret_contains_pt(Retangulo r, Ponto p) {
    return (p.x <= r.pos.x + r.tam.larg) && (p.y <= r.pos.y + r.tam.alt);
}
