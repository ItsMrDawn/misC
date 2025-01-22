#include <stdio.h>
#include <stdlib.h>

struct entry {
    
	unsigned int   key;
    char           name[50];
    unsigned short age;
    unsigned int   salary;

    //each of these must be 60 bytes long
	
}; 
typedef struct entry ENTRY;

struct index {

    unsigned int key;
    long int     pos;


};
typedef struct index INDEX;

void sort_index(INDEX* idx, short size){

	//basic selection sort

    INDEX aux;
    register unsigned int smallest;
	register short i, j, pos_smallest;

	for (i=0; i < size; i++){
		
		smallest = __UINT32_MAX__;
		pos_smallest = i;

		for (j=i; j < size; j++){
			if ((INDEX*)idx[j].key < smallest) {
				smallest    = (INDEX*)idx[j].key;
				pos_smallest = j;
			}
		}

        aux = idx[pos_smallest];
		idx[pos_smallest] = idx[i];
		idx[i] = aux;

	}

}

int create_index() {

    //read the file binary.bin and create an ordered index

    FILE *file;

    file = fopen("binary.bin", "rb");    

    //calculate file size and number of records
    long int tamreg = sizeof(ENTRY);

    fseek(file, 0L, SEEK_END); //SEEK_END goes to the end of the file
    int num_ent = ftell(file) / tamreg; //ftell == current position

    //go back to the beginning of file
    rewind(file);

    //allocate an array for all elements of the index
    INDEX* idxmem = malloc(num_ent * sizeof(INDEX));
    INDEX ind;

    ENTRY reg;
    int i = 0;

    //read the records and build an index in memory, in the array
    for (i = 0; i < num_ent; i++) {
    
        fread(&reg, tamreg, 1, file);

        ind.key = reg.key;
        ind.pos   = ftell(file) - tamreg;

        idxmem[i] = ind;
     
    }

    fclose(file);

    sort_index(idxmem, num_ent);

    //write file "index.idx"
    file = fopen("index.idx", "wb");    
    fwrite(idxmem, num_ent * sizeof(INDEX), 1, file); 
    fclose(file);

    free(idxmem);

    return 0;  


}

int convert() {

    //convert the records from the file text.txt to binary.bin

    FILE *bin;
    FILE *txt;

    ENTRY reg;

    txt = fopen("text.txt", "r");
    
    if ( txt == NULL ){

        printf("Error opening file text.txt\n");
        return 1;

    }

    bin = fopen("binary.bin", "wb");

    while (fscanf(txt, "%u %s %hu %u", &reg.key, &reg.name, &reg.age, &reg.salary) != EOF) {
        fwrite(&reg, sizeof(ENTRY), 1, bin);   
    }

    fclose(txt);
    fclose(bin);

    create_index();

    return 0;

}

int main() {
 
    FILE *file;
    FILE *idx;

    file = fopen("binary.bin", "rb");

    if ( file == NULL ){
        
        //if the binary file hasn't been created yet, convert from a "text.txt" file
        if (convert()) {
            return 0;
        }

        file = fopen("binary.bin", "rb");

        if ( file == NULL ){
            printf("Error opening file binary.bin\n");
            return 0;
        }

	}  

    //index file, also with a fixed name "index.idx"
    idx = fopen("index.idx", "rb");

    if ( idx == NULL ){
        
        //this one is created along with the binary file
        printf("Erro ao abrir o arquivo index.idx\n");
        return 0;

	}      


    //get key value
    int key;
    printf("Enter an identifier key to be searched: ");
    scanf("%d", &key);

    //////////////////

    //calculate index size, to use in the for loop

    fseek(idx, 0L, SEEK_END); //SEEK_END goes to the end of file
    int num_ent = ftell(idx) / sizeof(INDEX); //total number of entries in the file   
    rewind(idx); 

    ///////////////////

    //search for the key in the index file

    int i;
    INDEX ind;

    for (i = 0; i < num_ent; i++) {

        fread(&ind, sizeof(INDEX), 1, idx);

        //printf("Current entry key in the index: %u\n", ind.key);

        if (ind.key == key) {
            printf("Found the key in the index. Will now read the binary file in position %d\n", ind.pos);
            break;
        }

    }

    if (i == num_ent) {
        printf("Key not found in index.\n");
    } else {
        ENTRY reg;
        fseek(file, ind.pos, 0);
        fread(&reg, sizeof(ENTRY), 1, file);
        printf("\nName: %s \nAge: %hu \nSalary: %u \n", reg.name, reg.age, reg.salary);

    }

    fclose(file);
    fclose(idx);

    return 0;

};