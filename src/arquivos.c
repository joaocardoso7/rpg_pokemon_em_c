#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/arquivos.h"

/* Mude a versao sempre que o formato do save mudar */
#define CABECALHO_SAVE "# SAVE_POKEMON_RPG v2"

int verificar_ou_criar_save(const char *caminho) {
    FILE *arquivo = fopen(caminho, "r");
    if (arquivo == NULL) {
        arquivo = fopen(caminho, "w");
        if (arquivo == NULL) return 0;
        fclose(arquivo);
    } else {
        fclose(arquivo);
    }
    return 1;
}

int salvar_jogo(const Treinador *jogador) {
    FILE *arq = fopen(ARQUIVO_SAVE, "w");
    if (arq == NULL) {
        printf("\n[ERRO] Nao foi possivel abrir o arquivo para salvar.\n");
        return 0;
    }

    fprintf(arq, "%s\n", CABECALHO_SAVE);
    fprintf(arq, "%s\n", jogador->nome);
    fprintf(arq, "%d %d %d\n", jogador->qtd_pokemon, jogador->pokebolas, jogador->pocoes);

    for (int i = 0; i < jogador->qtd_pokemon; i++) {
        const Pokemon *p = &jogador->time[i];
        fprintf(arq, "%s\n%s\n", p->nome, p->tipo);
        fprintf(arq, "%d %d %d %d %d %d\n",
                p->nivel, p->vida_max, p->vida_atual, p->ataque, p->defesa, p->xp);

        for (int j = 0; j < MAX_ATAQUES; j++) {
            if (p->ataques[j].nome[0] != '\0') {
                fprintf(arq, "%s\n%d %s\n", p->ataques[j].nome, p->ataques[j].poder, p->ataques[j].tipo);
            } else {
                fprintf(arq, "VAZIO\n0 VAZIO\n");
            }
        }
    }

    fclose(arq);
    printf("\n[SUCESSO] Jogo salvo em '%s' com sucesso!\n", ARQUIVO_SAVE);
    return 1;
}

/* Le uma linha inteira (aceita espacos) e remove o \n / \r\n do final */
static int ler_linha(FILE *arq, char *dest, size_t tam) {
    if (!fgets(dest, (int)tam, arq)) return 0;
    dest[strcspn(dest, "\r\n")] = '\0';
    return 1;
}

static int ler_pokemon(FILE *arq, Pokemon *p) {
    if (!ler_linha(arq, p->nome, sizeof(p->nome))) return 0;
    if (!ler_linha(arq, p->tipo, sizeof(p->tipo))) return 0;

    if (fscanf(arq, "%d %d %d %d %d %d\n",
               &p->nivel, &p->vida_max, &p->vida_atual,
               &p->ataque, &p->defesa, &p->xp) != 6) return 0;

    /* valores que nao fazem sentido = save adulterado/corrompido */
    if (p->nivel < 1 || p->vida_max < 1 || p->vida_atual < 0 || p->vida_atual > p->vida_max
        || p->ataque < 1 || p->defesa < 1 || p->xp < 0) return 0;

    for (int j = 0; j < MAX_ATAQUES; j++) {
        char nome_ataque[30];
        char tipo_ataque[15];
        int poder;

        if (!ler_linha(arq, nome_ataque, sizeof(nome_ataque))) return 0;
        if (fscanf(arq, "%d %14s\n", &poder, tipo_ataque) != 2) return 0;

        if (strcmp(nome_ataque, "VAZIO") != 0) {
            snprintf(p->ataques[j].nome, sizeof(p->ataques[j].nome), "%s", nome_ataque);
            p->ataques[j].poder = poder;
            snprintf(p->ataques[j].tipo, sizeof(p->ataques[j].tipo), "%s", tipo_ataque);
        }
    }
    return 1;
}

int carregar_jogo(Treinador *jogador) {
    FILE *arq = fopen(ARQUIVO_SAVE, "r");
    if (arq == NULL) {
        printf("\n[AVISO] Nenhum save encontrado em '%s'. Inicie um Novo Jogo.\n", ARQUIVO_SAVE);
        return 0;
    }

    Treinador tmp;              /* so copia para o jogador se TUDO der certo */
    memset(&tmp, 0, sizeof(tmp));
    char cabecalho[64];
    int ok = 1;

    if (!ler_linha(arq, cabecalho, sizeof(cabecalho)) || strcmp(cabecalho, CABECALHO_SAVE) != 0) {
        ok = 0;
    } else if (!ler_linha(arq, tmp.nome, sizeof(tmp.nome))) {
        ok = 0;
    } else if (fscanf(arq, "%d %d %d\n", &tmp.qtd_pokemon, &tmp.pokebolas, &tmp.pocoes) != 3
               || tmp.qtd_pokemon < 1 || tmp.qtd_pokemon > MAX_TIME
               || tmp.pokebolas < 0 || tmp.pocoes < 0) {
        ok = 0;
    }

    for (int i = 0; ok && i < tmp.qtd_pokemon; i++) {
        if (!ler_pokemon(arq, &tmp.time[i])) ok = 0;
    }

    fclose(arq);

    if (!ok) {
        printf("\n[ERRO] Arquivo de save corrompido ou de versao incompativel.\n");
        return 0;
    }

    *jogador = tmp;
    printf("\n[SUCESSO] Progresso de %s carregado!\n", jogador->nome);
    return 1;
}
