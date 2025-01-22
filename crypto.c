#include <stdio.h>
#include <string.h>

int cripto (int encrypt) {

    FILE *input;
    FILE *output;

    if (encrypt) {

        input = fopen("input.txt", "r");
        
        if (input == NULL) {

            printf("Error opening the file input.txt \n");
            return 1;

        }   

        output = fopen("encrypted.txt", "w");

        printf("Enter a key to encrypt the file input.txt\n");

    } else {

        input = fopen("encrypted.txt", "r");
        
        if (input == NULL) {

            printf("Error opening the file encrypted.txt \n");
            return 1;

        }   

        output = fopen("output.txt", "w");

        printf("Enter a key to decrypt the file encrypted.txt\n");

    }

    char key[20];

    scanf(" %s", &key);

    char ch;
    int i = 0;
    int len = strlen(key);

    while (1) {

        // read a character
        ch = fgetc(input);

        if (feof(input)) {
            break;
        }

        // shift the character value according to the key
        if (encrypt) {
            ch += key[i++];
        } else {
            ch -= key[i++];
        }
        
        // write the character to the output file
        fputc(ch, output);

        if (i == len) {
            i = 0;
        }

    }

    fclose(input);
    fclose(output);

    return 0;

}

int main () {

    char option = '\0';

    while (option != '3') {

        printf("\nChoose an option:\n");

        printf("1) Encrypt file input.txt\n");
        printf("2) Decrypt file encrypted.txt\n");
        printf("3) Exit\n\n");

        scanf(" %c", &option);

        switch (option) {
            case '1': cripto(1); break;
            case '2': cripto(0); break;
        }
    
    }

}