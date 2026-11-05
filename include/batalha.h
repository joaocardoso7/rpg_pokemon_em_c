#ifndef BATALHA_H
#define BATALHA_H

#include "pokemon.h"

#define BATALHA_VITORIA 1
#define BATALHA_DERROTA 2
#define BATALHA_FUGA    3
#define BATALHA_CAPTURA 4

float multiplicador_tipo(const char *tipo_golpe, const char *tipo_defensor);
int calcular_dano(Pokemon *atacante, Pokemon *defensor, Ataque *golpe);
void aplicar_dano(Pokemon *p, int dano);
int tentar_captura(Treinador *t, Pokemon *selvagem);

void exibir_status(const Pokemon *p);

int escolher_ataque(const Pokemon *p);

void turno_inimigo(Pokemon *selvagem, Pokemon *alvo);
int usar_pocao(Treinador *t, Pokemon *p);
int tentar_fuga(void);

int iniciar_batalha(Treinador *t, Pokemon *selvagem);

#endif
