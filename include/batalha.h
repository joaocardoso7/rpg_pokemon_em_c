#ifndef BATALHA_H
#define BATALHA_H

#include "pokemon.h"

float multiplicador_tipo(const char *tipo_golpe, const char *tipo_defensor);
int calcular_dano(Pokemon *atacante, Pokemon *defensor, Ataque *golpe);
void aplicar_dano(Pokemon *p, int dano);
int tentar_captura(Treinador *t, Pokemon *selvagem);

void exibir_status(const Pokemon *p);

int ler_opcao(int min, int max);
int escolher_ataque(const Pokemon *p);

void turno_inimigo(Pokemon *selvagem, Pokemon *alvo);
int usar_pocao(Treinador *t, Pokemon *p);
int tentar_fuga(void);

#endif
