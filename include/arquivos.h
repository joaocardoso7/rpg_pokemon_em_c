#ifndef ARQUIVOS_H
#define ARQUIVOS_H
 
#include "pokemon.h"
 
#define ARQUIVO_SAVE "savegame.txt"
 
int salvar_jogo(const Treinador *jogador);
int carregar_jogo(Treinador *jogador);
int verificar_ou_criar_save(const char *caminho);
 
#endif
 
