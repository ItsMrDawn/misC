#include <stdio.h>
#include <sys/time.h>

int cont = 0;

int is_safe(int x, int y, int size, int board[size][size]) {
    int i, j;

    // column
    for (i = 0; i < x; i++) {
        if (board[i][y] == 1) {
            return 0;
        }
    }

    // upper left diagonal
    for (i = x, j = y; i >= 0 && j >= 0; i--, j--) {
        if (board[i][j] == 1) {
            return 0;
        }
    }

    // upper right diagonal
    for (i = x, j = y; i >= 0 && j < size; i--, j++) {
        if (board[i][j] == 1) {
            return 0;
        }
    }

    // position is safe
    return 1;
}

void print_board(int size, int board[size][size]) {
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            printf("%d ", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int solve_n_queens(int x, int size, int board[size][size]) {
    if (x == size) {
        // all queens have been placed, print the solution
        print_board(size, board);
        printf("%d \n", cont++);
        return 1;
    }

    int y, done = 0;
    for (y = 0; y < size; y++) {
        if (is_safe(x, y, size, board)) {
            // place a queen in the current position
            board[x][y] = 1;

            // move to the next row and try to place the next queen
            done = solve_n_queens(x + 1, size, board) || done;

            // backtrack: undo the move
            board[x][y] = 0;
        }
    }

    return done;
}

int main() {
    int size = 8;
    int board[size][size];
    int i, j;

    struct timeval st, et;

    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            board[i][j] = 0;
        }
    }

    gettimeofday(&st,NULL);

    solve_n_queens(0, size, board);

    gettimeofday(&et,NULL);

    int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
    printf("\nTime elapsed: %d microseconds\n", elapsed);

    return 0;
}
