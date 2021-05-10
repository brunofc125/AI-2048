#include <math.h>
#define POPULACAO_TAMANHO 1
#define SIZE 4
#define QTD_CLONES 1000

double* DNASalvo[POPULACAO_TAMANHO];
uint32_t scoreSalvo[POPULACAO_TAMANHO];
uint32_t scores[POPULACAO_TAMANHO];
int maiorNumSalvo[POPULACAO_TAMANHO];
int maiorNumAtual[POPULACAO_TAMANHO];
Board boards[POPULACAO_TAMANHO];

double getRandomValue()
{
    return (rand()%20001/10.0) - 1000.0;
    //return (rand()%201/10.0) - 10.0;
    //return (rand()%2001/1000.0) - 1.0;
    //return (rand()%2001/10000.0) - 0.1;

    //return rand()%3 - 1;
}

int maiorNum(int indTab) {
    int max = 0;
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            if(boards[indTab].matriz[i][j] > max) {
                max = boards[indTab].matriz[i][j];
            }
        }
    }
    return max;
}

void setRandPesos(double *vetor, int n) {
    for(int i = 0; i < n; i++) {
        vetor[i] = rand()%200-100;
    }
}

void reiniciarDNA(int indTab) {
    int TamanhoDNA = boards[indTab].TamanhoDNA;

    scoreSalvo[indTab] = scores[indTab];
    maiorNumSalvo[indTab] = maiorNumAtual[indTab];

    for(int i=0; i<TamanhoDNA; i++)
    {
        DNASalvo[indTab][i] = boards[indTab].DNA[i];
        if(rand()%10 >=3) {
            boards[indTab].DNA[i] *= ((rand()%100 + 1)/100.0);
        } else {
            boards[indTab].DNA[i] = rand()%200-100;
        }
    }
    RNA_CopiarVetorParaCamadas(boards[indTab].Cerebro, boards[indTab].DNA);
}

void reiniciarBoard(int indTab) {
    if(maiorNumSalvo[indTab] < maiorNumAtual[indTab] || (maiorNumSalvo[indTab] == maiorNumAtual[indTab] && scoreSalvo[indTab] < scores[indTab])) {
        reiniciarDNA(indTab);
    } else {
        scores[indTab] = scoreSalvo[indTab];
        maiorNumAtual[indTab] = maiorNumSalvo[indTab];
        memcpy(&boards[indTab].DNA, &DNASalvo[indTab], sizeof(&DNASalvo[indTab]));
        /*for(int i = 0; i < 10; i++) {
            printf("%.2f\t",boards[indTab].DNA[i]);
        }
        printf("\n");
        for(int i = 0; i < 10; i++) {
            printf("%.2f\t",DNASalvo[indTab][i]);
        }*/
        reiniciarDNA(indTab);
    }
}

void melhorConfig(int indTab) {
    scores[indTab] = scoreSalvo[indTab];
    maiorNumAtual[indTab] = maiorNumSalvo[indTab];
    memcpy(&boards[indTab].DNA, &DNASalvo[indTab], sizeof(&DNASalvo[indTab]));
}

void initVars() {
    memset(scoreSalvo, (uint32_t)0, sizeof(scoreSalvo));
    memset(maiorNumSalvo, (uint32_t)0, sizeof(maiorNumSalvo));
    memset(maiorNumAtual, (uint32_t)0, sizeof(maiorNumAtual));
}

