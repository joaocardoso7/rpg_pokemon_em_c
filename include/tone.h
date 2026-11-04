#ifndef TONE_H
#define TONE_H

#define NOTA_PAUSA 0
#define NOTA_C4 262
#define NOTA_D4 294
#define NOTA_E4 330
#define NOTA_F4 349
#define NOTA_G4 392
#define NOTA_A4 440
#define NOTA_B4 494
#define NOTA_C5 523
#define NOTA_D5 587
#define NOTA_E5 659
#define NOTA_F5 698
#define NOTA_G5 784
#define NOTA_A5 880
#define NOTA_C6 1047

void tocar_nota(int frequencia, int duracao);

void tocar_melodia(const int frequencias[], const int duracoes[], int quantidade);

void tocar_musica_tema(void);
void tocar_som_vitoria(void);
void tocar_som_captura(void);
void tocar_som_derrota(void);

#endif
