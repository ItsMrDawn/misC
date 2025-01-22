#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <mpi.h>

/*----------------------------------------------------------------------------*/
#define INTMAX 100
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

void inicializa_mat_vetB(int n, double *mat, double *vetB){
	
	register int i;
	
	memset(mat, 0, n * n * sizeof(double));

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

	}

}

void inicializa_vetX(int n, double *vetX){
	
	register int i;

	// preenche vetor X com 0
	for (i = 0; i < n; i++) {
		vetX[i] = 0;
	}

}

void multiplicacao_matriz_vetor (int n, int nproc, double *mat, double *vet, double *res){
	
	register int i, j;
	
	for(i = 0; i < (n / nproc); i++) {
		
		res[i] = 0;
		
		for(j = 0; j < n; j++) {
			res[i] += mat[(i * n) + j] * vet[j];
		}

	}		
}

void subtracao_vetor(int n, double* vet1, double* vet2, double* res){
	
	register int i;

	for(i = 0; i < n; i++) {
		res[i] = vet1[i] - vet2[i];
	}

}

void soma_vetor(int n, double* vet1, double* vet2, double* res){
	
	register int i;
	
	for(i = 0; i < n; i++) {
		res[i] = vet1[i] + vet2[i];
	}

}

double produto_escalar(int n, double* vet1, double* vet2){
	
	register int i;
	double prod = 0;
	
	for(i = 0; i < n; i++) {
		prod += vet1[i] * vet2[i];
	}

	return(prod);	
}

void escalar_vetor(int n, double escalar, double* vet, double* res){
	
	register int i;
	
	for(i = 0; i < n; i++) {
		res[i] = escalar * vet[i];
	}
}

/*----------------------------------------------------------------------------*/
int main(int argc, char **argv){

	double *matfull, *vetX, *vetB, *vetAuxsmol, *vetAuxfull, *vetR, *vetD, *vetQ, *matsmol;
	int n, id, nproc, i;
	int iteracao = 0;
	int stop = 0;
	double sigma_novo, alpha, sigma_velho, beta, inicio, fim;

	if (argc != 2)
	{
		printf("%s <ordem_da_matriz> \n", argv[0]);
		exit(0);
	} 

	n = atoi(argv[1]);

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);

	if ( id == 0 ){

		//o processo pai, tem a matriz final e os demais vetores

		matfull    = (double *)malloc(n * n * sizeof(double));
		vetAuxfull = (double *)malloc(n * sizeof(double));
	
		vetB = (double *)malloc(n * sizeof(double));
		vetR = (double *)malloc(n * sizeof(double));
		vetQ = (double *)malloc(n * sizeof(double));

		inicializa_mat_vetB(n, matfull, vetB);

		inicio = MPI_Wtime();	

	}

	//matrizes e vetores menores pra cada subprocesso
   	matsmol    = (double *) malloc ((n * n) / nproc * sizeof(double));
	vetAuxsmol = (double *) malloc ((n / nproc) * sizeof(double));

	vetD = (double *) malloc (n * sizeof(double));
	vetX = (double *) malloc (n * sizeof(double));

	inicializa_vetX(n, vetX);

	////////////////

	//distribuir a matriz
	MPI_Scatter(matfull, (n / nproc) * n, MPI_DOUBLE, matsmol, (n * n) / nproc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//a multiplicacao
	multiplicacao_matriz_vetor(n, nproc, matsmol, vetX, vetAuxsmol);

	//juntar os resultados
	MPI_Gather(vetAuxsmol, n / nproc, MPI_DOUBLE, vetAuxfull, n/nproc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	////////////////

	if ( id == 0 ) {

		subtracao_vetor(n, vetB, vetAuxfull, vetR);

		memcpy(vetD, vetR, n * sizeof(double));

		sigma_novo = produto_escalar(n, vetR, vetR);

	}

	while (!stop) {

		//distribuir os vetores
		MPI_Bcast(vetD, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		//a multiplicacao
		multiplicacao_matriz_vetor(n, nproc, matsmol, vetD, vetAuxsmol);

		//juntar os resultados no vetQ
		MPI_Gather(vetAuxsmol, n/nproc, MPI_DOUBLE, vetQ, n/nproc, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		//////////////////

		if (id != 0) {

			//filhos recebem o broadcast do pai informado se eh pra parar ou nao
			MPI_Bcast(&stop, 1, MPI_INT, 0, MPI_COMM_WORLD);			

			continue;

		};

		//processamento do proc pai
		{
		alpha = sigma_novo / produto_escalar(n, vetD, vetQ);

		escalar_vetor(n, alpha, vetD, vetAuxfull);

		soma_vetor(n, vetX, vetAuxfull, vetX);

		escalar_vetor(n, alpha, vetQ, vetAuxfull);

		subtracao_vetor(n, vetR, vetAuxfull, vetR);

		sigma_velho = sigma_novo;
		sigma_novo = produto_escalar(n, vetR, vetR);

		beta = sigma_novo / sigma_velho;

		escalar_vetor(n, beta, vetD, vetAuxfull);

		soma_vetor(n, vetR, vetAuxfull, vetD);

		iteracao++;
		}

		if (id == 0) {

			//ver se pode parar o loop
			if ((iteracao >= INTMAX) || (sigma_novo <= ERRO)) {
				stop = 1;
			}

			//printf("sigma = %f \n", sigma_novo);

			//pai faz um broadcast indicando que eh pra seguir
			MPI_Bcast(&stop, 1, MPI_INT, 0, MPI_COMM_WORLD);			

		};

	}

	MPI_Finalize();


	if (id == 0) {

		fim = MPI_Wtime();

		printf("Tempo %f | Numero de Iteracoes: %i\n", fim - inicio, iteracao);

		for (i = 0; i < n; i++ ) {
			printf("%f ", vetX[i]);
		}
		printf("\n");

		free(matfull);
		
		free(vetAuxfull);
		free(vetB);
		free(vetR);
		free(vetQ);

	}

	free(vetAuxsmol);
	free(matsmol);
	free(vetX);
	free(vetD);

	return 0;

}
/*----------------------------------------------------------------------------*/
