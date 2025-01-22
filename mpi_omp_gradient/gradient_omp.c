#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>

/*----------------------------------------------------------------------------*/
#define INTMAX 10000000
#define ERRO 0.00000001

/*----------------------------------------------------------------------------*/
double* lermat(int* n, char nomearq[20]) {

	FILE* arq = fopen(nomearq, "r");

	char linha[100];
	int cont, x, y;
	register int i; 
	double val;

	linha[0] = '%';

	//pular os comentarios iniciais comecados por %
	while (linha[0] == '%') {
		fgets(linha, 100, arq);
	}


	// ler a altura/largura da matriz (n) e o numero de elementos
	sscanf(linha, "%d %d %d", n, n, &cont);

	//alocar uma matriz zerada n*n
	double (*mat)[*n] = calloc(sizeof(*mat), *n);

	for (i = 0; i < cont; i++) {

		fscanf(arq, "%d %d %lf", &x, &y, &val);

		mat[x-1][y-1] = val;

	}

	return mat;

}

/*----------------------------------------------------------------------------*/
void inicializa(int n, double *mat, double *vetX, double *vetB){
	
	register int i;
	
	memset(mat,0, n * n* sizeof(double));

	for (i = 0; i < n; i++) {

		//matriz, diagonal principal
		mat[i * n + i] = 4;

        if (i+1 < n) {
            mat[(i * n) + (i + 1)] = 1;
            mat[(i + 1) *  n + i ] = 1;
        }
         
        if (i+4 < n) {
        
            mat[(i * n) + (i + 4)] = 0.5;
            mat[(i + 4) *  n + i ] = 0.5;
        }

		//vetor B
		vetB[i] = 1;

		//vetor X
		vetX[i] = 0;

	}
}

/*----------------------------------------------------------------------------*/
void multiplicacao_matriz_vetor (int n, double *mat, double *vet, double *res){
	
	int i, j;
	
	#pragma omp parallel for private(i, j)
	for(i = 0; i < n; i++) {
		
		res[i] = 0;
		
		for(j = 0; j < n; j++) {
			res[i] += mat[(i * n) + j] * vet[j];
		}

	}		
}

/*----------------------------------------------------------------------------*/
void subtracao_vetor(int n, double* vet1, double* vet2, double* res){
	
	register int i;

	for(i=0;i<n;i++) {
		res[i] = vet1[i] - vet2[i];
	}

}

/*----------------------------------------------------------------------------*/
void soma_vetor(int n, double* vet1, double* vet2, double* res){
	
	register int i;
	
	for(i = 0; i < n; i++) {
		res[i] = vet1[i] + vet2[i];
	}

}

/*----------------------------------------------------------------------------*/
double produto_escalar(int n, double* vet1, double* vet2){
	
	register int i;
	double prod = 0;
	
	for(i = 0; i < n; i++) {
		prod += vet1[i] * vet2[i];
	}

	return(prod);	
}

/*----------------------------------------------------------------------------*/
void escalar_vetor(int n, double escalar, double* vet, double* res){
	
	register int i;
	
	for(i = 0; i < n; i++) {
		res[i] = escalar * vet[i];
	}
}

/*----------------------------------------------------------------------------*/
int main(int argc, char **argv){

	double *mat, *vetX, *vetB, *vetAux, *vetR, *vetD, *vetQ;
	int n, nth, i;
	int iteracao = 0;
	double sigma_novo, alpha, sigma_velho, beta;
	double inicio, fim;

 	if (argc != 3)
	{
		printf("%s <ordem_da_matriz> <num_threads>\n", argv[0]);
		exit(0);
	} 

	n = atoi(argv[1]);
	nth = atoi(argv[2]);

	omp_set_num_threads(nth);

	mat =  (double *)malloc(n * n * sizeof(double));
	vetX = (double *)malloc(n * sizeof(double));
	vetB = (double *)malloc(n * sizeof(double));
	vetAux = (double *)malloc(n * sizeof(double));
	vetR = (double *)malloc(n * sizeof(double));
	vetD = (double *)malloc(n * sizeof(double));
	vetQ = (double *)malloc(n * sizeof(double));

	inicializa(n, mat, vetX, vetB);

	inicio = omp_get_wtime();

	multiplicacao_matriz_vetor(n, mat, vetX, vetAux);

	subtracao_vetor(n, vetB, vetAux, vetR);

	memcpy(vetD, vetR, n * sizeof(double));

	sigma_novo = produto_escalar(n, vetR, vetR);	

	while ((iteracao < INTMAX) && (sigma_novo > ERRO))
	{

		multiplicacao_matriz_vetor(n, mat, vetD, vetQ);

		alpha = sigma_novo / produto_escalar(n, vetD, vetQ);

		escalar_vetor(n, alpha, vetD, vetAux);

		soma_vetor(n, vetX, vetAux, vetX);

		escalar_vetor(n, alpha, vetQ, vetAux);

		subtracao_vetor(n, vetR, vetAux, vetR);

		sigma_velho = sigma_novo;
		sigma_novo = produto_escalar(n, vetR, vetR);

		beta = sigma_novo / sigma_velho;

		escalar_vetor(n, beta, vetD, vetAux);

		soma_vetor(n, vetR, vetAux, vetD);

		iteracao++;

		// printf("sigma = %f \n", sigma_novo);
	}

	fim = omp_get_wtime();

	printf("Tempo %f  | Numero de Iteracoes: %i\n", fim - inicio, iteracao);

	for (i = 0; i < n; i++ ) {
		printf("%f ", vetX[i]);
	}
	printf("\n");	

	free(mat);
	free(vetX);
	free(vetB);
	free(vetAux);
	free(vetR);
	free(vetD);
	free(vetQ);
}
/*----------------------------------------------------------------------------*/
