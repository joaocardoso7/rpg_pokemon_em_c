#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/batalha.h"

float multiplicador_tipo(const char *tipo_golpe, const char *tipo_defensor) {
    if (strcmp(tipo_golpe, "Fogo") == 0) {
        if (strcmp(tipo_defensor, "Planta") == 0) return 2.0f;
        if (strcmp(tipo_defensor, "Agua") == 0)   return 0.5f;
    } else if (strcmp(tipo_golpe, "Agua") == 0) {
        if (strcmp(tipo_defensor, "Fogo") == 0)   return 2.0f;
        if (strcmp(tipo_defensor, "Planta") == 0) return 0.5f;
    } else if (strcmp(tipo_golpe, "Planta") == 0) {
        if (strcmp(tipo_defensor, "Agua") == 0)   return 2.0f;
        if (strcmp(tipo_defensor, "Fogo") == 0)   return 0.5f;
    }
    return 1.0f;
}

// Formula baseada no nivel: com nivel 5, ataque 10, defesa 10 e poder 40
// o dano normal e 5 (10 com vantagem de tipo).
int calcular_dano(Pokemon *atacante, Pokemon *defensor, Ataque *golpe) {
    int defesa = defensor->defesa > 0 ? defensor->defesa : 1;
    int base = (2 * atacante->nivel / 5 + 2) * golpe->poder * atacante->ataque / defesa / 50 + 2;
    int dano = (int)(base * multiplicador_tipo(golpe->tipo, defensor->tipo));
    if (dano < 1) dano = 1;  // dano minimo
    return dano;
}

void aplicar_dano(Pokemon *p, int dano) {
    p->vida_atual = p->vida_atual - dano;
    if (p->vida_atual < 0) p->vida_atual = 0;  // nunca negativa
}

// Retorna: -1 = nao foi possivel tentar (nao gasta turno),
//           0 = tentou e falhou, 1 = capturou.
int tentar_captura(Treinador *t, Pokemon *selvagem) {
    if (t->pokebolas <= 0) {
        printf("Sem pokebolas disponiveis!\n");
        return -1;
    }
    if (t->qtd_pokemon >= MAX_TIME) {
        printf("Seu time esta cheio!\n");
        return -1;
    }
    t->pokebolas--;

    // Quanto menos vida, maior a chance (de 30% ate 80%)
    int chance = 30 + (100 - selvagem->vida_atual * 100 / selvagem->vida_max) / 2;

    if (rand() % 100 < chance) {
        t->time[t->qtd_pokemon] = *selvagem;
        t->qtd_pokemon++;
        printf("Pokemon capturado e adicionado ao time: %s!\n", selvagem->nome);
        return 1;
    }
    printf("%s escapou da pokebola!\n", selvagem->nome);
    return 0;
}

void exibir_status(const Pokemon *p) {
    int tamanho = 20;
    int cheios = p->vida_atual * tamanho / p->vida_max;

    printf("%s (%s) Nv.%d\n", p->nome, p->tipo, p->nivel);
    printf("HP [");
    for (int i = 0; i < tamanho; i++) {
        putchar(i < cheios ? '#' : '-');
    }
    printf("] %d/%d\n\n", p->vida_atual, p->vida_max);
}

// Golpe usado quando o Pokemon nao tem nenhum ataque cadastrado.
static Ataque ataque_basico = {"Investida", 40, "Normal"};

int ler_opcao(int min, int max) {
    char buf[32];
    int op;

    while (1) {
        if (fgets(buf, sizeof(buf), stdin) == NULL) return min;  // EOF
        if (sscanf(buf, "%d", &op) == 1 && op >= min && op <= max) {
            return op;
        }
        printf("Opcao invalida! Digite um numero entre %d e %d: ", min, max);
    }
}

// Retorna o indice (0 a MAX_ATAQUES-1) do ataque escolhido,
// ou -1 se o Pokemon nao tem ataques (nesse caso usa-se o golpe basico).
// Slot vazio: nome[0] == '\0'.
int escolher_ataque(const Pokemon *p) {
    int validos = 0;

    printf("Escolha o ataque:\n");
    for (int i = 0; i < MAX_ATAQUES; i++) {
        if (p->ataques[i].nome[0] != '\0') {
            printf("%d - %s (%s, poder %d)\n", i + 1,
                   p->ataques[i].nome, p->ataques[i].tipo, p->ataques[i].poder);
            validos = i + 1;
        }
    }
    if (validos == 0) {
        printf("%s nao tem ataques! Usando %s.\n", p->nome, ataque_basico.nome);
        return -1;
    }
    printf("> ");
    return ler_opcao(1, validos) - 1;
}

#define CURA_POCAO 10
#define CHANCE_FUGA 50

void turno_inimigo(Pokemon *selvagem, Pokemon *alvo) {
    int validos[MAX_ATAQUES];
    int n = 0;
    for (int i = 0; i < MAX_ATAQUES; i++) {
        if (selvagem->ataques[i].nome[0] != '\0') validos[n++] = i;
    }

    Ataque *golpe = (n > 0) ? &selvagem->ataques[validos[rand() % n]] : &ataque_basico;
    int dano = calcular_dano(selvagem, alvo, golpe);
    aplicar_dano(alvo, dano);
    printf("%s usou %s! %s sofreu %d de dano.\n",
           selvagem->nome, golpe->nome, alvo->nome, dano);
}

// Retorna 1 se usou a pocao, 0 se nao usou (e nao gasta o turno).
int usar_pocao(Treinador *t, Pokemon *p) {
    if (t->pocoes <= 0) {
        printf("Sem pocoes disponiveis!\n");
        return 0;
    }
    if (p->vida_atual == p->vida_max) {
        printf("%s ja esta com a vida cheia!\n", p->nome);
        return 0;
    }
    t->pocoes--;
    p->vida_atual += CURA_POCAO;
    if (p->vida_atual > p->vida_max) p->vida_atual = p->vida_max;  // trava
    printf("%s recuperou vida! (%d/%d)\n", p->nome, p->vida_atual, p->vida_max);
    return 1;
}

// Retorna 1 se conseguiu fugir, 0 se falhou.
int tentar_fuga(void) {
    if (rand() % 100 < CHANCE_FUGA) {
        printf("Fuga realizada com sucesso!\n");
        return 1;
    }
    printf("Falha ao fugir!\n");
    return 0;
}

static Pokemon *primeiro_vivo(Treinador *t) {
    for (int i = 0; i < t->qtd_pokemon; i++) {
        if (t->time[i].vida_atual > 0) return &t->time[i];
    }
    return NULL;
}

int iniciar_batalha(Treinador *t, Pokemon *selvagem) {
    Pokemon *ativo = primeiro_vivo(t);
    if (ativo == NULL) {
        printf("Todos os seus Pokemon estao desmaiados!\n");
        return BATALHA_DERROTA;
    }

    printf("\nUm %s selvagem apareceu!\n\n", selvagem->nome);

    while (1) {
        exibir_status(selvagem);
        exibir_status(ativo);
        printf("1 - Atacar\n2 - Capturar (%d pokebolas)\n3 - Usar Pocao (%d)\n4 - Fugir\n> ",
               t->pokebolas, t->pocoes);
        int op = ler_opcao(1, 4);

        if (op == 1) {
            int i = escolher_ataque(ativo);
            Ataque *golpe = (i >= 0) ? &ativo->ataques[i] : &ataque_basico;
            int dano = calcular_dano(ativo, selvagem, golpe);
            aplicar_dano(selvagem, dano);
            printf("%s usou %s! Causou %d de dano.\n", ativo->nome, golpe->nome, dano);

            if (selvagem->vida_atual == 0) {
                printf("Vitoria! %s selvagem foi derrotado!\n", selvagem->nome);
                return BATALHA_VITORIA;
            }
        } else if (op == 2) {
            int r = tentar_captura(t, selvagem);
            if (r == -1) continue;               // sem pokebola/time cheio: nao gasta turno
            if (r == 1) return BATALHA_CAPTURA;
        } else if (op == 3) {
            if (!usar_pocao(t, ativo)) continue; // sem pocao/vida cheia: nao gasta turno
        } else {
            if (tentar_fuga()) return BATALHA_FUGA;
        }

        turno_inimigo(selvagem, ativo);

        if (ativo->vida_atual == 0) {
            printf("%s desmaiou!\n", ativo->nome);
            ativo = primeiro_vivo(t);
            if (ativo == NULL) {
                printf("Todos os Pokemon desmaiaram!\n");
                return BATALHA_DERROTA;
            }
            printf("Vai, %s!\n", ativo->nome);
        }
    }
}
