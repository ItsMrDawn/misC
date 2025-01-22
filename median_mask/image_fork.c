#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/shm.h>

/*---------------------------------------------------------------------*/
#pragma pack(1) //bruxaria pra nao ter padding no struct

/*---------------------------------------------------------------------*/
struct cabecalho {
	unsigned short tipo;
	unsigned int   tamanho_arquivo;
	unsigned short reservado1;
	unsigned short reservado2;
	unsigned int   offset;
	unsigned int   tamanho_image_header;
			 int   largura;
			 int   altura;
	unsigned short planos;
	unsigned short bits_por_pixel;
	unsigned int   compressao;
	unsigned int   tamanho_imagem;
			 int   largura_resolucao;
			 int   altura_resolucao;
	unsigned int   numero_cores;
	unsigned int   cores_importantes;
}; 
typedef struct cabecalho CABECALHO;

struct rgb{
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};
typedef struct rgb RGB;



/*---------------------------------------------------------------------*/
void ordenavetor(unsigned char* vet, short tam){

	//tipo um selection idk

	register unsigned char menor;
	register short i, j, posmenor;

	for (i=0; i < tam; i++){
		
		menor = 255;
		posmenor = i;

		for (j=i; j < tam; j++){
			if (vet[j] < menor) {
				menor    = vet[j];
				posmenor = j;
			}
		}

		vet[posmenor] = vet[i];
		vet[i] = menor;

	}

}

void calcula_mediana_linha(RGB* imgout, RGB* imgin, int linha_ini, int numproc, int altura, int largura, int tammask) {

	int x, y, i, j, posvet;
	int borda = tammask / 2;
	int tamvet = tammask * tammask;

	RGB pixel;
	
	//vetores pra cada elemento dos pixel que tiverem na mascara
	//ja que a mediana é calculada separada pra cada um		
	unsigned char vetmaskr[tamvet], vetmaskg[tamvet], vetmaskb[tamvet];

	for(i = linha_ini; i < altura; i += numproc){
			
		for(j = 0; j < largura; j++){
			
			posvet = 0;

			//ler o quadrado da mascara em volta do pixel, usando uma "borda"
			//e.g. 5x5 vai de y-2 até y+2, mesma coisa pro x
			for (y = (i - borda); y <= (i + borda); y++) {
		
				if ((y < 0) || (y > altura)) {
					continue; //ja caiu fora do grid
				}

				for (x = (j - borda); x <= (j + borda); x++) {

					if ((x < 0) || (x > largura)){
						continue;
					}

					pixel = imgin[(y * largura) + x];

					vetmaskr[posvet] = pixel.red;	
					vetmaskg[posvet] = pixel.green;
					vetmaskb[posvet] = pixel.blue;

					posvet++;

				}

			}

			ordenavetor(vetmaskr, tamvet);
			ordenavetor(vetmaskg, tamvet);
			ordenavetor(vetmaskb, tamvet);

			//botar a mediana encontrada de volta no pixel

			pixel.red   = vetmaskr[tamvet / 2];
			pixel.green = vetmaskg[tamvet / 2];
			pixel.blue  = vetmaskb[tamvet / 2];

			imgout[(i * largura) + j] = pixel;

		}

	}

}


/*---------------------------------------------------------------------*/
int main(int argc, char **argv ){

	//validar os argumentos de entrada
	if (argc != 4) {
		printf("%s <nome do arquivo> <tamanho da mascara> <numero de processos>\n", argv[0]);
		exit(0);
	}

	char nomearq[100];
	strcpy(nomearq, argv[1]);

	int tammask = atoi(argv[2]);
	int numproc = atoi(argv[3]);

	CABECALHO cabecalho;
	int i, j;
	char aux[4];

	//======================= [leitura cabeçalho] ============================

	FILE *fin = fopen(nomearq, "rb");

	if ( fin == NULL ){
		printf("Erro ao abrir o arquivo %s\n", nomearq);
		exit(0);
	}  

	fread(&cabecalho, sizeof(CABECALHO), 1, fin);

	//printf("Tamanho da imagem: %u\n", cabecalho.tamanho_arquivo);
	//printf("Largura: %d\n", cabecalho.largura);
	//printf("Altura: %d\n", cabecalho.altura);
	//printf("Bits por pixel: %d\n", cabecalho.bits_por_pixel);


	//======================= [memória compartilhada] ============================

	int chave_in = 5;

	//vetorzão da imagem original intacta
	int shmid_in = shmget(chave_in, cabecalho.altura * cabecalho.largura * sizeof(RGB*), IPC_CREAT | 0600);

	RGB* imgin = shmat(shmid_in, 0, 0);

	
	int chave_out = 6;

	//mesma coisa pra imagem modificada
	int shmid_out = shmget(chave_out, cabecalho.altura * cabecalho.largura * sizeof(RGB*), IPC_CREAT | 0600);

	RGB* imgout = shmat(shmid_out, 0, 0);

	//======================= [leitura imagem] ============================

	//calcular alinhamento, podem sobrar bytes extras se a largura nao for multiplo de 4
	int ali = (cabecalho.largura * 3) % 4;
	if (ali != 0){
		ali = 4 - ali;
	}	

	int totalpixels = cabecalho.altura * cabecalho.largura;

	//ler a imagem propriamente

	if (ali == 0) {

		//se o alinhamento for 0 da pra escapar com ler toda a imagem de uma vez só
		fread(&imgin[0], sizeof(RGB), totalpixels, fin);

	} else {

		for(i = 0; i < totalpixels; i += cabecalho.largura){

			//fin = file in
			//ler a imagem linha por linha
			fread(&imgin[i], sizeof(RGB), cabecalho.largura, fin);
			
			//alinhamento no fim da linha, pixels sobrando
			fread(&aux, sizeof(unsigned char), ali, fin);
		}

	}


	fclose(fin);

	//======================= [criar processos] ============================

	int id, linhaini = 0;

	for (i = 1; i < numproc; i++) {

		id = fork();

		if (id == 0) {
			linhaini = i; // a linha inicial do filho (1, 2, 3...)
			break;        //é o filho, sai do laço
		}
	}

	//======================= [cálculo e mediana] ============================

	calcula_mediana_linha(imgout, imgin, linhaini, numproc, cabecalho.altura, cabecalho.largura, tammask);

	//======================= [liberar processos] ============================
	
	if (linhaini == 0) {

		//é o pai, espera os filhos
		for (i = 1; i < numproc; i++) {
			wait(NULL); // uma espera pra cada filho
		}

		//ponteiro da imagem de entrada
		shmdt(imgin);

		// liberar a memoria compartilhada dela tambem, não precisa mais
		shmctl(shmid_in, IPC_RMID, 0); 

	} else {
		// os filho, liberam seus ponteiros
		shmdt(imgin);
		shmdt(imgout);
	}

	//======================= [escrita] ============================

	//o nome do arquivo de saida vai ser o mesmo de entrada concatenado com "_out"

	//terminar a string antes do ".bmp"
	nomearq[strlen(nomearq) - 4] = '\0';

	strcat(nomearq, "_out.bmp");

	FILE *fout = fopen(nomearq, "wb");

	if ( fout == NULL ){
		printf("Erro ao abrir o arquivo %s\n", nomearq);
		exit(0);
	}  

	fwrite(&cabecalho, sizeof(CABECALHO), 1, fout);

	//hora de escrever os pixels

	if (ali == 0) {

		//sem alinhamento, vai tudo
		fwrite(&imgout[0], sizeof(RGB), totalpixels, fout);

	} else {

		//linha por linha
		for(i = 0; i < totalpixels; i += cabecalho.largura){
	
			fwrite(&imgout[i], sizeof(RGB), cabecalho.largura, fout);
			
			//alinhamento
			fwrite(&aux, sizeof(unsigned char), ali, fout);

		}

	}

	fclose(fout);

	//======================= [liberar memória] ============================

	shmdt(imgout);

	shmctl(shmid_out, IPC_RMID, 0); // liberar a memoria compartilhada


	return 0;
}
/*---------------------------------------------------------------------*/


