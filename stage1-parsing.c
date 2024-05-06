#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256



int convertToHex(int num){

}

int parse_line(char* line) {

    char** line_parsed = NULL;
    line_parsed = (char **)malloc(sizeof(char *));
    char* temp;
    int ctr = 0;
    int len_str = 0;


    temp = strtok(line, " ,");

    while (line != NULL) {
        // printf("%s\n", line);

        line_parsed[ctr] = (char *)malloc(strlen(line) + 1);
        strcpy(line_parsed[ctr], line);

        ctr++;
        line_parsed = (char **)realloc(line_parsed, (ctr + 1) * sizeof(char *));


        line = strtok(NULL, " ,");
    }

    
    len_str = strlen(line_parsed[0]);
    if(line_parsed[0][len_str - 1] == ':' ){
        printf("%s  ", line_parsed[0]);
        return 1;
    }
    else{
        return 0;
    }
    
    

    
}

void parse_input_file(const char* filename) {
    FILE* file = fopen(filename, "r");

    int num_words = 0;
    int num_line = 0;
    fscanf(file, "%d", &num_words);

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        
        if(parse_line(line)){
            printf("0x%x", num_line + 4194304 - 8);
            printf("\n");
        }
        num_line += 4;
    }

    

    fclose(file);
}

int main(int argc, char* argv[]) {
    
    
    
    parse_input_file("input-file-labels.txt");


    return 0;
}
