#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>

#define buffer(y, x) buffer[y * nx + x]

int max_iter = 255;

//*******************************************************************************//

int calcula_ponto(double ci, double cr) {
  int iteracoes = 0;
  double zi = 0;
  double zr = 0;

  while ((zr * zr + zi * zi < 4) && (iteracoes < max_iter)) {
    double nr, ni;

    /* Z <-- Z^2 + C */

    nr = zr * zr - zi * zi + cr;
    ni = 2 * zr * zi + ci;

    zi = ni;
    zr = nr;

    iteracoes++;
  }
  return iteracoes;
}

//*******************************************************************************//

void calcula_madelbrot(int linhaini, int numproc, int *buffer, int nx, int ny,
                       double xmin, double xmax, double ymin, double ymax) {
  double delta_x, delta_y;
  double valor_x, valor_y;
  int x, y;

  delta_x = (xmax - xmin) / nx;
  delta_y = (ymax - ymin) / ny;

  for (y = linhaini; y < ny; y += numproc) {
    valor_y = ymin + delta_y * y;
    for (x = 0; x < nx; x++) {
      valor_x = xmin + delta_x * x;
      buffer(y, x) = calcula_ponto(valor_x, valor_y);
    }
  }
}

//*******************************************************************************//
void gera_arquvio_ppm(char *nome_arquivo, int *buffer, int nx, int ny,
                      int max) {
  int i;

  FILE *file = fopen(nome_arquivo, "w");

  fprintf(file, "P2\n");
  fprintf(file, "%d %d\n", nx, ny);

  fprintf(file, "%d", max);

  for (i = 0; i < nx * ny; i++) {
    if (!(i % nx)) {
      fprintf(file, "\n");
    }
    fprintf(file, "%d ", buffer[i]);
  }
  fclose(file);
}

//*******************************************************************************//
int main(int argc, char **argv) {
  int nx = 10000;
  int ny = 10000;

  double xmin = -1.5;
  double ymin = -1.5;
  double xmax = 1.5;
  double ymax = 1.5;

  if (argc != 2) {
    printf("%s <nproc>\n", argv[0]);
    exit(0);
  }

  // criar memoria compartilhada
  int chave = 5;

  int shmid = shmget(chave, nx * ny * sizeof(int), IPC_CREAT | 0600);

  int *buffer = shmat(shmid, 0, 0);

  // criar processos filhos
  int numproc = atoi(argv[1]);

  int i, id, linhaini;

  linhaini = 0;

  for (i = 1; i < numproc; i++) {

    id = fork();

    if (id == 0) {
      linhaini = i; // a linha inicial do filho (1, 2, 3...)
      break;        //é o filho, sai do laço
    }
  }

  calcula_madelbrot(linhaini, numproc, buffer, nx, ny, xmin, xmax, ymin, ymax);

  if (linhaini == 0) {
    //é o pai, espera os filhos
    for (i = 1; i < numproc; i++) {
      wait(NULL); // uma espera pra cada filho
    }

    //gera_arquvio_ppm("saida.ppm", buffer, nx, ny, 255);

    shmdt(buffer);

    shmctl(shmid, IPC_RMID, 0); // liberar a memoria compartilhada

  } else {
    // os filho, liberam seu ponteiro
    shmdt(buffer);
  }

  // free(buffer);
}
//*******************************************************************************//
