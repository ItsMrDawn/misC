# misC

This repository contains a collection of small projects implemented in C. The projects demonstrate various algorithms and techniques, including backtracking, binary file search, multithreading, and more.

### Backtracking - Maze solver
Implements a maze-solving algorithm using backtracking which finds a path from the start to the end of the maze.

1. The program reads the maze dimensions, starting position, and ending position from the `input.txt` file.
2. It then reads the maze itself, which is represented by a grid of characters.
3. The program uses a backtracking algorithm to find a path from the starting position to the ending position.
4. If a path is found, it prints the maze with the path marked. If no path is found, it prints a message indicating that a path could not be found.

The `input.txt` file should have the following format:
<numl> <numc> 
<inix> <iniy> <endx> <endy> 
<maze>

- `<numl>`: Number of lines in the maze.
- `<numc>`: Number of columns in the maze.
- `<inix>`: Starting position x-coordinate.
- `<iniy>`: Starting position y-coordinate.
- `<endx>`: Ending position x-coordinate.
- `<endy>`: Ending position y-coordinate.
- `<maze>`: Each column and row of the maze, where 'x' represents a wall and 'o' represents an open space.

An example input file is provided.

### Backtracking - N Queens
Solves the N Queens problem by placing N chess queens on an N×N chessboard so that no two queens threaten each other. The `queens2.c` file goes through all 91 possibilities and times the execution.

### Binary File Search
Implements a binary file search algorithm using either hashing or indexing to quickly locate records in a generated binary file. A `text.txt` file should be present in the same directory with each line containing an entry in the following format: `<key> <name> <age> <salary>` 

### Multithreaded Image Modifier
Applies a median mask to BMP image files using multithreading. This is implemented using either `fork` or POSIX `pthreads`, with a different file for each. The input arguments are `./images_pthread <file name> <mask size> <number of threads>`.

### Multithreaded Conjugate Gradient Solver
Solves systems of linear equations using the Conjugate Gradient method. This project supports multithreading using either MPI or OpenMP, with a different file for each. The input argument is the matrix size.

### Text File Encryptor/Decryptor
Implements a simple text file encryptor/decryptor using the Vigenère cipher for basic text encryption and decryption. An `input.txt` file should be present in the same directory.

### Mandelbrot Fractal Generator
Generates images of the Mandelbrot fractal set using mathematical calculations to determine the color of each pixel, outputting an image in the PPM format. The input arguments are `./mandelbrot <number of threads>`.

### Multithreaded Pi Calculator
Calculates the value of Pi using the rectangle area method and multithreading to speed up the computation process. The input arguments are `./pi_pthread <number of rectangles> <number of threads>`.

## Compilation and execution
Since these are simple, single-file programs with no external dependencies other than the POSIX `pthreads` library, the usual should suffice:
```sh
gcc program.c -o program
./program <arguments, if any>
 ```
The exception, of course, being the MPI/OpenMP files:
```sh
mpicc -o gradient_mpi gradient_mpi.c
mpirun -mca opal_warn_on_missing_libcuda 0 -use-hwthread-cpus -np <number of processes> gradient_mpi <matrix size>
 ```

```sh
gcc -o gradient_omp gradient_omp.c -fopenmp
./gradient_omp <matrix size>
 ```



