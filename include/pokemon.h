#ifndef POKEMON_H
#define POKEMON_H

#define MAX_ATAQUES 4
#define MAX_TIME 6

#define POKEBOLAS_INICIAIS 5
#define POCOES_INICIAIS 3

typedef struct {
    char nome[30];
    int poder;
    char tipo[15];
} Ataque;

typedef struct {
    char nome[30];
    char tipo[15];
    int vida_max;
    int vida_atual;
    int nivel;
    int ataque;
    int defesa;
    int xp;
    Ataque ataques[MAX_ATAQUES];
} Pokemon;

typedef struct {
    char nome[50];
    Pokemon time[MAX_TIME];
    int qtd_pokemon;
    int pokebolas;
    int pocoes;
} Treinador;

void criar_pokemon(Pokemon *p, const char *nome, const char *tipo,
                   int nivel, int vida_max, int ataque, int defesa);

int adicionar_ataque(Pokemon *p, const char *nome, int poder, const char *tipo);

int contar_ataques(const Pokemon *p);

int tamanho_catalogo(void);

void gerar_pokemon_selvagem(Pokemon *p, int nivel);

void mostrar_pokemon(const Pokemon *pokemon);
void mostrar_time(const Treinador *treinador);
void mostrar_inventario(const Treinador *treinador);

void inicializar_treinador(Treinador *treinador, const char *nome);

int adicionar_ao_time(Treinador *treinador, const Pokemon *pokemon);

void adicionar_itens(Treinador *treinador, int pokebolas, int pocoes);

void curar_time(Treinador *treinador);

int time_derrotado(const Treinador *treinador);

#endif
