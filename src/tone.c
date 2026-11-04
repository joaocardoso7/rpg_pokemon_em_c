#ifdef _WIN32
    #include <windows.h>
#else
    #define _POSIX_C_SOURCE 199309L
    #include <time.h>
#endif

#include "../include/tone.h"

#define TEMPO 150

void tocar_nota(int frequencia, int duracao) {
    if (duracao <= 0) return;

#ifdef _WIN32
    if (frequencia >= 37 && frequencia <= 32767) {
        Beep((DWORD)frequencia, (DWORD)duracao);
    } else {
        Sleep((DWORD)duracao);
    }
#else
    (void)frequencia;
    struct timespec t;
    t.tv_sec = duracao / 1000;
    t.tv_nsec = (long)(duracao % 1000) * 1000000L;
    nanosleep(&t, NULL);
#endif
}

void tocar_melodia(const int frequencias[], const int duracoes[], int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        tocar_nota(frequencias[i], duracoes[i]);
    }
}

#define QTD(v) ((int)(sizeof(v) / sizeof(v[0])))

void tocar_musica_tema(void) {
    static const int notas[] = {
        NOTA_C5, NOTA_E5, NOTA_G5, NOTA_C6, NOTA_PAUSA, NOTA_G5, NOTA_C6,
        NOTA_A5, NOTA_F5, NOTA_A5, NOTA_G5, NOTA_E5, NOTA_D5, NOTA_C5
    };
    static const int duracoes[] = {
        TEMPO, TEMPO, TEMPO, TEMPO * 2, TEMPO / 2, TEMPO, TEMPO * 3,
        TEMPO, TEMPO, TEMPO, TEMPO * 2, TEMPO, TEMPO, TEMPO * 4
    };
    tocar_melodia(notas, duracoes, QTD(notas));
}

void tocar_som_vitoria(void) {
    static const int notas[]    = {NOTA_G4, NOTA_C5, NOTA_E5, NOTA_G5, NOTA_E5, NOTA_G5};
    static const int duracoes[] = {TEMPO, TEMPO, TEMPO, TEMPO * 2, TEMPO, TEMPO * 4};
    tocar_melodia(notas, duracoes, QTD(notas));
}

void tocar_som_captura(void) {
    static const int notas[]    = {NOTA_E5, NOTA_PAUSA, NOTA_E5, NOTA_PAUSA, NOTA_E5, NOTA_C6};
    static const int duracoes[] = {TEMPO, TEMPO, TEMPO, TEMPO, TEMPO, TEMPO * 4};
    tocar_melodia(notas, duracoes, QTD(notas));
}

void tocar_som_derrota(void) {
    static const int notas[]    = {NOTA_G4, NOTA_F4, NOTA_E4, NOTA_D4, NOTA_C4};
    static const int duracoes[] = {TEMPO * 2, TEMPO * 2, TEMPO * 2, TEMPO * 2, TEMPO * 5};
    tocar_melodia(notas, duracoes, QTD(notas));
}

