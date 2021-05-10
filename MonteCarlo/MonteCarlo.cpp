#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>

#define SIZE 4
#define QTD_CLONES 1000

uint32_t score[SIZE];
uint32_t qtdMov[SIZE];
double mediaPorMov[SIZE];
int proxMovimento;
uint8_t scheme=0;

typedef struct board {
    uint8_t matriz[SIZE][SIZE];
    uint32_t score;
    int primeiroMovimento;
} Board;

Board clones[QTD_CLONES];

void cloneBoard(Board &board) {
    for(int i = 0; i < QTD_CLONES; i++) {
        memcpy(&clones[i], &board, sizeof(board));
    }
}
