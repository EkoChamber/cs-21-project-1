#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_ARR_SIZE 100

char** split_by_space(char*);
char** split_by_comma(char*);

typedef struct instruction_line Inst;
struct instruction_line {
    char* type;
    char* address;
    char* label;
    char* mnemonic;
    char** operands;
};

char** split_by_parentheses(char* line) {
    char** splitString = (char**)malloc(5 * sizeof(char*));
    char* token = strtok(line, "(");

    int i = 0;
    while (token != NULL && i < 5) {
        splitString[i] = strdup(token);

        token = strtok(NULL, "(");
        i++;
    }
    // splitString[i] = NULL; // NULL terminate the array
    return splitString;
}

char** split_by_space(char* line) {
    char** splitString = (char**)malloc(5 * sizeof(char*));
    char* token = strtok(line, " ()");

    int i = 0;
    while (token != NULL && i < 5) {
        splitString[i] = strdup(token);

        token = strtok(NULL, " ()");
        i++;
    }
    // splitString[i] = NULL; // NULL terminate the array
    return splitString;
}

char** split_by_comma(char* line) {
    char** splitString = (char**)malloc(3 * sizeof(char*));
    char* token = strtok(line, ",");
    
    int i = 0;
    while (token != NULL && i < 3) {
        splitString[i] = strdup(token);

        token = strtok(NULL, ",");
        i++;
    }
    splitString[i] = NULL; // NULL terminate the array
    return splitString;
}


char** check_macro_pseudo(char* str){
    char** return_str;

    if(!strcmp(str, "print_str")){
        // printf("%s\n", str);
        return_str = (char**)malloc(4 * sizeof(char*));
        return_str[0] = "li $v0,4";
        return_str[1] = "li $a0,<parameter>";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else if(!strcmp(str, "read_str")){
        // printf("%s\n", str);
        return_str = (char**)malloc(3* sizeof(char*));
        return_str[0] = "li $v0,8";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else if(!strcmp(str, "print_integer")){
        // printf("%s\n", str);
        return_str = (char**)malloc(4 * sizeof(char*));
        return_str[0] = "li $v0,1";
        return_str[1] = "li $a0,<parameter>";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else if(!strcmp(str, "read_integer")){
        // printf("%s\n", str);
        return_str = (char**)malloc(3* sizeof(char*));
        return_str[0] = "li $v0,8";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else if(!strcmp(str, "exit")){
        // printf("%s\n", str);
        return_str = (char**)malloc(3* sizeof(char*));
        return_str[0] = "li $v0,8";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else{
        return NULL;
    }

}

void parse_input_file(const char* filename, Inst** instructions) {
    FILE* file = fopen(filename, "r");

    int num_lines = 0;
    fscanf(file, "%d", &num_lines);

    char line[MAX_LINE_LENGTH];
    char **lines = (char**)malloc(num_lines * sizeof(char*));
    char **macros_pseudo = (char**)malloc(MAX_ARR_SIZE * sizeof(char*));
    char **to_parse = (char**)malloc(MAX_ARR_SIZE * sizeof(char*));

    int i=-1;
    while (fgets(line, sizeof(line), file)) {
        int len_pseudo = 0;

        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';

        // printf("%s\n", line);
        if(i == -1){
            i++;
            continue;
        }

        lines[i] = strdup(line);


        // macros_pseudo = check_macro_pseudo(split_by_space(lines[i])[0]);
        // if(macros_pseudo != NULL){
        //     while(macros_pseudo[len_pseudo] != '\0'){
        //         to_parse[len_pseudo] = macros_pseudo[len_pseudo];
        //         printf("%s", to_parse[len_pseudo]);
        //         len_pseudo++;
        //     }
        // }
        // else{
            to_parse[0] = lines[i];
        // }

        for(int a = 0; a <= len_pseudo; a++){
            char** parsedBySpace = (char**)malloc(num_lines * sizeof(char*));
            parsedBySpace = split_by_space(to_parse[len_pseudo]); 

            instructions[i] = (Inst*)malloc(sizeof(Inst));

            // label
            if (strchr(parsedBySpace[0], ':') != NULL) {
                int len_str = strlen(parsedBySpace[0]);
                (parsedBySpace[0])[len_str - 1] = '\0';
                instructions[i]->label = strdup(parsedBySpace[0]);

                instructions[i]->mnemonic = strdup(parsedBySpace[1]);
            }
            else {
                instructions[i]->label = "0";

            // mnemonic
                if (strchr(parsedBySpace[0], '.') != NULL) {
                    instructions[i]->mnemonic = "0";
                }
                else
                    instructions[i]->mnemonic = strdup(parsedBySpace[0]);
            }

            // type
            if ((strcmp(instructions[i]->mnemonic, "add") == 0) || (strcmp(instructions[i]->mnemonic, "sub") == 0) ||
                (strcmp(instructions[i]->mnemonic, "and") == 0) || (strcmp(instructions[i]->mnemonic, "or") == 0) ||
                (strcmp(instructions[i]->mnemonic, "slt") == 0) || (strcmp(instructions[i]->mnemonic, "move") == 0)) {
                instructions[i]->type = "R";
            }
            else if ((strcmp(instructions[i]->mnemonic, "addi") == 0) || (strcmp(instructions[i]->mnemonic, "addiu") == 0)) {
                instructions[i]->type = "I";
            }
            else if ((strcmp(instructions[i]->mnemonic, "beq") == 0) || (strcmp(instructions[i]->mnemonic, "bne") == 0)) {
                instructions[i]->type = "branch";
            }
            else if ((strcmp(instructions[i]->mnemonic, "lw") == 0) || (strcmp(instructions[i]->mnemonic, "sw") == 0)) {
                instructions[i]->type = "transfer";
            }
            else if (strcmp(instructions[i]->mnemonic, "jr") == 0) {
                instructions[i]->type = "jr";
            }
            else if ((strcmp(instructions[i]->mnemonic, "j") == 0) || (strcmp(instructions[i]->mnemonic, "jal") == 0)) {
                instructions[i]->type = "J";
            }
            else {
                instructions[i]->type = "0";
            }

            for (int k = 0; k < 1; k++) {
                if(parsedBySpace[k] != NULL){
                    free(parsedBySpace[k]);
                }
            }
            free(parsedBySpace);
        }
        i++;
    }

    fclose(file);
}

void symbol_table(int num_lines, Inst** instructions) {
    FILE* symboltable = fopen("symboltable.txt", "w");
    ftruncate(fileno(symboltable), 0);

    char* output[10];

    for (int i=0; i<num_lines; i++) {
        instructions[i]->address = (i+1)*4 + 4194304 - 8;
        
        if (strcmp(instructions[i]->label, "0") != 0) {
            fprintf(symboltable, instructions[i]->label);
            fprintf(symboltable, "\t");
            sprintf(output, "%X\n", instructions[i]->address);
            fprintf(symboltable, "0x");
            fprintf(symboltable, output);
        }
    }
}


int main(int argc, char* argv[]) {
    FILE* file = fopen("mips.txt", "r");

    int num_lines = 0;
    fscanf(file, "%d", &num_lines);
    
    Inst** instructions;

    instructions = calloc(MAX_ARR_SIZE, MAX_LINE_LENGTH);
    
    parse_input_file("mips.txt", instructions);
    symbol_table(num_lines, instructions);

    printf("LABEL\tTYPE\tMNEMONIC ADDRESS\n");
    for (int i=0; i<num_lines; i++) {
        printf("%s\t%s\t%s\t%X\n", instructions[i]->label, instructions[i]->type, instructions[i]->mnemonic, instructions[i]->address);
    }

    free(instructions);
    return 0;
}
