#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

/*---------------------------------------------------------------------*/
struct args{
	int id;
	int nthr;
	double nr;
	double res;
};
typedef struct args ARGUMENTOS;

/*---------------------------------------------------------------------*/
void * calcula_pi( void * args ){

	ARGUMENTOS *p = (ARGUMENTOS *)args;

	int ini = p->id;
	int nthr = p->nthr;
	double nr = p->nr;
	
	double x, base, altura;
	double i, pi;

	base = 1.0 / nr; 
        pi = 0.0; 

       	for( i=ini ; i<=nr ;  i+=nthr ) { 
                x = base * (( double)i - 0.5); 
                altura = 4.0 / (1.0 + x*x); 
		pi += base * altura;
	}

	p->res = pi;
}
/*---------------------------------------------------------------------*/
int main(int argc, char **argv){

	double pi = 0;
	double nr;
	int i, nthr;
	pthread_t *tid = NULL;
	ARGUMENTOS *a = NULL;

	if ( argc != 3 ){
		printf("%s <num_retangulos> <nthreads> \n", argv[0]);
		exit(0);
	}

	nr = atof(argv[1]);
	nthr = atoi(argv[2]);

	tid = (pthread_t *)malloc(nthr * sizeof(pthread_t));
	a = (ARGUMENTOS *)malloc(nthr * sizeof(ARGUMENTOS));
	

	for( i=0; i<nthr; i++){
		a[i].id = i + 1;
		a[i].nthr = nthr;
		a[i].nr = nr;
		a[i].res = 0;
		pthread_create(&tid[i], NULL, calcula_pi, (void *)&a[i]);
	}

	for( i=0; i<nthr; i++){
		pthread_join(tid[i], NULL);
		pi += a[i].res;
	}
		

	printf("Pi = %.15f\n",pi);	
}
/*---------------------------------------------------------------------*/

