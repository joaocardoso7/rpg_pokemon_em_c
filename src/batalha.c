#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/batalha.h"

// Multiplicador de dano por vantagem elemental (Fogo, Planta, Agua, Normal)
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

// Com stats pequenos (~10) o divisor classico (50) zerava o dano; ajuste aqui para balancear
#define DIVISOR_DANO 10

// Formula de dano baseada no nivel, ataque e defesa
int calcular_dano(Pokemon *atacante, Pokemon *defensor, Ataque *golpe) {
    int def = defensor->defesa > 0 ? defensor->defesa : 1;
    int base = (2 * atacante->nivel / 5 + 2) * golpe->poder * atacante->ataque / def / DIVISOR_DANO + 2;
    int dano = (int)(base * multiplicador_tipo(golpe->tipo, defensor->tipo));
    
    if (dano < 1) dano = 1; // dano minimo garantido
    return dano;
}

void aplicar_dano(Pokemon *p, int dano) {
    p->vida_atual -= dano;
    if (p->vida_atual < 0) {
        p->vida_atual = 0;
    }
}

// Retorna: -1 = acao invalida (nao gasta turno), 0 = falhou, 1 = capturou
int tentar_captura(Treinador *t, Pokemon *selvagem) {
    if (t->pokebolas <= 0) {
        printf("\nSem pokebolas disponiveis na mochila!\n");
        return -1;
    }
    if (t->qtd_pokemon >= MAX_TIME) {
        printf("\nSeu time já esta cheio (maximo %d)!\n", MAX_TIME);
        return -1;
    }

    t->pokebolas--;

    // Chance de captura: 30% a 80%, dependendo da vida restante do selvagem
    int vida_maxima = selvagem->vida_max > 0 ? selvagem->vida_max : 1;
    int porcentagem_vida = (selvagem->vida_atual * 100) / vida_maxima;
    int chance = 30 + (100 - porcentagem_vida) / 2;

    printf("\nVoce arremessou uma Pokebola... (Chance estimada: %d%%)\n", chance);

    if ((rand() % 100) < chance) {
        t->time[t->qtd_pokemon] = *selvagem;
        t->qtd_pokemon++;
        printf("Sucesso! %s foi capturado e adicionado a sua equipe!\n", selvagem->nome);
        return 1;
    }

    printf("Ah nao! %s escapou da pokebola!\n", selvagem->nome);
    return 0;
}

void exibir_status(const Pokemon *p) {
    int tamanho = 20;
    int max = p->vida_max > 0 ? p->vida_max : 1;
    int cheios = (p->vida_atual * tamanho) / max;
    if (cheios > tamanho) cheios = tamanho;
    if (cheios < 0) cheios = 0;

    printf("\n----------------------------------------\n");
    printf("%s [%s] Nv.%d\n", p->nome, p->tipo, p->nivel);
    printf("HP: [");
    for (int i = 0; i < tamanho; i++) {
        putchar(i < cheios ? '#' : '-');
    }
    printf("] %d/%d\n", p->vida_atual, p->vida_max);
    printf("----------------------------------------\n");
}

static Ataque ataque_basico = {"Investida", 8, "Normal"};

// Leitor local com buffer isolado para evitar conflito com main.c
static int ler_opcao_batalha(int min, int max) {
    char buf[64];
    int op;

    while (1) {
        if (fgets(buf, sizeof(buf), stdin) == NULL) return min;
        if (sscanf(buf, "%d", &op) == 1 && op >= min && op <= max) {
            return op;
        }
        printf("Opcao invalida! Digite um numero entre %d e %d: ", min, max);
    }
}

int escolher_ataque(const Pokemon *p) {
    int validos = 0;

    printf("\nEscolha um golpe:\n");
    for (int i = 0; i < MAX_ATAQUES; i++) {
        if (p->ataques[i].nome[0] != '\0') {
            printf("  [%d] %s (Tipo: %s | Poder: %d)\n", 
                   i + 1, p->ataques[i].nome, p->ataques[i].tipo, p->ataques[i].poder);
            validos = i + 1;
        }
    }

    if (validos == 0) {
        printf("  %s nao possui golpes conhecidos! Usando %s por padrao.\n", p->nome, ataque_basico.nome);
        return -1;
    }

    printf("Golpe (1-%d): ", validos);
    return ler_opcao_batalha(1, validos) - 1;
}

#define CURA_POCAO 15
#define CHANCE_FUGA 50

void turno_inimigo(Pokemon *selvagem, Pokemon *alvo) {
    int validos[MAX_ATAQUES];
    int n = 0;

    for (int i = 0; i < MAX_ATAQUES; i++) {
        if (selvagem->ataques[i].nome[0] != '\0') {
            validos[n++] = i;
        }
    }

    Ataque *golpe = (n > 0) ? &selvagem->ataques[validos[rand() % n]] : &ataque_basico;
    int dano = calcular_dano(selvagem, alvo, golpe);
    aplicar_dano(alvo, dano);

    printf("\n[TURNO INIMIGO] %s selvagem usou %s!\n", selvagem->nome, golpe->nome);
    printf("%s recebeu %d de dano! (HP restante: %d/%d)\n", alvo->nome, dano, alvo->vida_atual, alvo->vida_max);
}

int usar_pocao(Treinador *t, Pokemon *p) {
    if (t->pocoes <= 0) {
        printf("\nVoce nao tem pocoes restantes!\n");
        return 0;
    }
    if (p->vida_atual >= p->vida_max) {
        printf("\n%s ja esta com a vida maxima!\n", p->nome);
        return 0;
    }

    t->pocoes--;
    p->vida_atual += CURA_POCAO;
    if (p->vida_atual > p->vida_max) {
        p->vida_atual = p->vida_max;
    }

    printf("\nPocao usada! %s recuperou HP. (HP atual: %d/%d | Pocoes restantes: %d)\n",
           p->nome, p->vida_atual, p->vida_max, t->pocoes);
    return 1;
}

int tentar_fuga(void) {
    if ((rand() % 100) < CHANCE_FUGA) {
        printf("\nFuga bem-sucedida!\n");
        return 1;
    }
    printf("\nNao foi possivel escapar!\n");
    return 0;
}

static Pokemon *primeiro_vivo(Treinador *t) {
    for (int i = 0; i < t->qtd_pokemon; i++) {
        if (t->time[i].vida_atual > 0) {
            return &t->time[i];
        }
    }
    return NULL;
}

// ---------- Experiencia e nivel ----------
#define XP_POR_VITORIA(nv_inimigo) ((nv_inimigo) * 10)
#define XP_PARA_PROXIMO(nv)        ((nv) * 15)
#define NIVEL_MAXIMO 100

static void ganhar_xp(Pokemon *p, int xp_ganho) {
    printf("%s ganhou %d de XP!\n", p->nome, xp_ganho);
    p->xp += xp_ganho;

    while (p->nivel < NIVEL_MAXIMO && p->xp >= XP_PARA_PROXIMO(p->nivel)) {
        p->xp -= XP_PARA_PROXIMO(p->nivel);
        p->nivel++;
        p->vida_max   += 3;
        p->vida_atual += 3;
        p->ataque++;
        p->defesa++;
        printf(">>> %s subiu para o nivel %d! (HP max %d | ATK %d | DEF %d) <<<\n",
               p->nome, p->nivel, p->vida_max, p->ataque, p->defesa);
    }
}

// ---------- Troca manual de Pokemon ----------
// Retorna 1 se trocou (gasta o turno), 0 se cancelou/invalido (nao gasta)
static int trocar_pokemon(Treinador *t, Pokemon **ativo) {
    printf("\nEscolha o Pokemon:\n");
    for (int i = 0; i < t->qtd_pokemon; i++) {
        printf("  [%d] %s Nv.%d HP %d/%d%s\n", i + 1, t->time[i].nome, t->time[i].nivel,
               t->time[i].vida_atual, t->time[i].vida_max,
               &t->time[i] == *ativo ? " (em campo)" : "");
    }
    printf("  [0] Cancelar\nEscolha: ");

    int op = ler_opcao_batalha(0, t->qtd_pokemon);
    if (op == 0) return 0;

    Pokemon *novo = &t->time[op - 1];
    if (novo == *ativo)       { printf("\n%s ja esta em campo!\n", novo->nome); return 0; }
    if (novo->vida_atual <= 0) { printf("\n%s esta desmaiado!\n", novo->nome);   return 0; }

    *ativo = novo;
    printf("\nVoce enviou %s para a batalha!\n", novo->nome);
    return 1;
}

int iniciar_batalha(Treinador *t, Pokemon *selvagem) {
    Pokemon *ativo = primeiro_vivo(t);

    if (ativo == NULL) {
        printf("\nTodos os seus Pokemons estao desmaiados! Voce nao pode batalhar.\n");
        return BATALHA_DERROTA;
    }

    printf("\n========================================\n");
    printf("     UM %s SELVAGEM APARECEU!     \n", selvagem->nome);
    printf("========================================\n");

    while (1) {
        exibir_status(selvagem);
        exibir_status(ativo);

        printf("\nComandos:\n");
        printf("  [1] Lutar\n");
        printf("  [2] Capturar (Pokebolas: %d)\n", t->pokebolas);
        printf("  [3] Usar Pocao (Pocoes: %d)\n", t->pocoes);
        printf("  [4] Fugir\n");
        printf("  [5] Trocar Pokemon\n");
        printf("Escolha: ");

        int op = ler_opcao_batalha(1, 5);

        if (op == 1) {
            int idx = escolher_ataque(ativo);
            Ataque *golpe = (idx >= 0) ? &ativo->ataques[idx] : &ataque_basico;

            int dano = calcular_dano(ativo, selvagem, golpe);
            aplicar_dano(selvagem, dano);

            printf("\n%s usou %s! Causou %d de dano.\n", ativo->nome, golpe->nome, dano);

            if (selvagem->vida_atual == 0) {
                printf("\n>>> VITORIA! O %s selvagem foi derrotado! <<<\n", selvagem->nome);
                ganhar_xp(ativo, XP_POR_VITORIA(selvagem->nivel));
                return BATALHA_VITORIA;
            }
        } else if (op == 2) {
            int r = tentar_captura(t, selvagem);
            if (r == -1) continue; // entrada invalida, nao perde turno
            if (r == 1)  return BATALHA_CAPTURA;
        } else if (op == 3) {
            if (!usar_pocao(t, ativo)) continue; // sem efeito, nao perde turno
        } else if (op == 4) {
            if (tentar_fuga()) return BATALHA_FUGA;
        } else if (op == 5) {
            if (!trocar_pokemon(t, &ativo)) continue; // cancelou/invalido: nao perde turno
        }

        // Turno de resposta do Pokémon selvagem
        turno_inimigo(selvagem, ativo);

        // Verifica se o Pokémon ativo caiu
        if (ativo->vida_atual == 0) {
            printf("\n%s desmaiou em combate!\n", ativo->nome);
            ativo = primeiro_vivo(t);

            if (ativo == NULL) {
                printf("\n>>> DERROTA! Todos os seus Pokemons desmaiaram! <<<\n");
                return BATALHA_DERROTA;
            }

            printf("Voce enviou %s para a batalha!\n", ativo->nome);
        }
    }
}
