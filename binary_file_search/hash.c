#include <stdio.h>

#define FILENAME "binaryhash.bin"
#define DIVHASH 9

struct entry {
    
	unsigned int   key;
    char           name[50];
    unsigned short age;
    unsigned int   salary;

    //each of these must be 60 bytes long
	
}; 
typedef struct entry ENTRY;

int create_empty_file() {

    //create an empty binary.bin file

    FILE *bin;

    //empty memory of the size of a record (60 bytes)
    char* empty = (calloc(sizeof(ENTRY), sizeof(char)));

    bin = fopen(FILENAME, "wb");

    //<DIVHASH> buckets with 3 records each
    for(int i=0; i < (DIVHASH*3); i++){
        fwrite(empty, sizeof(ENTRY), 1, bin);
    }

    fclose(bin);

    free(empty);


}

int convert() {

    create_empty_file();

    //convert the records from the file text.txt to binary.bin

    FILE *bin;
    FILE *txt;

    ENTRY ent;

    int hash, i;
    unsigned int filekey;

    txt = fopen("text.txt", "r");
    
    if ( txt == NULL ){

        printf("Error opening file text.txt\n");
        return 1;

    }

    //rb+ for rewriting specific parts
    bin = fopen(FILENAME, "rb+");

    while (fscanf(txt, "%u %s %hu %u", &ent.key, &ent.name, &ent.age, &ent.salary) != EOF) {
        
        //remainder of DIVHASH
        hash = ent.key % DIVHASH;

        //go to the position where it should be written
        fseek(bin, 3 * hash * sizeof(ENTRY), SEEK_SET);

        //try to write in the 3 positions of the bucket
        for (i = 0; i < 3; i++) {
            fread(&filekey, sizeof(unsigned int), 1, bin);

            //empty space
            if (filekey == 0) {
                //"rewind" the key that's been read
                fseek(bin, -sizeof(unsigned int), SEEK_CUR);
                break; 
            }

            //advance to the position of the next key
            fseek(bin, sizeof(ENTRY) - sizeof(unsigned int), SEEK_CUR);
        }

        //couldn't write in the bucket, go to the overflow at the end of the file
        if (i == 3) {
            fseek(bin, 0, SEEK_END);
        }
        
        fwrite(&ent, sizeof(ENTRY), 1, bin);  
         
    }

    fclose(txt);
    fclose(bin);

    return 0;

}

int main() {

    FILE *bin_file;

    bin_file = fopen(FILENAME, "rb");

    if ( bin_file == NULL ){
        
        //if the binary file hasn't been created yet, convert from a "text.txt" file
        if (convert()) {
            return 0;
        }

        bin_file = fopen(FILENAME, "rb");

        if ( bin_file == NULL ){
            printf("Error opening file binaryhash.bin\n");
            return 0;
        }

	}  

    //////////////////

    //read key value
    
    int key, hash, i;
    printf("Enter an identifier key to be searched: ");
    scanf("%d", &key);

    hash = key % DIVHASH;

    ///////////////////

    ENTRY ent;

    //go to the position calculated by the hash
    fseek(bin_file, 3* hash * sizeof(ENTRY), SEEK_SET); //SEEK_SET goes to the beginning of the file

    for (i = 0; i < 3; i++) {

        fread(&ent, sizeof(ENTRY), 1, bin_file);

        if (ent.key == 0) {
            printf("Not found.\n");   
            break;
        }

        printf("Current entry: %u - %s \n", ent.key, ent.name);

        if (ent.key == key) {
            printf("Found!\n");
            break;
        }

    }

    if (i == 3) {
        printf("Not found.\n");
    }

    fclose(bin_file);

    return 0;

};