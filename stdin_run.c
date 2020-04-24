#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void print_word(char delimiter) {

    char* palabra;
    size_t total_length = 0;
    int found_delimiter = 0;
    char c;

    while(!found_delimiter) {

        char static_buffer[32] = "";
        size_t len = 0;

        for (int i = 0; ((i<32) && (!found_delimiter)); i++) {
            c = fgetc(stdin);
            if (c != delimiter) {
                static_buffer[i] = c;
                len++;
            } else {
                found_delimiter = 1;
            }
        }

        if (total_length == 0) {
            palabra = malloc(sizeof(char)*(len+1));
            strncpy(palabra, static_buffer, len);
            total_length+= len;
        } else {
            char* temp = malloc(sizeof(char) * total_length);
            strncpy(temp, palabra, total_length);
            free(palabra);
            palabra = malloc(sizeof(char)*total_length + sizeof(char)*(len+1));
            strncpy(palabra, temp, total_length);
            strncpy(palabra+total_length, static_buffer, len);
            free(temp);
            total_length+= len;
        }
    }
    
    palabra[total_length] = '\0';
    printf("La palabra es: %s (%ld bytes)\n", palabra, total_length);
    free(palabra);
}


int main() {
    print_word(' ');
    print_word(' ');
    print_word(' ');
    print_word('(');
    print_word(')');
    return 0;
}