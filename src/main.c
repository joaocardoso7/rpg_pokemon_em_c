#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/pokemon.h"
#include "../include/batalha.h"
#include "../include/arquivos.h"
#include "../include/tone.h"

/* ---------- Entrada de dados ---------- */

void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int ler_opcao(int min, int max) {
    int valor;
    while (1) {
        int lidos = scanf("%d", &valor);
        if (lidos == EOF) {
            printf("\nSessao finalizada.\n");
            exit(0);
        }
        if (lidos != 1) {
            limpar_buffer();
            printf("Entrada invalida! Digite apenas numeros: ");
            continue;
        }
        limpar_buffer();
        if (valor >= min && valor <= max) {
            return valor;
        }
        printf("Opcao invalida! Escolha entre %d e %d: ", min, max);
    }
}

/* ---------- Inicialização ---------- */

static void definir_ataque(Ataque *a, const char *nome, int poder, const char *tipo) {
    snprintf(a->nome, sizeof(a->nome), "%s", nome);
    a->poder = poder;
    snprintf(a->tipo, sizeof(a->tipo), "%s", tipo);
}

static void definir_pokemon_inicial(Pokemon *p, int escolha) {
    memset(p, 0, sizeof(*p));

    if (escolha == 1) {
        snprintf(p->nome, sizeof(p->nome), "Charmander");
        snprintf(p->tipo, sizeof(p->tipo), "Fogo");
        definir_ataque(&p->ataques[1], "Brasa", 12, "Fogo");
    } else if (escolha == 2) {
        snprintf(p->nome, sizeof(p->nome), "Bulbasaur");
        snprintf(p->tipo, sizeof(p->tipo), "Planta");
        definir_ataque(&p->ataques[1], "Chicote de Vinha", 12, "Planta");
    } else {
        snprintf(p->nome, sizeof(p->nome), "Squirtle");
        snprintf(p->tipo, sizeof(p->tipo), "Agua");
        definir_ataque(&p->ataques[1], "Jato de Agua", 12, "Agua");
    }
    definir_ataque(&p->ataques[0], "Investida", 8, "Normal");

    p->nivel = 5;
    p->vida_max = 22;
    p->vida_atual = 22;
    p->ataque = 10;
    p->defesa = 10;
}

void novo_jogo(Treinador *jogador) {
    char nome[50];

    printf("\n========================================\n");
    printf("         NOVA JORNADA POKEMON           \n");
    printf("========================================\n");
    printf("Prof. Carvalho: Ola! Qual e o seu nome, jovem treinador?\n> ");

    if (fgets(nome, sizeof(nome), stdin) != NULL) {
        nome[strcspn(nome, "\r\n")] = '\0';
    } else {
        nome[0] = '\0';
    }
    if (nome[0] == '\0') {
        snprintf(nome, sizeof(nome), "Ash");
    }

    inicializar_treinador(jogador, nome);

    printf("\nProf. Carvalho: Escolha o seu primeiro parceiro:\n");
    printf("  [1] Charmander (Fogo)\n");
    printf("  [2] Bulbasaur  (Planta)\n");
    printf("  [3] Squirtle   (Agua)\n");
    printf("Escolha (1-3): ");
    int escolha = ler_opcao(1, 3);

    definir_pokemon_inicial(&jogador->time[0], escolha);
    jogador->qtd_pokemon = 1;

    printf("\nExcelente escolha! Voce recebeu um %s!\n", jogador->time[0].nome);
    tocar_som_vitoria();
    mostrar_pokemon(&jogador->time[0]);
}

/* ---------- Loop de Exploração e Eventos ---------- */

static int nivel_medio_time(const Treinador *jogador) {
    if (jogador->qtd_pokemon == 0) return 5;
    int soma = 0;
    for (int i = 0; i < jogador->qtd_pokemon; i++) {
        soma += jogador->time[i].nivel;
    }
    return soma / jogador->qtd_pokemon;
}

void evento_explorar(Treinador *jogador) {
    if (time_derrotado(jogador)) {
        printf("\nSeus Pokemons estao todos desmaiados! Va ao Centro Pokemon descansar antes de lutar.\n");
        return;
    }

    // Calcula nivel próximo ao do jogador com leve variação (-1 a +1)
    int nv_base = nivel_medio_time(jogador);
    int variacao = (rand() % 3) - 1;
    int nivel_inimigo = nv_base + variacao;
    if (nivel_inimigo < 2) nivel_inimigo = 2;

    Pokemon selvagem;
    gerar_pokemon_selvagem(&selvagem, nivel_inimigo);

    int resultado = iniciar_batalha(jogador, &selvagem);

    switch (resultado) {
        case BATALHA_VITORIA:
            tocar_som_vitoria();
            // Recompensa itens aleatórios após vitória
            if (rand() % 2 == 0) {
                printf("[RECOMPENSA] Voce encontrou 1 Pocao e 1 Pokebola no caminho!\n");
                adicionar_itens(jogador, 1, 1);
            }
            break;
        case BATALHA_CAPTURA:
            tocar_som_captura();
            break;
        case BATALHA_DERROTA:
            tocar_som_derrota();
            printf("\nVoce correu para o Centro Pokemon mais proximo apos a derrota...\n");
            curar_time(jogador);
            break;
        case BATALHA_FUGA:
            printf("Voce conseguiu escapar em seguranca!\n");
            break;
    }
}

void centro_pokemon(Treinador *jogador) {
    printf("\nEnfermeira Joy: Bem-vindo ao Centro Pokemon! Seus companheiros estao novinhos em folha.\n");
    curar_time(jogador);
    printf("Todos os Pokemons foram completamente curados!\n");
}

void menu_exploracao(Treinador *jogador) {
    int voltar = 0;

    while (!voltar) {
        printf("\n========== AREA DE EXPLORACAO ==========\n");
        printf("1. Explorar matinho (Batalha selvagem)\n");
        printf("2. Centro Pokemon (Curar time)\n");
        printf("3. Abrir Mochila e Time\n");
        printf("4. Salvar jogo\n");
        printf("5. Voltar ao menu principal\n");
        printf("Escolha uma acao: ");

        switch (ler_opcao(1, 5)) {
            case 1:
                evento_explorar(jogador);
                break;
            case 2:
                centro_pokemon(jogador);
                break;
            case 3:
                mostrar_time(jogador);
                mostrar_inventario(jogador);
                break;
            case 4:
                salvar_jogo(jogador);
                break;
            case 5:
                voltar = 1;
                break;
        }
    }
}

/* ---------- Ponto de Entrada ---------- */

int main(void) {
    srand((unsigned int)time(NULL));

    int rodando = 1;

    tocar_musica_tema();

    while (rodando) {
        printf("\n=============================\n");
        printf("       RPG POKEMON EM C      \n");
        printf("=============================\n");
        printf("1. Novo Jogo\n");
        printf("2. Carregar Jogo\n");
        printf("3. Sair\n");
        printf("Escolha uma opcao: ");

        switch (ler_opcao(1, 3)) {
            case 1: {
                Treinador jogador;
                novo_jogo(&jogador);
                menu_exploracao(&jogador);
                break;
            }
            case 2: {
                Treinador jogador;
                if (carregar_jogo(&jogador)) {
                    menu_exploracao(&jogador);
                }
                break;
            }
            case 3:
                printf("\nEncerrando a jornada. Ate a proxima!\n");
                rodando = 0;
                break;
        }
    }

    return 0;
}
