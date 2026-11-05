#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/arquivos.h"

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

    // Cabecalho e dados do treinador
    fprintf(arq, "# SAVE_POKEMON_RPG\n");
    fprintf(arq, "%s\n", jogador->nome);
    fprintf(arq, "%d %d %d\n", jogador->qtd_pokemon, jogador->pokebolas, jogador->pocoes);

    // Salva cada Pokemon do time
    for (int i = 0; i < jogador->qtd_pokemon; i++) {
        const Pokemon *p = &jogador->time[i];
        fprintf(arq, "%s\n%s\n", p->nome, p->tipo);
        fprintf(arq, "%d %d %d %d %d\n", p->nivel, p->vida_max, p->vida_atual, p->ataque, p->defesa);

        // Salva os 4 slots de ataques
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

int carregar_jogo(Treinador *jogador) {
    FILE *arq = fopen(ARQUIVO_SAVE, "r");
    if (arq == NULL) {
        printf("\n[AVISO] Nenhum save encontrado em '%s'. Inicie um Novo Jogo.\n", ARQUIVO_SAVE);
        return 0;
    }

    char cabecalho[64];
    if (!fgets(cabecalho, sizeof(cabecalho), arq) || strncmp(cabecalho, "# SAVE_POKEMON_RPG", 18) != 0) {
        printf("\n[ERRO] Arquivo de save corrompido ou incompativel.\n");
        fclose(arq);
        return 0;
    }

    memset(jogador, 0, sizeof(*jogador));

    if (!fgets(jogador->nome, sizeof(jogador->nome), arq)) {
        fclose(arq);
        return 0;
    }
    jogador->nome[strcspn(jogador->nome, "\r\n")] = '\0';

    if (fscanf(arq, "%d %d %d\n", &jogador->qtd_pokemon, &jogador->pokebolas, &jogador->pocoes) != 3) {
        fclose(arq);
        return 0;
    }

    for (int i = 0; i < jogador->qtd_pokemon; i++) {
        Pokemon *p = &jogador->time[i];

        if (!fgets(p->nome, sizeof(p->nome), arq)) break;
        p->nome[strcspn(p->nome, "\r\n")] = '\0';

        if (!fgets(p->tipo, sizeof(p->tipo), arq)) break;
        p->tipo[strcspn(p->tipo, "\r\n")] = '\0';

        fscanf(arq, "%d %d %d %d %d\n", &p->nivel, &p->vida_max, &p->vida_atual, &p->ataque, &p->defesa);

        for (int j = 0; j < MAX_ATAQUES; j++) {
            char nome_ataque[30];
            int poder;
            char tipo_ataque[15];

            if (!fgets(nome_ataque, sizeof(nome_ataque), arq)) break;
            nome_ataque[strcspn(nome_ataque, "\r\n")] = '\0';

            fscanf(arq, "%d %s\n", &poder, tipo_ataque);

            if (strcmp(nome_ataque, "VAZIO") != 0) {
                snprintf(p->ataques[j].nome, sizeof(p->ataques[j].nome), "%s", nome_ataque);
                p->ataques[j].poder = poder;
                snprintf(p->ataques[j].tipo, sizeof(p->ataques[j].tipo), "%s", tipo_ataque);
            }
        }
    }

    fclose(arq);
    printf("\n[SUCESSO] Progresso de %s carregado!\n", jogador->nome);
    return 1;
}
