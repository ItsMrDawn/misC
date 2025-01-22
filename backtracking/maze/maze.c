#include <stdio.h>

void print_maze(int numl, int numc, char maze[numl][numc]) {

    int i, j;

    for (i = 0; i < numl; i++) {
        for (j = 0; j < numc; j++) {

            printf("%c ", maze[i][j]);

        }

        printf("\n");

    }

}

int walk(int x, int y, int endx, int endy, int numl, int numc, char maze[numl][numc]) {

    // reached the end 
	if ((x == endx) && (y == endy)) {
		return 1;
	}

	int i, j;

    // consider the 4 positions around the current x and y
	for (i=(x-1); i<=(x+1); i++) {
		for (j=(y-1); j<=(y+1); j++) {


            // diagonals are not tested
            if ( ((i==(x-1)) && (j==(y-1))) || ((i==(x-1)) && (j==(y+1))) || ((i==(x+1)) && (j==(y-1))) || ((i==(x+1)) && (j==(y+1))) ){
                continue;
            }

            // out of bounds
            if ((i < 0) || (j < 0) || (i >= numl) || (j >= numc)) {
                continue;
            }

			// test valid position (no walls or previous steps)
			if ((maze[i][j] == 'x') || (maze[i][j] == '.')) {
				continue;
			}

			// can be placed
            maze[i][j] = '.';
			
			// test new maze
			if (walk(i, j, endx, endy, numl, numc, maze)) {
				return 1;
			}

			// unsuccessful, remove the step
			maze[i][j] = 'o';
		}
	}

	return 0;

}

int main () {

    FILE *input;

    input = fopen("input.txt", "r");
    
    if ( input == NULL ){

        printf("Error opening file input.txt \n");
        return 1;

    }   

    // number of lines and columns
    int numl, numc;

    if (!fscanf(input, "%d %d", &numl, &numc)) {
        return 1;
    };

    // starting and ending positions
    int inix, iniy, endx, endy;

    if (!fscanf(input, "%d %d %d %d", &inix, &iniy, &endx, &endy)) {
        return 1;
    }

    // the maze itself
    char maze[numl][numc]; 

    int i, j;

    // read an \n
    fgetc(input);

    // read the maze
    for (i = 0; i < numl; i++) {
        for (j = 0; j < numc; j++) {

            maze[i][j] = fgetc(input);

            // the space between the characters
            fgetc(input);

        }
    }

    fclose(input);

    ///////////////////////////////////////////////////////////////////////////////////////

    // start the algorithm
    if (!walk(inix, iniy, endx, endy, numl, numc, maze)) {
        printf("A path could not be found.\n");
        return 0;
    } 

    print_maze(numl, numc, maze);

    return 0;

}