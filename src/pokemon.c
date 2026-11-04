#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pokemon.h"

void criar_pokemon(Pokemon *p, const char *nome, const char *tipo,
                   int nivel, int vida_max, int ataque, int defesa) {
    memset(p, 0, sizeof(*p));

    snprintf(p->nome, sizeof(p->nome), "%s", nome);
    snprintf(p->tipo, sizeof(p->tipo), "%s", tipo);

    p->nivel = nivel < 1 ? 1 : nivel;
    p->vida_max = vida_max < 1 ? 1 : vida_max;
    p->vida_atual = p->vida_max;
    p->ataque = ataque < 1 ? 1 : ataque;
    p->defesa = defesa < 1 ? 1 : defesa;
}

int adicionar_ataque(Pokemon *p, const char *nome, int poder, const char *tipo) {
    for (int i = 0; i < MAX_ATAQUES; i++) {
        if (p->ataques[i].nome[0] == '\0') {
            snprintf(p->ataques[i].nome, sizeof(p->ataques[i].nome), "%s", nome);
            snprintf(p->ataques[i].tipo, sizeof(p->ataques[i].tipo), "%s", tipo);
            p->ataques[i].poder = poder;
            return 1;
        }
    }
    return 0;
}

int contar_ataques(const Pokemon *p) {
    int n = 0;
    for (int i = 0; i < MAX_ATAQUES; i++) {
        if (p->ataques[i].nome[0] != '\0') n++;
    }
    return n;
}

typedef struct {
    const char *nome;
    const char *tipo;
    int vida;
    int ataque;
    int defesa;
    Ataque golpes[2];
} Especie;

static const Especie CATALOGO[] = {
    {"Rattata",   "Normal", 16, 9,  7,  {{"Investida", 8, "Normal"}, {"Mordida", 10, "Normal"}}},
    {"Pidgey",    "Normal", 17, 8,  8,  {{"Investida", 8, "Normal"}, {"Rajada de Vento", 10, "Normal"}}},
    {"Caterpie",  "Planta", 15, 7,  8,  {{"Investida", 8, "Normal"}, {"Folha Navalha", 10, "Planta"}}},
    {"Oddish",    "Planta", 18, 8,  9,  {{"Absorver", 9, "Planta"},  {"Folha Navalha", 10, "Planta"}}},
    {"Vulpix",    "Fogo",   17, 10, 8,  {{"Investida", 8, "Normal"}, {"Brasa", 11, "Fogo"}}},
    {"Growlithe", "Fogo",   19, 10, 9,  {{"Mordida", 10, "Normal"},  {"Brasa", 11, "Fogo"}}},
    {"Poliwag",   "Agua",   17, 8,  9,  {{"Investida", 8, "Normal"}, {"Bolhas", 10, "Agua"}}},
    {"Psyduck",   "Agua",   18, 9,  8,  {{"Arranhao", 8, "Normal"},  {"Jato de Agua", 11, "Agua"}}},
};

#define TAMANHO_CATALOGO ((int)(sizeof(CATALOGO) / sizeof(CATALOGO[0])))

int tamanho_catalogo(void) {
    return TAMANHO_CATALOGO;
}

void gerar_pokemon_selvagem(Pokemon *p, int nivel) {
    if (nivel < 1) nivel = 1;
    if (nivel > 100) nivel = 100;

    const Especie *e = &CATALOGO[rand() % TAMANHO_CATALOGO];

    int diferenca = nivel - 5;
    criar_pokemon(p, e->nome, e->tipo, nivel,
                  e->vida + diferenca * 2,
                  e->ataque + diferenca,
                  e->defesa + diferenca);

    for (int i = 0; i < 2; i++) {
        adicionar_ataque(p, e->golpes[i].nome, e->golpes[i].poder, e->golpes[i].tipo);
    }
}

void mostrar_pokemon(const Pokemon *pokemon) {
    printf("\n===== POKEMON =====\n");
    printf("Nome: %s\n", pokemon->nome);
    printf("Tipo: %s\n", pokemon->tipo);
    printf("Nivel: %d\n", pokemon->nivel);
    printf("Vida: %d/%d%s\n", pokemon->vida_atual, pokemon->vida_max,
           pokemon->vida_atual == 0 ? " (desmaiado)" : "");
    printf("Ataque: %d\n", pokemon->ataque);
    printf("Defesa: %d\n", pokemon->defesa);

    printf("Golpes:\n");
    if (contar_ataques(pokemon) == 0) {
        printf("  (nenhum)\n");
    }
    for (int i = 0; i < MAX_ATAQUES; i++) {
        const Ataque *a = &pokemon->ataques[i];
        if (a->nome[0] != '\0') {
            printf("  - %s (%s, poder %d)\n", a->nome, a->tipo, a->poder);
        }
    }
}

void mostrar_time(const Treinador *treinador) {
    printf("\n===== MEU TIME (%d/%d) =====\n", treinador->qtd_pokemon, MAX_TIME);

    if (treinador->qtd_pokemon == 0) {
        printf("Nenhum Pokemon no time.\n");
        return;
    }
    for (int i = 0; i < treinador->qtd_pokemon; i++) {
        printf("\nPokemon %d:", i + 1);
        mostrar_pokemon(&treinador->time[i]);
    }
}

void mostrar_inventario(const Treinador *treinador) {
    printf("\n===== INVENTARIO =====\n");
    printf("Treinador: %s\n", treinador->nome);
    printf("Pokebolas: %d\n", treinador->pokebolas);
    printf("Pocoes: %d\n", treinador->pocoes);
}

void inicializar_treinador(Treinador *treinador, const char *nome) {
    memset(treinador, 0, sizeof(*treinador));

    snprintf(treinador->nome, sizeof(treinador->nome), "%s", nome);
    treinador->qtd_pokemon = 0;
    treinador->pokebolas = POKEBOLAS_INICIAIS;
    treinador->pocoes = POCOES_INICIAIS;
}

int adicionar_ao_time(Treinador *treinador, const Pokemon *pokemon) {
    if (treinador->qtd_pokemon >= MAX_TIME) {
        return 0;
    }
    treinador->time[treinador->qtd_pokemon] = *pokemon;
    treinador->qtd_pokemon++;
    return 1;
}

void adicionar_itens(Treinador *treinador, int pokebolas, int pocoes) {
    if (pokebolas > 0) treinador->pokebolas += pokebolas;
    if (pocoes > 0) treinador->pocoes += pocoes;
}

void curar_time(Treinador *treinador) {
    for (int i = 0; i < treinador->qtd_pokemon; i++) {
        treinador->time[i].vida_atual = treinador->time[i].vida_max;
    }
}

int time_derrotado(const Treinador *treinador) {
    for (int i = 0; i < treinador->qtd_pokemon; i++) {
        if (treinador->time[i].vida_atual > 0) return 0;
    }
    return 1;
}
