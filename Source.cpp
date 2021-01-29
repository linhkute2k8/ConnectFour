#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define N 6
#define M 7
#define MAX_DEPTH 7

const int SASUKE = 1;
const int NARUTO = -1;

int last_turn;
int winner;
int node_size;
int maximizing_player;

struct SpielStand {
	//board
	int ariSpielFeld[N + 1][M + 1];
	//check fill col
	int ariSeineInSpalte[M + 1];
	//current player
	int iPlayer;
	//cnt empty elements
	int iFreieFelder;
	SpielStand(){
		for (int i = 1; i <= N; i++) {
			ariSeineInSpalte[i] = 0;
			for (int j = 1; j <= M; j++) {
				ariSpielFeld[i][j] = 0;
			}
		}
	ariSeineInSpalte[7] = 0;
	iFreieFelder = N * M;
	iPlayer = SASUKE;
	}
}spielStands[50000];
SpielStand* mainBoard = spielStands;

void shuffle(int* array, int n) {
	for (int i = 0; i < n - 1; i++) {
		size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
		int t = array[j];
		array[j] = array[i];
		array[i] = t;
	}
}

void get_valid_move(int temp_board[N + 1][M + 1], int valid_move[10], int& valid_move_size) {
	valid_move_size = 0;

	for (int i = 0; i <= 3; i++) {
		if (temp_board[1][4 - i] == 0) {
			valid_move[valid_move_size++] = 4 - i;
		}
		if (i == 0) continue;
		if (temp_board[1][4 + i] == 0) {
			valid_move[valid_move_size++] = 4 + i;
		}
		shuffle(valid_move, valid_move_size);
	}
}

void show_value_at(int i, int j, int temp_board[N + 1][M + 1]) {
	if (temp_board[i][j] == SASUKE) printf("\xDB\xDB");
	else if (temp_board[i][j] == NARUTO) printf("\xB1\xB1");
	else printf("  ");
}

void show_board(int temp_board[N + 1][M + 1]) {
	//system("cls");
	printf("\nNARUTO : \xB1\xB1\nSASUKE : \xDB\xDB\n");
	printf("Last turn: %d\n", last_turn);
	//print number line
	for (int i = 1; i <= 7; i++) {
		printf(" %d ", i);
	}
	printf("\n");
	//draw first line (top)
	for (int i = 1; i <= 8; i++) {
		if (i == 1) printf("\xDA");
		else if (i == 8) printf("\xC4\xC4\xBF");
		else printf("\xC4\xC4\xC2");
	}
	printf("\n");

	//draw normal line (1,2,3,4,5) and value
	for (int i = 1; i < 6; i++) {
		for (int j = 1; j <= 8; j++) {
			printf("\xB3");
			if (i < 8) show_value_at(i, j, temp_board);
		}
		printf("\n");
		for (int j = 1; j <= 8; j++) {
			if (j == 1) printf("\xC3\xC4\xC4");
			else if (j == 8) printf("\xB4");
			else printf("\xC5\xC4\xC4");
		}
		printf("\n");
	}
	//draw last line (6) + value and bottom
	for (int i = 1; i <= 8; i++) {
		printf("\xB3");
		if (i < 8) show_value_at(6, i, temp_board);
	}
	printf("\n");
	for (int i = 1; i <= 8; i++) {
		if (i == 1) printf("\xC0");
		else if (i == 8) printf("\xC4\xC4\xD9");
		else printf("\xC4\xC4\xC1");
	}
}

//drop discs into the valid 'col'
int drop(int col, int player, int temp_board[N + 1][M + 1]) {
	//check valid col
	if (col > 7 || col < 1 || temp_board[1][col]) {
		printf("Invalid option.");
		return 0;
	}
	//find disc location
	int x, y;
	for (int i = 1; i <= 6; i++) {
		if (temp_board[i][col] == 0) {
			x = i, y = col;
		}
		else break;
	}
	//set value at the bottom
	temp_board[x][y] = player;
	last_turn = col;
	return col;
}
//count sequence by length
int count_sequence(int player, int length, int temp_board[N + 1][M + 1]) {
	int total_cnt = 0;
	//check horizontal sequence
	for (int i = 1; i <= 6; i++) {
		int cnt = 0;
		for (int j = 1; j <= 7; j++) {
			if (temp_board[i][j] != player) {
				cnt = 0;
				continue;
			}
			cnt++;
			//check maxtiefe horizontal
			if (length == 3 && cnt == 2) {
				int row = i, col = j;
				if ((j < 6 && temp_board[row][col + 1] == 0 && temp_board[row][col + 2] == player)
					|| (j > 3 && temp_board[row][col - 2] == 0 && temp_board[row][col - 3] == player))
					total_cnt++;
			}
			if (cnt == length) {
				//check maxtiefe horizontal
				if (cnt == 3) {
					int row = i, col = j;
					if (j < 7 && temp_board[row][col - 3] == 0 && temp_board[row][col + 1] == 0)
						total_cnt += 3;
				}
				total_cnt++;
				cnt = 0;
			}
		}
	}
	//check vertical sequence
	for (int j = 1; j <= 7; j++) {
		int cnt = 0;
		for (int i = 1; i <= 6; i++) {
			if (temp_board[i][j] != player) {
				cnt = 0;
				continue;
			}
			cnt++;
			if (cnt == length) {
				total_cnt++;
				cnt = 0;
			}
		}
	}
	//check negative diagonal sequence
	for (int i = 6; i >= 1; i--) {
		int cnt = 0;
		for (int j = 1; j <= 7; j++) {
			int row = i, col = j;
			int cnt = 0;
			while (true) {
				if (temp_board[row][col] == player)
					cnt++;
				else break;
				//check maxtiefe
				if (cnt == 2 && length == 3) {
					if (row > 2 && col > 2 && temp_board[row - 2][col - 2] == player)
						total_cnt++;
				}
				if (cnt == length) {
					//check maxtiefe
					if (row > 1 && row < 4 && col>1 && col < 5 && temp_board[row - 1][col - 1] && temp_board[row + 3][col + 3] == 0)
						total_cnt += 3;
					total_cnt++;
					cnt = 0;
				}
				row--, col--;
				if (row < 1 || col < 1) break;
			}
		}
	}
	//check positive diagonal sequence
	for (int i = 1; i <= 6; i++) {
		for (int j = 1; j <= 7; j++) {
			int row = i, col = j;
			int cnt = 0;
			while (true) {
				if (temp_board[row][col] == player)
					cnt++;
				else break;
				//check maxtiefe
				if (cnt == 2 && length == 3) {
					if (row < 5 && col > 2 && temp_board[row + 2][col - 2] == player)
						total_cnt++;
				}
				if (cnt == length) {
					//check maxtiefe
					if (row < 6 && row > 3 && col > 1 && col < 5 && temp_board[row + 1][col - 1] == 0 && temp_board[row - 3][col + 3] == 0)
						total_cnt += 3;
					total_cnt++;
					cnt = 0;
				}
				row++, col--;
				if (row > 6 || col < 1) break;
			}
		}
	}
	return total_cnt;
}

int max(int a, int b) { return a >= b ? a : b; }
int min(int a, int b) { return a <= b ? a : b; }

//copy array
void copy_node(int dst[N + 1][M + 1], int src[N + 1][M + 1]) {
	for (int i = 1; i <= 6; i++) {
		for (int j = 1; j <= 7; j++) {
			dst[i][j] = src[i][j];
		}
	}
}

bool is_valid_move(int col) {
	if (col < 1 || col > 7) return false;
	if (mainBoard->ariSpielFeld[1][col]) return false;
	return true;
}

bool is_draw(int node[7][8]) {
	for (int i = 1; i <= 7; i++) {
		if (node[1][i] == 0) return false;
	}
	return true;
}


void TogglePlayer(SpielStand* pstAktSpielstand) {
	pstAktSpielstand->iPlayer *= -1;
}
void UpdateSpielstand(SpielStand* pstAktSpielstand, int iSpalte) {
	drop(iSpalte, pstAktSpielstand->iPlayer, pstAktSpielstand->ariSpielFeld);

	pstAktSpielstand->ariSeineInSpalte[iSpalte]++;
	pstAktSpielstand->iFreieFelder--;

	TogglePlayer(pstAktSpielstand);
}
void InitSpielstand(const char* pszInit, SpielStand* pstAktSpielstand) {
	while (*pszInit) {
		UpdateSpielstand(pstAktSpielstand, *pszInit++ - 48);
	}
}

void PrintSpielstand(SpielStand* pstAktSpielstand) {
	show_board(pstAktSpielstand->ariSpielFeld);
	printf("\n%d Felder-aktueller Spieler: %d (", pstAktSpielstand->iFreieFelder, pstAktSpielstand->iPlayer);
	if (pstAktSpielstand->iPlayer == 1) {
		printf("\xDB\xDB)\n");
	}
	else
		printf("\xB1\xB1)\n");
}
int TesteSieg(SpielStand* pstAktSpielstand, int iSpalte) {
	int res			 = count_sequence(SASUKE, 4, pstAktSpielstand->ariSpielFeld);
	return res ? res : count_sequence(NARUTO, 4, pstAktSpielstand->ariSpielFeld);
}

struct ComputerZug {
	//best move
	int iColumn;
	//heuristic value
	int iGewicht;
};

int heuristic_value(SpielStand* node) {
	int bot_score = count_sequence(maximizing_player, 2, node->ariSpielFeld) * 100
				  + count_sequence(maximizing_player, 3, node->ariSpielFeld) * 1000;

	int player_score = count_sequence(-maximizing_player, 2, node->ariSpielFeld) * 100
					 + count_sequence(-maximizing_player, 3, node->ariSpielFeld) * 1000;
	return bot_score - player_score;
}

int negamax(SpielStand* node, int depth, int alpha, int beta, int color) {
	if (count_sequence(-maximizing_player, 4, node->ariSpielFeld)) return -(1 << 30) * color;
	if (count_sequence(maximizing_player, 4, node->ariSpielFeld)) return (1 << 30) * color;
	if (is_draw(node->ariSpielFeld)) return 0;
	if (depth == 0)
		return color * heuristic_value(node);

	int value = 0;
	int valid_move[10];
	int valid_move_size = 0;

	get_valid_move(node->ariSpielFeld, valid_move, valid_move_size);
	value = -(1 << 30);

	for (int i = 0; i < valid_move_size; i++) {
		int idx = node_size++;
		SpielStand* ss = spielStands + idx;
		ss->iPlayer = node->iPlayer;
		copy_node(ss->ariSpielFeld, node->ariSpielFeld);
		UpdateSpielstand(ss, valid_move[i]);

		value = max(value, -negamax(ss, depth - 1, -beta, -alpha, -color));
		alpha = max(alpha, value);
		if (alpha >= beta)
			break;
	}
	return value;
}

ComputerZug getBestMove(SpielStand* spielStand, int depth) {
	int valid_move[10];
	int valid_move_size = 0;
	int node[7][8];
	copy_node(node, spielStand->ariSpielFeld);
	get_valid_move(node, valid_move, valid_move_size);

	int scores[10];
	int best_score = -(1 << 30) - (1 << 29);
	int best_move = valid_move[0];

	depth += 7 - valid_move_size;

	for (int i = 0; i < valid_move_size; i++) {
		node_size = 1;
		int idx = node_size++;
		SpielStand* ss = spielStand + idx;
		ss->iPlayer = maximizing_player;
		copy_node(ss->ariSpielFeld, node);
		UpdateSpielstand(ss, valid_move[i]);

		int score = -negamax(ss, depth - 1, -(1 << 30), 1 << 30, -1);
		scores[i] = score;

		if (scores[i] > best_score) {
			best_score = score;
			best_move = valid_move[i];
		}
	}
	int best_moves[10];
	int best_move_size = 0;
	for (int i = 0; i < valid_move_size; i++) {
		if (scores[i] == best_score) {
			best_moves[best_move_size++] = valid_move[i];
		}
	}
	best_move = rand() % best_move_size;
	best_move = best_moves[best_move];
	printf("%3d\n", best_move);

	ComputerZug computerzug;
	computerzug.iColumn = best_move;
	computerzug.iGewicht = best_score;

	return computerzug;
}

ComputerZug Negamax(SpielStand* pstAktSpielstand, int iTiefe) {
	return getBestMove(pstAktSpielstand, iTiefe);
}

void initalize() {
	for (int i = 1; i <= 7; i++) {
		mainBoard->ariSeineInSpalte[i] = 0;
	}
	mainBoard->iPlayer = 1;
	winner = 0;
	mainBoard->iFreieFelder = 42;
	for (int i = 1; i <= 6; i++) {
		for (int j = 1; j <= 7; j++)
			mainBoard->ariSpielFeld[i][j] = 0;
	}
}

void AI_vs_AI() {
	initalize();
	InitSpielstand("54463633211232122133666", mainBoard);
	while (true) {
		maximizing_player = mainBoard->iPlayer;
		int move = getBestMove(mainBoard, 7).iColumn;

		UpdateSpielstand(mainBoard, move);
		printf("\n==========================================\n");
		PrintSpielstand(mainBoard);

		if (count_sequence(-mainBoard->iPlayer, 4, mainBoard->ariSpielFeld)) {
			winner = -mainBoard->iPlayer;
		}
		if (winner) {
			if (winner == SASUKE)
				printf("\nSASUKE WIN\n");
			else
				printf("\nNARUTO WIN\n");
			break;
		}
		if (is_draw(mainBoard->ariSpielFeld)) {
			printf("\nDRAW\n");
			break;
		}
	}
}
int test[10];
int main() {
	printf("%d", test[10]);
	int player;
	int depth;
	int c = 1;
	int mode = 1;
	while (c == 1) {
		printf("Do you want to show an Example?\n1. Yes\n2. No.\n");
		scanf("%d", &c);
		if (c == 1) {
			AI_vs_AI();
			continue;
		}
		printf("\nMode:\n1. 1 Player\n2. 2 Player\n");
		scanf("%d", &mode);

		initalize();
		if (mode == 1) {
			printf("\nPick level:\n1. Easy\n2. Normal\n3. Hard\n");
			scanf("%d", &c);
			depth = c + 2;
			printf("Do you want to play first?\n1. Yes\n2. No\n");
			scanf("%d", &c);
		}

		if (c == 1) player = NARUTO;
		else player = SASUKE;
		mainBoard->iPlayer = player;

		srand(time(NULL));
		maximizing_player = SASUKE;
		node_size = 0;
		mainBoard = spielStands + node_size++;

		PrintSpielstand(mainBoard);
		int col = 1;
		winner = 0;
		
		while (col) {
			printf("\nYour turn: ");

			if (player == maximizing_player && mode == 1)
				col = Negamax(mainBoard, depth).iColumn;
			else scanf("%d", &col);

			if (!is_valid_move(col)) {
				printf("Invalid move.");
				continue;
			}

			UpdateSpielstand(mainBoard, col);
			PrintSpielstand(mainBoard);

			if (is_draw(mainBoard->ariSpielFeld)) {
				printf("\nDRAW\n");
				break;
			}
			if (count_sequence(player, 4, mainBoard->ariSpielFeld)) {
				winner = player;
				break;
			}

			player *= -1;
		}
		if (winner == NARUTO) printf("\nNARUTO WIN\n");
		else if (winner == SASUKE) printf("\nSASUKE WIN\n");
		printf("\nPlay again?\n1. Yes\n2. No\n");
		scanf("%d", &c);
	}
}