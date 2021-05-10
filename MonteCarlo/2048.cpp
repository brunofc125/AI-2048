#include "MonteCarlo.cpp"

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

void drawBoard(Board &board) {
	uint8_t x,y;
	char color[40], reset[] = "\033[m";
	printf("\033[H");

	printf("2048.c %17d pts\n\n",board.score);

	for (y=0;y<SIZE;y++) {
		for (x=0;x<SIZE;x++) {
			getColor(board.matriz[x][y],color,40);
			printf("%s",color);
			printf("       ");
			printf("%s",reset);
		}
		printf("\n");
		for (x=0;x<SIZE;x++) {
			getColor(board.matriz[x][y],color,40);
			printf("%s",color);
			if (board.matriz[x][y]!=0) {
				char s[8];
				snprintf(s,8,"%u",(uint32_t)1<<board.matriz[x][y]);
				uint8_t t = 7-strlen(s);
				printf("%*s%s%*s",t-t/2,"",s,t/2,"");
			} else {
				printf("   ·   ");
			}
			printf("%s",reset);
		}
		printf("\n");
		for (x=0;x<SIZE;x++) {
			getColor(board.matriz[x][y],color,40);
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

bool slideArray(uint8_t array[SIZE], Board &board) {
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
					board.score+=(uint32_t)1<<array[t];
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

void rotateBoard(Board &board) {
	uint8_t i,j,n=SIZE;
	uint8_t tmp;
	for (i=0; i<n/2; i++) {
		for (j=i; j<n-i-1; j++) {
			tmp = board.matriz[i][j];
			board.matriz[i][j] = board.matriz[j][n-i-1];
			board.matriz[j][n-i-1] = board.matriz[n-i-1][n-j-1];
			board.matriz[n-i-1][n-j-1] = board.matriz[n-j-1][i];
			board.matriz[n-j-1][i] = tmp;
		}
	}
}

bool moveUp(Board &board) {
	bool success = false;
	uint8_t x;
	for (x=0;x<SIZE;x++) {
		success |= slideArray(board.matriz[x], board);
	}
	return success;
}

bool moveLeft(Board &board) {
	bool success;
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}

bool moveDown(Board &board) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	rotateBoard(board);
	return success;
}

bool moveRight(Board &board) {
	bool success;
	rotateBoard(board);
	rotateBoard(board);
	rotateBoard(board);
	success = moveUp(board);
	rotateBoard(board);
	return success;
}

bool findPairDown(Board &board) {
	bool success = false;
	uint8_t x,y;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE-1;y++) {
			if (board.matriz[x][y]==board.matriz[x][y+1]) return true;
		}
	}
	return success;
}

uint8_t countEmpty(Board &board) {
	uint8_t x,y;
	uint8_t count=0;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			if (board.matriz[x][y]==0) {
				count++;
			}
		}
	}
	return count;
}

bool gameEnded(Board &board) {
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

void addRandom(Board &board) {
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
			if (board.matriz[x][y]==0) {
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
		board.matriz[x][y]=n;
	}
}

void initBoard(Board &board, bool draw) {
	uint8_t x,y;
	for (x=0;x<SIZE;x++) {
		for (y=0;y<SIZE;y++) {
			board.matriz[x][y]=0;
		}
	}
	addRandom(board);
	addRandom(board);
	if(draw) 
		drawBoard(board);
	board.score = 0;
}

void setBufferedInput(bool enable) {
	static bool enabled = true;
	static struct termios old;
	struct termios anew;

	if (enable && !enabled) {
		// restore the former settings
		tcsetattr(STDIN_FILENO,TCSANOW,&old);
		// set the new state
		enabled = true;
	} else if (!enable && enabled) {
		// get the terminal settings for standard input
		tcgetattr(STDIN_FILENO,&anew);
		// we want to keep the old setting to restore them at the end
		old = anew;
		// disable canonical mode (buffered i/o) and local echo
		anew.c_lflag &=(~ICANON & ~ECHO);
		// set the new settings immediately
		tcsetattr(STDIN_FILENO,TCSANOW,&anew);
		// set the new state
		enabled = false;
	}
}

void signal_callback_handler(int signum) {
	printf("         TERMINATED         \n");
	setBufferedInput(true);
	printf("\033[?25h\033[m");
	exit(signum);
}

int main(int argc, char *argv[]) {
	Board board;
	char c;
	bool success;
	bool draw = true;

	if (argc == 2 && strcmp(argv[1],"blackwhite")==0) {
		scheme = 1;
	}
	if (argc == 2 && strcmp(argv[1],"bluered")==0) {
		scheme = 2;
	}
	if (argc == 2 && strcmp(argv[1],"montecarlo")==0) {
		goto monteCarlo;
	}

	printf("\033[?25l\033[2J");

	// register signal handler for when ctrl-c is pressed
	signal(SIGINT, signal_callback_handler);

	initBoard(board, draw);
	setBufferedInput(false);
	while (true) {
		c=getchar();
		if (c == -1){
			puts("\nError! Cannot read keyboard input!");
			break;
		}
		switch(c) {
			case 97:	// 'a' key
			case 104:	// 'h' key
			case 68:	// left arrow
				success = moveLeft(board);  break;
			case 100:	// 'd' key
			case 108:	// 'l' key
			case 67:	// right arrow
				success = moveRight(board); break;
			case 119:	// 'w' key
			case 107:	// 'k' key
			case 65:	// up arrow
				success = moveUp(board);    break;
			case 115:	// 's' key
			case 106:	// 'j' key
			case 66:	// down arrow
				success = moveDown(board);  break;
			default: success = false;
		}
		if (success) {
			drawBoard(board);
			usleep(150000);
			addRandom(board);
			drawBoard(board);
			if (gameEnded(board)) {
				printf("         GAME OVER          \n");
				break;
			}
		}
		if (c=='q') {
			printf("        QUIT? (y/n)         \n");
			c=getchar();
			if (c=='y') {
				break;
			}
			drawBoard(board);
		}
		if (c=='r') {
			printf("       RESTART? (y/n)       \n");
			c=getchar();
			if (c=='y') {
				initBoard(board, draw);
			}
			drawBoard(board);
		}
	}
	setBufferedInput(true);

	printf("\033[?25h\033[m");

	return EXIT_SUCCESS;

	monteCarlo:;
	draw = false;
	int movimento, primeiroMovimento;
	bool primeiroMov;
	initBoard(board, draw);
	while(true) {
		cloneBoard(board);
		memset(&score, 0, sizeof(score));
		memset(&qtdMov, 0, sizeof(qtdMov));
		for(int i = 0; i < QTD_CLONES; i++) {
			primeiroMov = true;
			while(true) {
				movimento = rand()%4;
				switch(movimento) {
					case 0:
						success = moveDown(clones[i]);
						break;
					case 1:
						success = moveLeft(clones[i]);
						break;
					case 2:
						success = moveRight(clones[i]);
						break;
					case 3:
						success = moveUp(clones[i]);
						break;
					default:
						break;
				}
				if(success) {
					success = false;
					addRandom(clones[i]);
					if(primeiroMov) {
						primeiroMov = false;
						primeiroMovimento = movimento;
						qtdMov[primeiroMovimento]++;
					}
					if (gameEnded(clones[i])) {
						score[primeiroMovimento]+=clones[i].score;
						goto prox;
					}
				}
			}
			prox:;
		}
		proxMovimento = 0;
		for(int i = 0; i< SIZE; i++) {
			mediaPorMov[i] = qtdMov[i] != 0 ? ((double)score[i])/((double)qtdMov[i]) : 0;
			if(mediaPorMov[proxMovimento] < mediaPorMov[i]) {
				proxMovimento = i;
			}
		}

		switch(proxMovimento) {
			case 0:
				success = moveDown(board);
				break;
			case 1:
				success = moveLeft(board);
				break;
			case 2:
				success = moveRight(board);
				break;
			case 3:
				success = moveUp(board);
				break;
			default:
				break;
		}
		if(success) {
			success = false;
			drawBoard(board);
			//usleep(80000);
			addRandom(board);
			drawBoard(board);
			if (gameEnded(board)) {
				goto finish;
			}
		}
	}
	
	finish:;
	return EXIT_SUCCESS;
}