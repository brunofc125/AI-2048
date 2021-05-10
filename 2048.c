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
#include "Tipos.h"
#include "redeNeural.c"
#include "Tabuleiro.c"

uint8_t scheme=0;

void getColor(uint8_t value, char *color, size_t length) {
	uint8_t original[] = {8,255,1,255,2,255,3,255,4,255,5,255,6,255,7,255,9,0,10,0,11,0,12,0,13,0,14,0,255,0,255,0};
	uint8_t blackwhite[] = {232,255,234,255,236,255,238,255,240,255,242,255,244,255,246,0,248,0,249,0,250,0,251,0,252,0,253,0,254,0,255,0};
	uint8_t bluered[] = {235,255,63,255,57,255,93,255,129,255,165,255,201,255,200,255,199,255,198,255,197,255,196,255,196,255,196,255,196,255,196,255};
	uint8_t *schemes[] = {original,blackwhite,bluered};
	uint8_t *background = schemes[scheme]+0;
	uint8_t *foreground = schemes[scheme]+1;
	if (value > 0) while (value--) {
		if (background+2<schemes[scheme]+sizeof(original)) {
			background+=2;
			foreground+=2;
		}
	}
	snprintf(color,length,"\033[38;5;%d;48;5;%dm",*foreground,*background);
}

void drawBoard(uint8_t board[SIZE][SIZE], int indTab) {
	uint8_t x,y;
	char color[40], reset[] = "\033[m";
	printf("\033[H");

	printf("2048.c %17d pts\n\n",scores[indTab]);

	for (y=0;y<SIZE;y++) {
		for (x=0;x<SIZE;x++) {
			getColor(board[x][y],color,40);
			printf("%s",color);
			printf("       ");
			printf("%s",reset);
		}
		printf("\n");
		for (x=0;x<SIZE;x++) {
			getColor(board[x][y],color,40);
			printf("%s",color);
			if (board[x][y]!=0) {
				char s[8];
				snprintf(s,8,"%u",(uint32_t)1<<board[x][y]);
				uint8_t t = 7-strlen(s);
				printf("%*s%s%*s",t-t/2,"",s,t/2,"");
			} else {
				printf("   ·   ");
			}
			printf("%s",reset);
		}
		printf("\n");
		for (x=0;x<SIZE;x++) {
			getColor(board[x][y],color,40);
			printf("%s",color);
			printf("       ");
			printf("%s",reset);
		}
		printf("\n");
	}
	printf("\n");
	printf("        ←,↑,→,↓ or q        \n");
	printf("\033[A"); // one line up
}

uint8_t findTarget(uint8_t array[SIZE],uint8_t x,uint8_t stop) {
	uint8_t t;
	// if the position is already on the first, don't evaluate
	if (x==0) {
		return x;
	}
	for(t=x-1;;t--) {
		if (array[t]!=0) {
			if (array[t]!=array[x]) {
				// merge is not possible, take next position
				return t+1;
			}
			return t;
		} else {
			// we should not slide further, return this one
			if (t==stop) {
				return t;
			}
		}
	}
	// we did not find a
	return x;
}

bool slideArray(uint8_t array[SIZE], int indTab) {
	bool success = false;
	uint8_t x,t,stop=0;

	for (x=0;x<SIZE;x++) {
		if (array[x]!=0) {
			t = findTarget(array,x,stop);
			// if target is not original position, then move or merge
			if (t!=x) {
				// if target is zero, this is a move
				if (array[t]==0) {
					array[t]=array[x];
				} else if (array[t]==array[x]) {
					// merge (increase power of two)
					array[t]++;
					// increase score
					scores[indTab]+=(uint32_t)1<<array[t];
					// set stop to avoid double merge
					stop = t+1;
				}
				array[x]=0;
				success = true;
			}
		}
	}
	return success;
}

void rotateBoard(uint8_t board[SIZE][SIZE]) {
	uint8_t i,j,n=SIZE;
	uint8_t tmp;
	for (i=0; i<n/2; i++) {
		for (j=i; j<n-i-1; j++) {
			tmp = board[i][j];
			board[i][j] = board[j][n-i-1];
			board[j][n-i-1] = board[n-i-1][n-j-1];
			board[n-i-1][n-j-1] = board[n-j-1][i];
			board[n-j-1][i] = tmp;
		}
	}
}

bool moveUp(uint8_t board[SIZE][SIZE], int indTab) {
	bool success = false;
	uint8_t x;
	for (x=0;x<SIZE;x++) {
		success |= slideArray(board[x], indTab);
	}
	return success;
}

bool moveLeft(uint8_t board[SIZE][SIZE], int indTab) {
	bool success;
	rotateBoard(board);
	success = moveUp(board, indTab);
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}

bool moveDown(uint8_t board[SIZE][SIZE], int indTab) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board, indTab);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}

bool moveRight(uint8_t board[SIZE][SIZE], int indTab) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board, indTab);
	rotateBoard(board);
	return success;
}

bool findPairDown(uint8_t board[SIZE][SIZE]) {
	bool success = false;
	uint8_t x,y;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE-1;y++) {
			if (board[x][y]==board[x][y+1]) return true;
		}
	}
	return success;
}

uint8_t countEmpty(uint8_t board[SIZE][SIZE]) {
	uint8_t x,y;
	uint8_t count=0;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			if (board[x][y]==0) {
				count++;
			}
		}
	}
	return count;
}

bool gameEnded(uint8_t board[SIZE][SIZE]) {
	bool ended = true;
	if (countEmpty(board)>0) return false;
	if (findPairDown(board)) return false;
	rotateBoard(board);
	if (findPairDown(board)) ended = false;
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	return ended;
}

void addRandom(uint8_t board[SIZE][SIZE]) {
	static bool initialized = false;
	uint8_t x,y;
	uint8_t r,len=0;
	uint8_t n,list[SIZE*SIZE][2];

	if (!initialized) {
		srand(time(NULL));
		initialized = true;
	}

	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			if (board[x][y]==0) {
				list[len][0]=x;
				list[len][1]=y;
				len++;
			}
		}
	}

	if (len>0) {
		r = rand()%len;
		x = list[r][0];
		y = list[r][1];
		n = (rand()%10)/9+1;
		board[x][y]=n;
	}
}

void initBoard(int indTab, bool draw) {
	uint8_t x,y;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			boards[indTab].matriz[x][y]=0;
		}
	}
	addRandom(boards[indTab].matriz);
	addRandom(boards[indTab].matriz);
	if(draw) 
		drawBoard(boards[indTab].matriz, indTab);
	scores[indTab] = 0;
}

/*void setBufferedInput(bool enable) {
	static bool enabled = true;
	static struct termios old;
	struct termios new;

	if (enable && !enabled) {
		// restore the former settings
		tcsetattr(STDIN_FILENO,TCSANOW,&old);
		// set the new state
		enabled = true;
	} else if (!enable && enabled) {
		// get the terminal settings for standard input
		tcgetattr(STDIN_FILENO,&new);
		// we want to keep the old setting to restore them at the end
		old = new;
		// disable canonical mode (buffered i/o) and local echo
		new.c_lflag &=(~ICANON & ~ECHO);
		// set the new settings immediately
		tcsetattr(STDIN_FILENO,TCSANOW,&new);
		// set the new state
		enabled = false;
	}
}*/

int test(int indTab) {
	uint8_t array[SIZE];
	// these are exponents with base 2 (1=2 2=4 3=8)
	uint8_t data[] = {
		0,0,0,1,	1,0,0,0,
		0,0,1,1,	2,0,0,0,
		0,1,0,1,	2,0,0,0,
		1,0,0,1,	2,0,0,0,
		1,0,1,0,	2,0,0,0,
		1,1,1,0,	2,1,0,0,
		1,0,1,1,	2,1,0,0,
		1,1,0,1,	2,1,0,0,
		1,1,1,1,	2,2,0,0,
		2,2,1,1,	3,2,0,0,
		1,1,2,2,	2,3,0,0,
		3,0,1,1,	3,2,0,0,
		2,0,1,1,	2,2,0,0
	};
	uint8_t *in,*out;
	uint8_t t,tests;
	uint8_t i;
	bool success = true;

	tests = (sizeof(data)/sizeof(data[0]))/(2*SIZE);
	for (t=0;t<tests;t++) {
		in = data+t*2*SIZE;
		out = in + SIZE;
		for (i=0;i<SIZE;i++) {
			array[i] = in[i];
		}
		slideArray(array, indTab);
		for (i=0;i<SIZE;i++) {
			if (array[i] != out[i]) {
				success = false;
			}
		}
		if (success==false) {
			for (i=0;i<SIZE;i++) {
				printf("%d ",in[i]);
			}
			printf("=> ");
			for (i=0;i<SIZE;i++) {
				printf("%d ",array[i]);
			}
			printf("expected ");
			for (i=0;i<SIZE;i++) {
				printf("%d ",in[i]);
			}
			printf("=> ");
			for (i=0;i<SIZE;i++) {
				printf("%d ",out[i]);
			}
			printf("\n");
			break;
		}
	}
	if (success) {
		printf("All %u tests executed successfully\n",tests);
	}
	return !success;
}

void signal_callback_handler(int signum) {
	printf("         TERMINATED         \n");
	//setBufferedInput(true);
	printf("\033[?25h\033[m");
	exit(signum);
}

uint8_t getHorizontalValues(uint8_t board[SIZE][SIZE]) {
	uint8_t horizontalValue = 0;
	for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE-1; j++) {
			if(board[i][j] != 0 && board[i][j] == board[i][j+1]) {
				horizontalValue++;
				j += 1;
			}
		}
	}
}

uint8_t getVerticalValues(uint8_t board[SIZE][SIZE]) {
	uint8_t verticalValue = 0;
	for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE-1; j++) {
			if(board[j][i] != 0 &&  board[j][i] == board[j+1][i]) {
				verticalValue++;
				j += 1;
			}
		}
	}
}

void bubble_sort(double vetor[], int ind[]) {
    int k, j, aux;

	ind[0] = 0;
	ind[1] = 1;
	ind[2] = 2;
	ind[3] = 3;

    for (k = 1; k < SIZE; k++) {
        for (j = 0; j < SIZE - 1; j++) {
            if (vetor[ind[j]] < vetor[ind[j + 1]]) {
                aux          = ind[j];
                ind[j]     = ind[j + 1];
                ind[j + 1] = aux;
            }
        }
    }

	/*printf("\n");
	for(k = 0; k < 4; k++) {
		printf("%.2f\t", vetor[k]);
	}
	
	printf("\n");
	for(k = 0; k < 4; k++) {
		printf("%d\t", ind[k]);
	}
	printf("\n");*/

}

int main(int argc, char *argv[]) {
	
	char c;
	bool success;
	double saida[4];
	double entrada[18];
	clock_t h;
    double tempo;
	int indiceSaida[4];
	bool draw = false;
	int auxTest = 0;
	int maiorValor = 0;
	srand(time(NULL));
	
	initVars();

	if (argc == 2 && strcmp(argv[1],"test")==0) {
		return test(0);
	}
	if (argc == 2 && strcmp(argv[1],"blackwhite")==0) {
		scheme = 1;
	}
	if (argc == 2 && strcmp(argv[1],"bluered")==0) {
		scheme = 2;
	}

	printf("\033[?25l\033[2J");

	// register signal handler for when ctrl-c is pressed
	signal(SIGINT, signal_callback_handler);

	//setBufferedInput(false);
	initBoard(0, draw);
	//boards[0].Cerebro = RNA_CarregarRede("n");
	boards[0].Cerebro = RNA_CriarRedeNeural(2, 18, 10, 4);
	boards[0].TamanhoDNA = RNA_QuantidadePesos(boards[0].Cerebro);
	boards[0].DNA = (double*)malloc(boards[0].TamanhoDNA*sizeof(double));
	DNASalvo[0] = (double*)malloc(boards[0].TamanhoDNA*sizeof(double));
	setRandPesos(boards[0].DNA, boards[0].TamanhoDNA);
	RNA_LerDNA(boards[0].DNA);
	memcpy(&DNASalvo[0], &boards[0].DNA, sizeof(boards[0].DNA));
	RNA_CopiarVetorParaCamadas(boards[0].Cerebro, DNASalvo[0]);

	for(int i = 1; i < POPULACAO_TAMANHO; i++) {
		initBoard(i, draw);
		boards[i].Cerebro = RNA_CriarRedeNeural(2, 18, 10, 4);
		boards[i].TamanhoDNA = boards[0].TamanhoDNA;
		boards[i].DNA = (double*)malloc(boards[i].TamanhoDNA*sizeof(double));
		DNASalvo[i] = (double*)malloc(boards[i].TamanhoDNA*sizeof(double));
	}

	h = clock();

	for(int tab = 0; tab < POPULACAO_TAMANHO; tab++) {
		while(true) {
			int k = 2;
			for(int i = 0; i < 4; i++) {
				for(int j = 0; j < 4; j++) {
					entrada[k] = boards[tab].matriz[i][j];//pow(2,boards[tab].matriz[i][j]);
					k++;
				}
			}

			entrada[0] = getHorizontalValues(boards[tab].matriz);
			entrada[1] = getVerticalValues(boards[tab].matriz);

			/*printf("\nentrada: ");
			for(int m = 0; m < 18; m++) {
				printf("%.2f\t", entrada[m]);
			}
			printf("\n");*/

			RNA_CopiarParaEntrada(boards[tab].Cerebro, entrada);
			RNA_CalcularSaida(boards[tab].Cerebro);
			RNA_CopiarDaSaida(boards[tab].Cerebro, saida);

			bubble_sort(saida, indiceSaida);

			for(int i = 0; i < SIZE && !success; i++) {
				switch (indiceSaida[i]) {
					case 0:
						success = moveLeft(boards[tab].matriz, tab);
						break;
					case 1:
						success = moveRight(boards[tab].matriz, tab);
						break;
					case 2:
						success = moveUp(boards[tab].matriz, tab);
						break;
					case 3:
						success = moveDown(boards[tab].matriz, tab);
						break;
					default:
						break;
				}
				
				if (success) {
					success = false;
					//drawBoard(boards[tab].matriz, tab);
					//usleep(800000);
					addRandom(boards[tab].matriz);
					//drawBoard(boards[tab].matriz, tab);
					
					if (gameEnded(boards[tab].matriz)) {
						//printf("         GAME OVER          \n");
						tempo = (double) (clock() - h) / CLOCKS_PER_SEC;
						if(tempo > 21600) {
							goto end;
						}
						maiorNumAtual[tab] = maiorNum(tab);
						if(maiorNumAtual[tab] > maiorValor) {
							maiorValor = maiorNumAtual[tab];
							//drawBoard(boards[tab].matriz, tab);
							RNA_SalvarDNA(boards[tab].DNA);
						}
						//printf("\033[A");
						//printf("\033[A");
						//printf("\nmaior atual: %d\tmaior salvo: %d\ttempo: %.2lf\n", maiorNumAtual[tab], maiorNumSalvo[tab], tempo);
						reiniciarBoard(tab);
						initBoard(tab, draw);
					}
				}
			}
		}
		end:;
		RNA_DestruirRedeNeural(boards[tab].Cerebro);
	}
	printf("\nmaior atual: %d\tmaior salvo: %d\ttempo: %.2lf\n", maiorNumAtual[0], maiorNumSalvo[0], tempo);

	return 0;
}
