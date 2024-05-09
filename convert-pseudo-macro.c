#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_ARR_SIZE 100

typedef struct instruction_line Inst;
struct instruction_line{
    char** instruction;
};


char** parse_line(char* line) {

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

    return line_parsed;
 
    

    
}

char* parse_symbols(char** line){
    int len_str = strlen((*line));
    if((*line)[len_str - 1] == ':'){
        (*line)[len_str - 1] = '\0';
        return (*line);
    }
    else{
        return NULL;
    }
}

void parse_input_file(const char* filename, Inst** instructions) {
    FILE* file = fopen(filename, "r");
    FILE* symboltable = fopen("symboltable.txt", "w");
    ftruncate(fileno(symboltable), 0);

    int num_words = 0;
    int num_line = 0;
    int temp;
    char* output[10];
    char* label;
    fscanf(file, "%d", &num_words);

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        Inst *parsed_inst = (Inst *)malloc(sizeof(Inst));
        
        parsed_inst->instruction = parse_line(line);
        instructions[num_line] = parsed_inst;

        label = parse_symbols(parsed_inst->instruction);
        
        if(label != NULL){
            fprintf(symboltable, label);
            fprintf(symboltable, "  ");
            temp = num_line*4 + 4194304 - 8;
            sprintf(output, "%X\n", temp);
            fprintf(symboltable, "0x");
            fprintf(symboltable, output);
        }
        num_line++;
    }

    

    fclose(file);
}

int main(int argc, char* argv[]) {
    
    
}
