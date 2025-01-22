#include <stdio.h>

int is_attacked(int x, int y, int size, int board[size][size]) {

	int i;

	for (i=0; i<size; i++) {

		//line
		if (board[x][i] == 1) {
			return 1;
		}
		
		//column
		if (board[i][y] == 1) {
			return 1;
		}

		//diagonals
		if (((x-i) >= 0) && ((y-i) >= 0))
		{
			if (board[x-i][y-i] == 1) {
				return 1;
			}
		}	

		if (((x-i) >= 0) && ((y+i) <= (size-1)))
		{
			if (board[x-i][y+i] == 1) {
				return 1;
			}
		}

		if (((x+i) <= (size-1)) && ((y-i) >= 0))
		{
			if (board[x+i][y-i] == 1) {
				return 1;
			}	
		}

		if (((x+i) <= (size-1)) && ((y+i) <= (size-1)))
		{
			if (board[x+i][y+i] == 1) {
				return 1;
			}				
		}		

	}

	return 0;

}

int n_queens(int N, int size, int board[size][size]) {

	if (N == 0) {
		return 1;
	}

	int i, j;

	for (i=0; i<size; i++) {
		for (j=0; j<size; j++) {

			//test position
			if (is_attacked(i, j, size, board)) {
				continue;
			}

			//can be placed
			board[i][j] = 1;
			
			//test new board state
			if (n_queens(N-1, size, board)) {
				return 1;
			}

			//unsuccessful, backtrack
			board[i][j] = 0;
		}
	}

	return 0;


}

int main(){
	int N = 8;
	int size = N;
	// scanf("%d", &N);  
	
	int i, j;
	
	int board[size][size];
	
	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {	
			board[i][j] = 0;
		}
	}

	if (!n_queens(N, size, board)) {
		return 0;
	}


	for (i=0; i<N; i++) {
		for (j=0; j<N; j++) {	
			printf("%d ", board[i][j]); 
		}

		printf("\n"); 

	}
	
}
