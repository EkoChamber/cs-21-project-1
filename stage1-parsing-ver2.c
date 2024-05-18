#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>   

#define MAX_LINE_LENGTH 256
#define MAX_ARR_SIZE 100

char** split(char*);
int final_total_lines = 0;
char *data_array[MAX_ARR_SIZE][3];

char* concat_str(const char* str1, const char* str2);

typedef struct instruction_line Inst;
struct instruction_line {
    char* type;
    char* address;
    char* label;
    char* mnemonic;
    char** operands;
};


char* concat_str(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char *result = (char*)malloc(len1 + len2 + 1);  // Allocate memory for the concatenated string

    if (result) {
        strcpy(result, str1);   // Copy the first string
        strcat(result, str2);   // Concatenate the second string
    }

    return result;  // Return the concatenated string
}


char** split(char* line) {
    char** splitString = (char**)malloc(5 * sizeof(char*));
    char* token = strtok(line, " ,()");

    int i = 0;
    while (token != NULL && i < 5) {
        splitString[i] = strdup(token);

        token = strtok(NULL, " ,()");
        i++;
    }
    // splitString[i] = NULL; // NULL terminate the array
    return splitString;
}

char** split_without_space(char* line) {
    char** splitString = (char**)malloc(5 * sizeof(char*));
    char* token = strtok(line, ",()");

    int i = 0;
    while (token != NULL && i < 5) {
        splitString[i] = strdup(token);

        token = strtok(NULL, ",()");
        i++;
    }
    // splitString[i] = NULL; // NULL terminate the array
    return splitString;
}



char** check_macro_pseudo(char* input_str){
    char** return_str;
    int temp = 0;
    char** str;
    char* label;
    

    str = split(input_str);

    // printf("%s\n", str[0]);

    if(strstr(str[0], ":") != NULL){
        label = str[0];
        label = concat_str(label, " ");
        temp++;
    }
    else{
        label = "";
        temp = 0;
    }

    if(!strcmp(str[temp], "move")){
        return_str = (char**)malloc(2 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "addu ");
        return_str[0] = concat_str(return_str[0], str[temp+1]);
        return_str[0] = concat_str(return_str[0], ",$zero,");
        return_str[0] = concat_str(return_str[0], str[temp+2]);
        return_str[1] = "end str";
        return return_str;
    }
    else if(!strcmp(str[temp], "li")){
        return_str = (char**)malloc(2 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "ori ");
        return_str[0] = concat_str(return_str[0], str[temp+1]);
        return_str[0] = concat_str(return_str[0], ",$zero,");
        return_str[0] = concat_str(return_str[0], str[temp+2]);
        return_str[1] = "end str";
        return return_str;
    }
    else if(!strcmp(str[temp], "lw")){
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "lui ");
        return_str[0] = concat_str(return_str[0], str[temp+2]);
        return_str[0] = concat_str(return_str[0], ",0x1001");
        return_str[1] = "lw ";
        return_str[0] = concat_str(return_str[1], str[temp+1]);
        return_str[0] = concat_str(return_str[0], ",12($at)");
        return_str[2] = "end str";
        return return_str;
    }
    else if(!strcmp(str[temp], "print_str")){
        return_str = (char**)malloc(4 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,4");
        return_str[1] = "li $a0,";
        return_str[1] = concat_str(return_str[1], str[temp+1]);
        return_str[2] = "syscall";
        return_str[3] = "end str";
        return return_str;
    }
    else if(!strcmp(str[temp], "read_str")){
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,8");
        return_str[1] = "syscall";
        return_str[2] = "end str";
        return return_str;
    }
    else if(!strcmp(str[temp], "print_integer")){
        return_str = (char**)malloc(4 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,1");
        return_str[1] = "li $a0,<parameter>";
        return_str[1] = concat_str(return_str[1], str[temp+1]);
        return_str[2] = "syscall";
        return_str[3] = "end str";
        return return_str;
    }
    else if(!strcmp(str[temp], "read_integer")){
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,8");
        return_str[1] = "syscall";
        return_str[2] = "end str";
        return return_str;
    }
    else if(!strcmp(str[temp], "exit")){
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,10");
        return_str[1] = "syscall";
        return_str[2] = "end str";
        return return_str;
    }
    else if(!strcmp(str[temp], "GCD")){
        return_str = (char**)malloc(19 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "subu $sp,$sp,32");
        return_str[1] = "sw $ra, 28($sp)";
        return_str[2] = "sw $s0, 24($sp)";
        return_str[3] = "sw $s1, 20($sp)";
        return_str[4] = "add $s0, $a0, $0";
        return_str[5] = "add $s1, $a1, $0";
        return_str[6] = "beqz $s1, return_a";
        return_str[7] = "add $a0, $0, $s1	";
        return_str[8] = "div $s0, $s1";
        return_str[9] = "mfhi $a1";
        return_str[10] = "recurse: jal GCD";
        return_str[11] = "bc: lw $ra, 28 ($sp)";
        return_str[12] = "lw $s0, 24 ($sp)";
        return_str[13] = "lw $s1, 20 ($sp)";
        return_str[14] = "addu $sp, $sp, 32";
        return_str[15] = "jr $ra";
        return_str[16] = "return_a: add $v0, $0, $s0";
        return_str[17] = "j bc";
        return_str[18] = "end str";
        return return_str;
    }
    else{
        return_str = (char**)malloc(2 * sizeof(char*));
        return_str[0] = "not macro";
        return_str[1] = "end str";
        return return_str;
    }

}


void parse_data(char *line, int num_data_line){
    char **parsed_line;
    char **return_str;

    // printf("%d\n", num_data_line);

    parsed_line = split_without_space(line);


    if(strstr(parsed_line[0], ":") != NULL){
        parsed_line = split(line);
        
        data_array[num_data_line][0] = parsed_line[0];
        data_array[num_data_line][1] = parsed_line[2];
    }
    else{
        for(int i = 0; i < 2; i++){
            data_array[num_data_line][i] = parsed_line[i+1];
        }
    }

}

void parse_input_file(const char* filename, Inst** instructions) {
    FILE* file = fopen(filename, "r");

    int num_lines = 0;
    int num_data_line = 0;
    int is_data_seg = 0;
    fscanf(file, "%d", &num_lines);

    char line[MAX_LINE_LENGTH];
    char **lines = (char**)malloc(num_lines * sizeof(char*));
    char **macros_pseudo = (char**)malloc(MAX_ARR_SIZE * sizeof(char*));
    char **to_parse = (char**)malloc(MAX_ARR_SIZE * sizeof(char*));
    char *compare_str;

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

        len_pseudo = 0;
        macros_pseudo = check_macro_pseudo(lines[i]);
        compare_str = macros_pseudo[len_pseudo];
        while(strstr(compare_str, "end str") == NULL){
            to_parse[len_pseudo] = macros_pseudo[len_pseudo];
            len_pseudo++;
            compare_str = macros_pseudo[len_pseudo];
        }
        len_pseudo--;

        if(to_parse[0] == "not macro"){
            to_parse[0] = strdup(line);
        }
        
        if(strstr(to_parse[0], ".data") != NULL){
            is_data_seg = 1;
            continue;
        }
        
        if(!is_data_seg){
            for(int a = 0; a <= len_pseudo; a++){
                //to_parse[a] has the line to parse
                //there will be problems with num_lines because input number of lines is not the same as
                //new number of lines with parsed macros since itll be much more
                // if(strstr(to_parse[a], ".text") == NULL){
                //     printf("%s\n", to_parse[a]);
                // }

                

            //     char** parsedBySpace = (char**)malloc(num_lines * sizeof(char*));

            //     if(strstr(to_parse[a], " ") == NULL){
            //         parsedBySpace = split(to_parse[a]); 
            //     }
            //     else{
            //         continue;
            //     }

            //     if(a != 0){
            //         i++;
            //     }

                // instructions[i] = (Inst*)malloc(sizeof(Inst));

            //     // label
            //     if (strchr(parsedBySpace[0], ':') != NULL) {
            //         int len_str = strlen(parsedBySpace[0]);
            //         (parsedBySpace[0])[len_str - 1] = '\0';
            //         instructions[i]->label = strdup(parsedBySpace[0]);

            //         instructions[i]->mnemonic = strdup(parsedBySpace[1]);
            //     }
            //     else {
            //         instructions[i]->label = "0";

            //     // mnemonic
            //         if (strchr(parsedBySpace[0], '.') != NULL) {
            //             instructions[i]->mnemonic = "0";
            //         }
            //         else
            //             instructions[i]->mnemonic = strdup(parsedBySpace[0]);
            //     }

            //     // type
            //     if ((strcmp(instructions[i]->mnemonic, "add") == 0) || (strcmp(instructions[i]->mnemonic, "sub") == 0) ||
            //         (strcmp(instructions[i]->mnemonic, "and") == 0) || (strcmp(instructions[i]->mnemonic, "or") == 0) ||
            //         (strcmp(instructions[i]->mnemonic, "slt") == 0) || (strcmp(instructions[i]->mnemonic, "move") == 0)) {
            //         instructions[i]->type = "R";
            //     }
            //     else if ((strcmp(instructions[i]->mnemonic, "addi") == 0) || (strcmp(instructions[i]->mnemonic, "addiu") == 0)) {
            //         instructions[i]->type = "I";
            //     }
            //     else if ((strcmp(instructions[i]->mnemonic, "beq") == 0) || (strcmp(instructions[i]->mnemonic, "bne") == 0)) {
            //         instructions[i]->type = "branch";
            //     }
            //     else if ((strcmp(instructions[i]->mnemonic, "lw") == 0) || (strcmp(instructions[i]->mnemonic, "sw") == 0)) {
            //         instructions[i]->type = "transfer";
            //     }
            //     else if (strcmp(instructions[i]->mnemonic, "jr") == 0) {
            //         instructions[i]->type = "jr";
            //     }
            //     else if ((strcmp(instructions[i]->mnemonic, "j") == 0) || (strcmp(instructions[i]->mnemonic, "jal") == 0)) {
            //         instructions[i]->type = "J";
            //     }
            //     else {
            //         instructions[i]->type = "0";
            //     }

            //     for (int k = 0; k < 1; k++) {
            //         if(parsedBySpace[k] != NULL){
            //             free(parsedBySpace[k]);
            //         }
            //     }
            //     free(parsedBySpace);
            }
            
            i++;
        }
        else{
            parse_data(to_parse[0], num_data_line);
            num_data_line++;
        }
    }

    for(int i = 0; i < 5; i++){
        printf("%s ", data_array[i][0]);
        printf("%s\n", data_array[i][1]);
    }

    free(to_parse);
    free(macros_pseudo);
    fclose(file);
}

void symbol_table(int num_lines, Inst** instructions) {
    FILE* symboltable = fopen("symboltable.txt", "w");
    ftruncate(fileno(symboltable), 0);

    char *temp_str = (char*)malloc(8 * sizeof(char*));
    int temp = 0;

    for (int i=0; i<num_lines; i++) {
        temp = (i+1)*4 + 4194304 - 8;
        snprintf(temp_str, sizeof(temp_str), "%X\n", temp);
        instructions[i]->address = temp_str;
        
        if (strcmp(instructions[i]->label, "0") != 0) {
            fprintf(symboltable, instructions[i]->label);
            fprintf(symboltable, "\t");
            fprintf(symboltable, "0x");
            fprintf(symboltable, instructions[i]->address);
        }
    }
}


int main(int argc, char* argv[]) {
    FILE* file = fopen("mips.txt", "r");
    final_total_lines = 0;

    int num_lines = 0;
    fscanf(file, "%d", &num_lines);
    
    Inst** instructions;

    instructions = calloc(MAX_ARR_SIZE, MAX_LINE_LENGTH);
    
    parse_input_file("mips.txt", instructions);
    
    // symbol_table(num_lines, instructions);

    // printf("LABEL\tTYPE\tMNEMONIC ADDRESS\n");
    // for (int i=0; i<3; i++) {
    //     printf("%s\t%s\t%s\n", instructions[i]->label, instructions[i]->type, instructions[i]->mnemonic);
    // }

    free(instructions);
    return 0;
}
