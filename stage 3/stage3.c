#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>   
#include "macro.c"

#define MAX_LINE_LENGTH 256
#define MAX_ARR_SIZE 100
#define INT_MAX 4294967295

int reg_to_index(const char*);
int hex_to_decimal(const char*);
char* imm_int_to_binary(int decimal);
char* concat_str(const char*, const char*);
char* convert_register(const char*);
char* imm_to_binary(const char*);
char* prep_JTA(const char*);
char* address_to_binary(const char*);
char** split(char*);

int final_total_lines = 0;
char* data_array[MAX_ARR_SIZE][3];
int memory_array[MAX_ARR_SIZE][2];
char* var_buffer[MAX_ARR_SIZE][2];

typedef struct instruction_line Inst;
struct instruction_line {
    char* type;
    char* address;
    char* label;
    char* mnemonic;
    char* binary;
    char** binaryArr;
    char** operands;
};


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

char** check_macro_pseudo(char* input_str) {
    char** return_str;
    int temp = 0;
    char** str;
    char* label;

    str = split(input_str);

    // printf("%s\n", str[0]);

    if (strstr(str[0], ":") != NULL) {
        label = str[0];
        label = concat_str(label, " ");
        temp++;
    }
    else {
        label = "";
        temp = 0;
    }

    if (!strcmp(str[temp], "move")) {
        return_str = (char**)malloc(2 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "addu ");
        return_str[0] = concat_str(return_str[0], str[temp+1]);
        return_str[0] = concat_str(return_str[0], ",$zero,");
        return_str[0] = concat_str(return_str[0], str[temp+2]);
        return_str[1] = "end str";
        return return_str;
    }
    else if (!strcmp(str[temp], "li")) {
        return_str = (char**)malloc(2 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "ori ");
        return_str[0] = concat_str(return_str[0], str[temp+1]);
        return_str[0] = concat_str(return_str[0], ",$zero,");
        return_str[0] = concat_str(return_str[0], str[temp+2]);
        return_str[1] = "end str";
        return return_str;
    }
    else if (!strcmp(str[temp], "la")) {
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "lui $at,0x1000");
        return_str[1] = concat_str(return_str[1], "ori ");
        return_str[1] = concat_str(return_str[1], str[temp+1]);
        return_str[1] = concat_str(return_str[1], ",$at,16");
        return_str[2] = "end str";
        return return_str;
    } 
    // else if (!strcmp(str[temp], "lw")) {
    //     return_str = (char**)malloc(3 * sizeof(char*));
    //     return_str[0] = label;
    //     return_str[0] = concat_str(return_str[0], "lui ");
    //     return_str[0] = concat_str(return_str[0], str[temp+2]);
    //     return_str[0] = concat_str(return_str[0], ",0x1001");
    //     return_str[1] = "lw ";
    //     return_str[1] = concat_str(return_str[1], str[temp+1]);
    //     return_str[1] = concat_str(return_str[1], ",12(");
    //     return_str[1] = concat_str(return_str[1], str[temp+2]);
    //     return_str[1] = concat_str(return_str[1], ")");
    //     return_str[2] = "end str";
    //     return return_str;
    // }
    else if (!strcmp(str[temp], "print_str")) {
        for (int i = 0; i < MAX_ARR_SIZE; i++)
        {
            if(var_buffer[i][0] == NULL && var_buffer[i][1] == NULL){
                var_buffer[i][0] = "print str";
                var_buffer[i][1] = strdup(str[temp+1]);
                break;
            }
        }
        return_str = (char**)malloc(5 * sizeof(char*));
        return_str = convert_macro(str);
        return return_str;
    }
    else if (!strcmp(str[temp], "read_str")) {
        for (int i = 0; i < MAX_ARR_SIZE; i++)
        {
            if(var_buffer[i][0] == NULL && var_buffer[i][1] == NULL){
                var_buffer[i][0] = "read str";
                var_buffer[i][1] = strdup(str[temp+1]);
                break;
            }
        }
        return_str = (char**)malloc(6 * sizeof(char*));
        return_str = convert_macro(str);
        return return_str;
    }
    else if (!strcmp(str[temp], "print_integer")) {
        for (int i = 0; i < MAX_ARR_SIZE; i++)
        {
            if(var_buffer[i][0] == NULL && var_buffer[i][1] == NULL){
                var_buffer[i][0] = "print int";
                var_buffer[i][1] = strdup(str[temp+1]);
                break;
            }
        }
        return_str = (char**)malloc(5 * sizeof(char*));
        return_str = convert_macro(str);
        return return_str;
    }
    else if (!strcmp(str[temp], "read_integer")) {
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str = convert_macro(str);
        return return_str;
    }
    else if (!strcmp(str[temp], "exit")) {
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str = convert_macro(str);
        return return_str;
    }
    else if (!strcmp(str[temp], "GCD")) {
        return_str = (char**)malloc(19 * sizeof(char*));
        return_str = convert_macro(str);
        return return_str;
    }
    else {
        return_str = (char**)malloc(2 * sizeof(char*));
        return_str[0] = "not macro";
        return_str[1] = "end str";
        return return_str;
    }
}

void parse_data(char *line, int num_data_line) {
    char **parsed_line;
    char **return_str;

    // printf("%d\n", num_data_line);

    parsed_line = split_without_space(line);

    if (strstr(parsed_line[0], ":") != NULL) {
        parsed_line = split(line);
        
        data_array[num_data_line][0] = parsed_line[0];
        data_array[num_data_line][1] = parsed_line[2];
    }
    else {
        data_array[num_data_line][0] = parsed_line[1];
        data_array[num_data_line][1] = parsed_line[2];
        data_array[num_data_line][1]++;
        data_array[num_data_line][1][strlen(data_array[num_data_line][1])-2] = '\0';
    }
}

int parse_input_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    FILE* temp = fopen("temp.txt", "w");
    ftruncate(fileno(temp), 0);

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
    int lines_counter = 0;

    while (fgets(line, sizeof(line), file)) {
        int len_pseudo = 0;

        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';

        // printf("%s\n", line);
        if (i == -1) {
            i++;
            continue;
        }

        lines[i] = strdup(line);

        len_pseudo = 0;
        macros_pseudo = check_macro_pseudo(lines[i]);
        compare_str = macros_pseudo[len_pseudo];
        while (strstr(compare_str, "end str") == NULL) {
            to_parse[len_pseudo] = macros_pseudo[len_pseudo];
            len_pseudo++;
            compare_str = macros_pseudo[len_pseudo];
        }
        len_pseudo--;

        if (!strcmp(to_parse[0], "not macro")) {
            to_parse[0] = strdup(line);
        }
        
        if (!strcmp(to_parse[0], ".data")) {
            is_data_seg = 1;
            continue;
        }
        
        if (!is_data_seg) {
            for (int a=0; a<=len_pseudo; a++) {
                if (strstr(to_parse[a], ".text") == NULL && strstr(to_parse[a], ".data") == NULL && strstr(to_parse[a], ".include") == NULL) {
                    fprintf(temp, "%s\n", to_parse[a]);
                    lines_counter++;
                }
            }
        }
        else {
            parse_data(to_parse[0], num_data_line);
            num_data_line++;
        }
    }

    /* for (int i = 0; i < 5; i++) {
        printf("%s ", data_array[i][0]);
        printf("%s\n", data_array[i][1]);
    } */
    free(to_parse);
    free(macros_pseudo);
    fclose(temp);
    fclose(file);
    return lines_counter;
}

void parse_instructions(const char* filename, Inst** instructions, int num_lines) {
    FILE* file = fopen(filename, "r");

    char line[MAX_LINE_LENGTH];
    char **lines = (char**)malloc(num_lines * sizeof(char*));
    char **to_parse = (char**)malloc(MAX_ARR_SIZE * sizeof(char*));

    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        int len_pseudo = 0;

        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';


        lines[i] = strdup(line);

        to_parse[0] = lines[i];

        for (int a = 0; a <= len_pseudo; a++) {
            char** parsedInst = (char**)malloc(num_lines * sizeof(char*));
            parsedInst = split(to_parse[len_pseudo]); 

            instructions[i] = (Inst*)malloc(sizeof(Inst));

            // label
            if (strchr(parsedInst[0], ':') != NULL) {
                int len_str = strlen(parsedInst[0]);
                (parsedInst[0])[len_str - 1] = '\0';
                instructions[i]->label = strdup(parsedInst[0]);

                instructions[i]->mnemonic = strdup(parsedInst[1]);
            }
            else {
                instructions[i]->label = "0";

            // mnemonic
            if (strchr(parsedInst[0], '.') != NULL) {
                instructions[i]->mnemonic = "0";
            }
            else
                instructions[i]->mnemonic = strdup(parsedInst[0]);
            }

            // type
            if ((strcmp(instructions[i]->mnemonic, "add") == 0) || (strcmp(instructions[i]->mnemonic, "sub") == 0) ||
                (strcmp(instructions[i]->mnemonic, "addu") == 0) || (strcmp(instructions[i]->mnemonic, "subu") == 0) ||
                (strcmp(instructions[i]->mnemonic, "and") == 0) || (strcmp(instructions[i]->mnemonic, "or") == 0) ||
                (strcmp(instructions[i]->mnemonic, "slt") == 0) || (strcmp(instructions[i]->mnemonic, "move") == 0))
                instructions[i]->type = "R";
            else if ((strcmp(instructions[i]->mnemonic, "addi") == 0) || (strcmp(instructions[i]->mnemonic, "addiu") == 0)  ||
                (strcmp(instructions[i]->mnemonic, "li") == 0) || (strcmp(instructions[i]->mnemonic, "ori") == 0) ||
                (strcmp(instructions[i]->mnemonic, "lui") == 0))
                instructions[i]->type = "I";
            else if ((strcmp(instructions[i]->mnemonic, "beq") == 0) || (strcmp(instructions[i]->mnemonic, "bne") == 0) ||
                    (strcmp(instructions[i]->mnemonic, "beqz") == 0))
                instructions[i]->type = "branch";
            else if ((strcmp(instructions[i]->mnemonic, "lw") == 0) || (strcmp(instructions[i]->mnemonic, "sw") == 0))
                instructions[i]->type = "T"; // transfer
            else if (strcmp(instructions[i]->mnemonic, "jr") == 0)
                instructions[i]->type = "jr";
            else if ((strcmp(instructions[i]->mnemonic, "j") == 0) || (strcmp(instructions[i]->mnemonic, "jal") == 0))
                instructions[i]->type = "J";
            else if (strcmp(instructions[i]->mnemonic, "div") == 0)
                instructions[i]->type = "div";
            else if (strcmp(instructions[i]->mnemonic, "mfhi") == 0)
                instructions[i]->type = "mfhi";
            else if (strcmp(instructions[i]->mnemonic, "la") == 0)
                instructions[i]->type = "la";
            else
                instructions[i]->type = "0";

            instructions[i]->operands = (char**)malloc(3 * sizeof(char*));

            for (int j=0; j<3; j++) {
                if (strcmp(instructions[i]->label, "0") == 0)
                    instructions[i]->operands[j] = parsedInst[j+1];
                else
                    instructions[i]->operands[j] = parsedInst[j+2];
            }

            for (int j=0; j<3; j++) {
                if (instructions[i]->operands[j] == NULL)
                    instructions[i]->operands[j] = "NULL";
            }

            for (int j=0; j<1; j++){
                free(parsedInst[j]);
            }
        }
        i++;
    }
    fclose(file);
}

void symbol_table(int num_lines, Inst** instructions) {
    FILE* symboltable = fopen("symboltable.txt", "w");
    ftruncate(fileno(symboltable), 0);
    int i = 0;

    char *temp_str = (char*)malloc(11 * sizeof(char*));
    long int temp = 0;

    while(data_array[i][0] != NULL){
        temp = 268435456 + ((i+1)*4);
        snprintf(temp_str, sizeof(temp_str), "%X", temp);
        fprintf(symboltable, "%s", data_array[i][0]);
        fprintf(symboltable, "\t");
        fprintf(symboltable, "0x");
        fprintf(symboltable, "%s", temp_str);
        snprintf(temp_str, sizeof(temp_str), "%X", ((i+1)*4)%16);
        fprintf(symboltable, "%s", temp_str);
        fprintf(symboltable, "\n");
        i++;
    }

    for (int i=0; i<(num_lines); i++) {
        temp = (i+1)*4 + 4194304 - 4;
        snprintf(temp_str, sizeof(temp_str), "%X", temp);
        instructions[i]->address = strdup(temp_str);
        
        if (strcmp(instructions[i]->label, "0") != 0) {
            fprintf(symboltable, "%s", instructions[i]->label);
            fprintf(symboltable, "\t");
            fprintf(symboltable, "0x00");
            fprintf(symboltable, "%s", instructions[i]->address);
            fprintf(symboltable, "\n");
        }
    }
    free(temp_str);
    fclose(symboltable);
}

void print_execute(int num_lines, Inst** instructions) {
    FILE* execute = fopen("execute.txt", "w");
    ftruncate(fileno(execute), 0);

    for (int i = 0; i < (num_lines); i++) {
        // printf("%d: %s\n", i, instructions[i]->mnemonic);
        // fflush(stdout);

        fprintf(execute, "%s ", instructions[i]->mnemonic);

        if (strcmp(instructions[i]->mnemonic, "syscall") == 0) {
            instructions[i]->binaryArr = (char**)malloc(2 * sizeof(char*));
            instructions[i]->binaryArr[0] = strdup("0000000000000000");
            instructions[i]->binaryArr[1] = strdup("0000000000000000");

            for (int j=0; j<2; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // R type
        else if ((strcmp(instructions[i]->type, "R") == 0) || (strcmp(instructions[i]->mnemonic, "move") == 0)) {
            instructions[i]->binaryArr = (char**)malloc(6 * sizeof(char*));
            
            // opcode and shamt
            instructions[i]->binaryArr[0] = strdup("000000");
            instructions[i]->binaryArr[4] = strdup("00000");

            // funct
            if (strcmp(instructions[i]->mnemonic, "add") == 0)
                instructions[i]->binaryArr[5] = strdup("100000");
            else if (strcmp(instructions[i]->mnemonic, "move") == 0)        // 20
                instructions[i]->binaryArr[5] = strdup("100000");
            else if (strcmp(instructions[i]->mnemonic, "addu") == 0)    // 21
                instructions[i]->binaryArr[5] = strdup("100001");
            else if (strcmp(instructions[i]->mnemonic, "sub") == 0)     // 22
                instructions[i]->binaryArr[5] = strdup("100010");
            else if (strcmp(instructions[i]->mnemonic, "subu") == 0)    // 23
                instructions[i]->binaryArr[5] = strdup("100011");
            else if (strcmp(instructions[i]->mnemonic, "and") == 0)    // 24
                instructions[i]->binaryArr[5] = strdup("100100");
            else if (strcmp(instructions[i]->mnemonic, "or") == 0)      // 25   
                instructions[i]->binaryArr[5] = strdup("100101");
            else if (strcmp(instructions[i]->mnemonic, "slt") == 0)     // 2a
                instructions[i]->binaryArr[5] = strdup("101010");

            // registers
            instructions[i]->binaryArr[1] = convert_register(instructions[i]->operands[1]);   // rs

            if (strcmp(instructions[i]->mnemonic, "move") == 0)
                instructions[i]->binaryArr[2] = strdup("00000");   // rt
            else
                instructions[i]->binaryArr[2] = convert_register(instructions[i]->operands[2]);   // rt

            instructions[i]->binaryArr[3] = convert_register(instructions[i]->operands[0]);   // rd

            for (int j=0; j<6; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // div
        else if (strcmp(instructions[i]->type, "div") == 0) {
            instructions[i]->binaryArr = (char**)malloc(5 * sizeof(char*));
            
            instructions[i]->binaryArr[0] = strdup("000000");
            instructions[i]->binaryArr[1] = convert_register(instructions[i]->operands[0]);
            instructions[i]->binaryArr[2] = convert_register(instructions[i]->operands[1]);
            instructions[i]->binaryArr[3] = strdup("0000000000");
            instructions[i]->binaryArr[4] = strdup("011010");

            for (int j=0; j<5; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // mfhi
        else if (strcmp(instructions[i]->type, "mfhi") == 0) {
            instructions[i]->binaryArr = (char**)malloc(4 * sizeof(char*));

            instructions[i]->binaryArr[0] = strdup("0000000000000000");
            instructions[i]->binaryArr[1] = convert_register(instructions[i]->operands[0]);
            instructions[i]->binaryArr[2] = strdup("00000");
            instructions[i]->binaryArr[3] = strdup("010000");

            for (int j=0; j<4; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // I type
        else if (strcmp(instructions[i]->type, "I") == 0) {
            instructions[i]->binaryArr = (char**)malloc(4 * sizeof(char*));

            // opcode
            if (strcmp(instructions[i]->mnemonic, "addi") == 0)           // 8
                instructions[i]->binaryArr[0] = strdup("001000");
            else if (strcmp(instructions[i]->mnemonic, "addiu") == 0)     // 9
                instructions[i]->binaryArr[0] = strdup("001001");
            else if (strcmp(instructions[i]->mnemonic, "ori") == 0 && strcmp(instructions[i]->operands[0], "$v0") == 0)    // d
                instructions[i]->binaryArr[0] = strdup("001101");
            else if (strcmp(instructions[i]->mnemonic, "ori") == 0)     // d
                instructions[i]->binaryArr[0] = strdup("001101");

            // operands 
            if ((strcmp(instructions[i]->mnemonic, "li") == 0) || (strcmp(instructions[i]->mnemonic, "lui") == 0)) {
                if (strcmp(instructions[i]->mnemonic, "li") == 0)
                    instructions[i]->binaryArr[0] = strdup("001101");
                else
                    instructions[i]->binaryArr[0] = strdup("001111");
                instructions[i]->binaryArr[1] = strdup("00000");
                instructions[i]->binaryArr[2] = convert_register(instructions[i]->operands[0]);
                instructions[i]->binaryArr[3] = imm_to_binary(instructions[i]->operands[1]);    // imm
            }
            else {
                instructions[i]->binaryArr[1] = convert_register(instructions[i]->operands[1]);     // rs
                instructions[i]->binaryArr[2] = convert_register(instructions[i]->operands[0]);     // rt
                instructions[i]->binaryArr[3] = imm_to_binary(instructions[i]->operands[2]);    // imm
            }

            for (int j=0; j<4; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // branch
        else if (strcmp(instructions[i]->type, "branch") == 0) {
            instructions[i]->binaryArr = (char**)malloc(4 * sizeof(char*));
            int BTA;
        
            // opcode
            if (strcmp(instructions[i]->mnemonic, "beq") == 0) {            // 4
                instructions[i]->binaryArr[0] = strdup("000100");

                for (int j = 0; j < num_lines; j++) {
                    if (strstr(instructions[j]->label, instructions[i]->operands[2]) != NULL) {
                        BTA = hex_to_decimal(instructions[j]->address) - hex_to_decimal(instructions[i+1]->address);
                        instructions[i]->binaryArr[3] = strdup(imm_int_to_binary(BTA));

                        break;
                    }
                }
            }
            else if (strcmp(instructions[i]->mnemonic, "beqz") == 0) {
                instructions[i]->binaryArr[0] = strdup("000100");

                for (int j = 0; j < num_lines; j++) {
                    if (strstr(instructions[j]->label, instructions[i]->operands[1]) != NULL) {
                        BTA = hex_to_decimal(instructions[j]->address) - hex_to_decimal(instructions[i+1]->address);
                        instructions[i]->binaryArr[3] = strdup(imm_int_to_binary(BTA));

                        break;
                    }
                }
            }
            else if (strcmp(instructions[i]->mnemonic, "bne") == 0) {       // 5
                instructions[i]->binaryArr[0] = strdup("000101");

                for (int j = 0; j < num_lines; j++) {
                    if (strstr(instructions[j]->label, instructions[i]->operands[2]) != NULL) {
                        BTA = hex_to_decimal(instructions[j]->address) - hex_to_decimal(instructions[i+1]->address);
                        instructions[i]->binaryArr[3] = strdup(imm_int_to_binary(BTA));

                        break;
                    }
                }
            }

            // operands
            instructions[i]->binaryArr[1] = convert_register(instructions[i]->operands[0]);
            instructions[i]->binaryArr[2] = convert_register(instructions[i]->operands[1]);

            for (int j=0; j<4; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // transfer
        else if (strcmp(instructions[i]->type, "T") == 0) { // transfer
            instructions[i]->binaryArr = (char**)malloc(4 * sizeof(char*));
        
            // opcode
            if (strcmp(instructions[i]->mnemonic, "lw") == 0)            // 23
                instructions[i]->binaryArr[0] = strdup("100011");
            else if (strcmp(instructions[i]->mnemonic, "sw") == 0)       // 2b
                instructions[i]->binaryArr[0] = strdup("101011");

            // operands
            instructions[i]->binaryArr[1] = convert_register(instructions[i]->operands[2]);     // rs
            instructions[i]->binaryArr[2] = convert_register(instructions[i]->operands[0]);     // rt
            instructions[i]->binaryArr[3] = imm_to_binary(instructions[i]->operands[1]);    // imm

            for (int j=0; j<4; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // jump
        else if (strcmp(instructions[i]->type, "J") == 0) {
            instructions[i]->binaryArr = (char**)malloc(2 * sizeof(char*));

            // opcode
            if (strcmp(instructions[i]->mnemonic, "j") == 0) {           // 8
                instructions[i]->binaryArr[0] = strdup("000010");

                for (int j=0; j<num_lines; j++) {
                    if (strstr(instructions[j]->label, instructions[i]->operands[0]) != NULL) {
                        instructions[i]->binaryArr[1] = strdup(prep_JTA(instructions[j]->address));
                        break;
                    }
                }
            }
            else if (strcmp(instructions[i]->mnemonic, "jal") == 0) {     // 9
                instructions[i]->binaryArr[0] = strdup("000011");

                for (int j=0; j<num_lines; j++) {
                    if (strstr(instructions[j]->label, instructions[i]->operands[0]) != NULL) {
                        instructions[i]->binaryArr[1] = strdup(prep_JTA(instructions[j]->address));
                        break;
                    }
                }
            }
        
            for (int j=0; j<2; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // jr
        else if (strcmp(instructions[i]->type, "jr") == 0) {
            instructions[i]->binaryArr = (char**)malloc(4 * sizeof(char*));

            instructions[i]->binaryArr[0] = strdup("000000");
            instructions[i]->binaryArr[1] = convert_register(instructions[i]->operands[0]);
            instructions[i]->binaryArr[2] = strdup("000000000000000");
            instructions[i]->binaryArr[3] = strdup("001000");
        
            for (int j=0; j<4; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // la
        else if (strcmp(instructions[i]->type, "la") == 0) {
            instructions[i]->binaryArr = (char**)malloc(2 * sizeof(char*));

            instructions[i]->binaryArr[0] = strdup("00111100000000010001000000000001");
            instructions[i]->binaryArr[1] = strdup("00110100001001000000000000011111");
        
            for (int j=0; j<2; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }
        
        else {
            fprintf(execute, "Instruction not supported\n");
        }

        // char output[32];
        // sprintf(output, "%s\n", instructions[i]->binary);
        // fprintf(execute, "%s\n", output);
    }
    fclose(execute);
    printf("Assemble: operation completed successfully.\n");
}

void execute(int num_lines, Inst** instructions) {
    int regFile[32];
    int syscall_info;
    int temp = 0;
    int target_i = 0;
    int branch_taken = 0;
    int return_address = 0;

    for (int i = 0; i < 31; i++) {
        regFile[i] = 0;
    }
    

    char temp_str[1000];

    for (int i = 0; i < (num_lines); i++) {
        // printf("%s %s\n", instructions[i]->mnemonic, instructions[i]->operands[0]);

        if (branch_taken == 1){
            i = target_i;
            branch_taken = 0;
        }
        // printf("%d: %s %s\n", i, instructions[i]->mnemonic, instructions[i]->operands[0]);

        //pseudo running of the print and read int and str macros
        if((strstr(instructions[i]->mnemonic, "addiu") != NULL && strstr(instructions[i]->operands[0], "$v0") != NULL) ||
            (strstr(instructions[i]->mnemonic, "li") != NULL && strstr(instructions[i-2]->mnemonic, "ori") != NULL && i > 2)){
            for (int i = 0; i < MAX_ARR_SIZE; i++)
            {
                if(var_buffer[i][0] == NULL && var_buffer[i][1] == NULL){
                    break;
                }
                else{
                    if(strstr(var_buffer[i][0], "print str") != NULL){
                        for (int j = 0; j < MAX_ARR_SIZE; j++){
                            if(strstr(data_array[j][0], var_buffer[i][1]) != NULL){
                                printf("%s\n", data_array[j][1]);
                                break;
                            }
                        }
                        var_buffer[i][0] = "done";
                        var_buffer[i][1] = "done";
                        syscall_info = -1;
                        break;
                    }
                    else if(strstr(var_buffer[i][0], "print int") != NULL){
                        for (int j = 0; j < MAX_ARR_SIZE; j++){
                            if(strstr(data_array[j][0], var_buffer[i][1]) != NULL){
                                printf("%s\n", data_array[j][1]);
                                break;
                            }
                        }
                        var_buffer[i][0] = "done";
                        var_buffer[i][1] = "done";
                        syscall_info = -1;
                        break;
                    }
                    else if(strstr(var_buffer[i][0], "read str") != NULL){
                        for (int j = 0; j < MAX_ARR_SIZE; j++){
                            if(data_array[j][0] == NULL && data_array[j][1] == NULL){
                                data_array[j][0] = var_buffer[i][1];
                                fgets(temp_str, 1000, stdin);
                                if (strlen(temp_str) > 0 && temp_str[strlen(temp_str) - 1] == '\n') {
                                    temp_str[strlen(temp_str) - 1] = '\0';
                                }
                                data_array[j][1] = temp_str;
                                break;
                            }
                            else if(strstr(data_array[j][0], var_buffer[i][1]) != NULL){
                                fgets(temp_str, 1000, stdin);
                                if (strlen(temp_str) > 0 && temp_str[strlen(temp_str) - 1] == '\n') {
                                    temp_str[strlen(temp_str) - 1] = '\0';
                                }
                                data_array[j][1] = temp_str;
                                break;
                            }
                        }
                        var_buffer[i][0] = "done";
                        var_buffer[i][1] = "done";
                        syscall_info = -1;
                        break;
                    }
                }
            }
        }
        else if (strcmp(instructions[i]->mnemonic, "syscall") == 0) {
            // print
            if (syscall_info == 1){
                printf("%d\n", regFile[4]);
            }
            else if (syscall_info == 4){
                temp = ((regFile[4] - 268435456) / 4) - 1;
                if (temp >= 0) {
                    printf("%s\n", data_array[temp][1]);
                }
            }
            else if (syscall_info == 11) {
                temp = ((regFile[4] - 268435456) / 4) - 1;
                if (temp >= 0) {
                    printf("%s\n", data_array[temp][1]);
                }
            }

            // read
            else if (syscall_info == 5) {
                scanf("%d", &regFile[2]);
            }
            else if (syscall_info == 8) {
                scanf("%s", &regFile[2]);
            }

            //exit
            else if (syscall_info == 10){
                exit(1);
            }
        }

        else if (strstr(instructions[i]->mnemonic, "subu") != NULL && strstr(instructions[i]->operands[0], "sp") != NULL) {
            
            int a = regFile[4];
            int b = regFile[5];
            // printf("%d\n", regFile[4]);
            // printf("%d\n", regFile[5]);
            if (a < b) {
                int temp = a;
                a = b;
                b = temp;
            }
            
            while (b != 0) {
                int remainder = a % b;
                a = b;
                b = remainder;
            }
            regFile[2] = a;
            // printf("%d\n", regFile[2]);
            i = i + 17;
        }

        

        // R type
        else if ((strcmp(instructions[i]->type, "R") == 0) || (strcmp(instructions[i]->mnemonic, "move") == 0)) {

            if (strcmp(instructions[i]->mnemonic, "add") == 0) {
                if (regFile[reg_to_index(instructions[i]->operands[1])] > 0 && 
                    regFile[reg_to_index(instructions[i]->operands[2])] > INT_MAX - regFile[reg_to_index(instructions[i]->operands[1])]) {
                    exit(1);
                }
                else {
                    regFile[reg_to_index(instructions[i]->operands[0])] =
                        regFile[reg_to_index(instructions[i]->operands[1])] + regFile[reg_to_index(instructions[i]->operands[2])];
                }
            }
            else if (strcmp(instructions[i]->mnemonic, "move") == 0) {         // 20
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])];
            }
            else if (strcmp(instructions[i]->mnemonic, "addu") == 0) {    // 21
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] + regFile[reg_to_index(instructions[i]->operands[2])];
            }
            else if (strcmp(instructions[i]->mnemonic, "sub") == 0) {     // 22
                if (regFile[reg_to_index(instructions[i]->operands[1])] < 0 && 
                    regFile[reg_to_index(instructions[i]->operands[2])] < INT16_MIN - regFile[reg_to_index(instructions[i]->operands[1])]) {
                    exit(1);
                }
                else {
                    regFile[reg_to_index(instructions[i]->operands[0])] =
                        regFile[reg_to_index(instructions[i]->operands[1])] - regFile[reg_to_index(instructions[i]->operands[2])];
                }
            }
            else if (strcmp(instructions[i]->mnemonic, "subu") == 0) {    // 23
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] - regFile[reg_to_index(instructions[i]->operands[2])];
            }
            else if (strcmp(instructions[i]->mnemonic, "and") == 0) {    // 24
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] & regFile[reg_to_index(instructions[i]->operands[2])];
            }
            else if (strcmp(instructions[i]->mnemonic, "or") == 0) {      // 25   
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] | regFile[reg_to_index(instructions[i]->operands[2])];
            }
            else if (strcmp(instructions[i]->mnemonic, "slt") == 0) {     // 2a
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    (regFile[reg_to_index(instructions[i]->operands[1])] < regFile[reg_to_index(instructions[i]->operands[2])])
                    ? 1 : 0;
            }
        }

        // div
        else if (strcmp(instructions[i]->type, "div") == 0) {
            regFile[reg_to_index(instructions[i+1]->operands[0])] =
                regFile[reg_to_index(instructions[i]->operands[0])] % regFile[reg_to_index(instructions[i]->operands[1])];
        }

        // mfhi
        else if (strcmp(instructions[i]->type, "mfhi") == 0) {
            // div already takes this into account
        }

        // I type
        else if (strcmp(instructions[i]->type, "I") == 0) {

            if (strcmp(instructions[i]->mnemonic, "addi") == 0) {           // 8
                if (regFile[reg_to_index(instructions[i]->operands[1])] > 0 && 
                    atoi(instructions[i]->operands[2]) > INT_MAX - regFile[reg_to_index(instructions[i]->operands[1])]){
                    exit(1);
                }
                else {
                    regFile[reg_to_index(instructions[i]->operands[0])] =
                        regFile[reg_to_index(instructions[i]->operands[1])] + atoi(instructions[i]->operands[2]);
                }
            }
            else if (strstr(instructions[i]->mnemonic, "addiu") != NULL && strstr(instructions[i]->operands[0], "$v0") != NULL) {     // 9
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] + atoi(instructions[i]->operands[2]);
                syscall_info = atoi(instructions[i]->operands[2]);
            }
            else if (strcmp(instructions[i]->mnemonic, "addiu") == 0) {     // 9
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] + atoi(instructions[i]->operands[2]);
            }
            else if (strcmp(instructions[i]->mnemonic, "ori") == 0 && strcmp(instructions[i]->operands[0], "$v0") == 0) {    // d
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    0 | atoi(instructions[i]->operands[1]);

                syscall_info = atoi(instructions[i]->operands[2]);
            }
            else if (strcmp(instructions[i]->mnemonic, "ori") == 0) {     // d
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] | atoi(instructions[i]->operands[2]);
            }
        }

        // branch
        else if (strcmp(instructions[i]->type, "branch") == 0) {
        
            if (strcmp(instructions[i]->mnemonic, "beq") == 0) {            // 4

                if (regFile[reg_to_index(instructions[i]->operands[0])] == regFile[reg_to_index(instructions[i]->operands[1])]) {
                    for(int j = 0; j < num_lines; j++) {
                        if (strstr(instructions[j]->label, instructions[i]->operands[2]) != NULL) {
                            target_i = j;
                            branch_taken = 1;
                            break;
                        }
                        else { 
                            branch_taken = 0;
                        }
                    }
                }
                else { 
                    branch_taken = 0;
                }
            }
            else if (strcmp(instructions[i]->mnemonic, "beqz") == 0) {

                if (regFile[reg_to_index(instructions[i]->operands[0])] == 0) {
                    for (int j = 0; j < num_lines; j++) {
                        if (strstr(instructions[j]->label, instructions[i]->operands[1]) != NULL) {
                            target_i = j;
                            branch_taken = 1;
                            break;
                        }
                        else {
                            branch_taken = 0;
                        }
                    }
                }
                else {
                    branch_taken = 0;
                }
            }
            else if (strcmp(instructions[i]->mnemonic, "bne") == 0) {       // 5
                if (regFile[reg_to_index(instructions[i]->operands[0])] != regFile[reg_to_index(instructions[i]->operands[1])]) {
                    for(int j = 0; j < num_lines; j++) {
                        if (strstr(instructions[j]->label, instructions[i]->operands[2]) != NULL) {
                            target_i = j;
                            branch_taken = 1;
                            break;
                        }
                        else {
                            branch_taken = 0;
                        }
                    }
                }
                else {
                    branch_taken = 0;
                }
            }
        }

        // transfer
        else if (strcmp(instructions[i]->type, "T") == 0) {
        
            if (strcmp(instructions[i]->mnemonic, "lw") == 0) {
                
                int mem_address = 0;
                mem_address = atoi(instructions[i]->operands[1]) + regFile[reg_to_index(instructions[i]->operands[2])];

                for (int j = 0; j < MAX_ARR_SIZE; j++) {
                    if (memory_array[j][0] == 0 && memory_array[j][1] == 0) {
                        memory_array[j][0] = regFile[reg_to_index(instructions[i]->operands[0])];
                        memory_array[j][1] = mem_address;
                        break;
                    }
                    else {
                        if (memory_array[j][1] == mem_address) {
                            regFile[reg_to_index(instructions[i]->operands[0])] = memory_array[j][0];
                            break;
                        }
                    }
                }
   
            }
            else if (strcmp(instructions[i]->mnemonic, "sw") == 0) {        // 2b

                int mem_address = 0;
                mem_address = atoi(instructions[i]->operands[1]) + regFile[reg_to_index(instructions[i]->operands[2])];

                for (int j = 0; j < MAX_ARR_SIZE; j++) {
                    if (memory_array[j][0] == 0 && memory_array[j][1] == 0) {
                        memory_array[j][0] = regFile[reg_to_index(instructions[i]->operands[0])];
                        memory_array[j][1] = mem_address;
                        break;
                    }
                    else {
                        if (memory_array[j][1] == regFile[reg_to_index(instructions[i]->operands[1])]) {
                            memory_array[j][0] = regFile[reg_to_index(instructions[i]->operands[0])];
                            break;
                        }
                    }
                }
            }
        }

        // jump
        else if (strcmp(instructions[i]->type, "J") == 0) {

            if (strcmp(instructions[i]->mnemonic, "j") == 0) {           // 8

                for (int j=0; j<num_lines; j++) {
                    if (strstr(instructions[j]->label, instructions[i]->operands[0]) != NULL) {
                        target_i = j;
                        branch_taken = 1;
                        break;
                    }
                    else
                        branch_taken = 0;
                }
            }
            else if (strcmp(instructions[i]->mnemonic, "jal") == 0) {     // 9

                for (int j=0; j<num_lines; j++) {
                    if (strstr(instructions[j]->label, instructions[i]->operands[0]) != NULL) {
                        target_i = j;
                        branch_taken = 1;
                        return_address = i+1;
                        regFile[31] = atoi(instructions[j]->address) + 4;
                        break;
                    }
                    else
                        branch_taken = 0;
                }
            }
        }

        // jr
        else if (strcmp(instructions[i]->type, "jr") == 0) {
            target_i = return_address;
            branch_taken = 1;
        }
        
        else {
            printf("Instruction not supported\n");
        }
    }
    // for (int j = 8; j <= 15; j++)
    // {
    //     printf("%d\n", regFile[j]);
    // }
    // printf("%d\n", regFile[2]);
}

char *convert_register(const char* regName) {
    char* regBinary = malloc(5);

    strcpy(regBinary, regName);
    
    // $0 : 0
    if ((strcmp(regName, "$zero") == 0) || (strcmp(regName, "$0") == 0)) regBinary = "00000";

    // $at : 1
    else if (strcmp(regName, "$at") == 0) regBinary = "00001";

    // $v0-$v1 : 2-3
    else if (strcmp(regName, "$v0") == 0) regBinary = "00010"; else if (strcmp(regName, "$v1") == 0) regBinary = "00011";

    // $a0-$a3 : 4-7
    else if (strcmp(regName, "$a0") == 0) regBinary = "00100"; else if (strcmp(regName, "$a1") == 0) regBinary = "00101";
    else if (strcmp(regName, "$a2") == 0) regBinary = "00110"; else if (strcmp(regName, "$a3") == 0) regBinary = "00111";

    // $t0-$t9 : 8-15, 24-25
    else if (strcmp(regName, "$t0") == 0) regBinary = "01000"; else if (strcmp(regName, "$t1") == 0) regBinary = "01001";
    else if (strcmp(regName, "$t2") == 0) regBinary = "01010"; else if (strcmp(regName, "$t3") == 0) regBinary = "01011";
    else if (strcmp(regName, "$t4") == 0) regBinary = "01100"; else if (strcmp(regName, "$t5") == 0) regBinary = "01101";
    else if (strcmp(regName, "$t6") == 0) regBinary = "01110"; else if (strcmp(regName, "$t7") == 0) regBinary = "01111";
    else if (strcmp(regName, "$t8") == 0) regBinary = "11000"; else if (strcmp(regName, "$t9") == 0) regBinary = "11001";

    // $s0-$s7 : 16-23
    else if (strcmp(regName, "$s0") == 0) regBinary = "10000"; else if (strcmp(regName, "$s1") == 0) regBinary = "10001";
    else if (strcmp(regName, "$s2") == 0) regBinary = "10010"; else if (strcmp(regName, "$s3") == 0) regBinary = "10011";
    else if (strcmp(regName, "$s4") == 0) regBinary = "10100"; else if (strcmp(regName, "$s5") == 0) regBinary = "10101";
    else if (strcmp(regName, "$s6") == 0) regBinary = "10110"; else if (strcmp(regName, "$s7") == 0) regBinary = "10111";

    // $k0-$k1 : 26-27
    else if (strcmp(regName, "$k0") == 0) regBinary = "11010"; else if (strcmp(regName, "$k1") == 0) regBinary = "11011";

    // $gp : 28
    else if (strcmp(regName, "$gp") == 0) regBinary = "11100";
    
    // $sp : 29
    else if (strcmp(regName, "$sp") == 0) regBinary = "11101";

    // $fp : 30
    else if (strcmp(regName, "$fp") == 0) regBinary = "11110";

    // $ra : 31
    else if (strcmp(regName, "$ra") == 0) regBinary = "11111";

    return regBinary;
}


int reg_to_index(const char* regName) {
    int index;
    
    // $0 : 0
    if ((strcmp(regName, "$zero") == 0) || (strcmp(regName, "$0") == 0)) index = 0;

    // $at : 1
    else if (strcmp(regName, "$at") == 0) index = 1;

    // $v0-$v1 : 2-3
    else if (strcmp(regName, "$v0") == 0) index = 2; else if (strcmp(regName, "$v1") == 0) index = 3;

    // $a0-$a3 : 4-7
    else if (strcmp(regName, "$a0") == 0) index = 4; else if (strcmp(regName, "$a1") == 0) index = 5;
    else if (strcmp(regName, "$a2") == 0) index = 6; else if (strcmp(regName, "$a3") == 0) index = 7;

    // $t0-$t9 : 8-15, 24-25
    else if (strcmp(regName, "$t0") == 0) index = 8; else if (strcmp(regName, "$t1") == 0) index = 9;
    else if (strcmp(regName, "$t2") == 0) index = 10; else if (strcmp(regName, "$t3") == 0) index = 11;
    else if (strcmp(regName, "$t4") == 0) index = 12; else if (strcmp(regName, "$t5") == 0) index = 13;
    else if (strcmp(regName, "$t6") == 0) index = 14; else if (strcmp(regName, "$t7") == 0) index = 15;
    else if (strcmp(regName, "$t8") == 0) index = 24; else if (strcmp(regName, "$t9") == 0) index = 25;

    // $s0-$s7 : 16-23
    else if (strcmp(regName, "$s0") == 0) index = 16; else if (strcmp(regName, "$s1") == 0) index = 17;
    else if (strcmp(regName, "$s2") == 0) index = 18; else if (strcmp(regName, "$s3") == 0) index = 19;
    else if (strcmp(regName, "$s4") == 0) index = 20; else if (strcmp(regName, "$s5") == 0) index = 21;
    else if (strcmp(regName, "$s6") == 0) index = 22; else if (strcmp(regName, "$s7") == 0) index = 23;

    // $k0-$k1 : 26-27
    else if (strcmp(regName, "$k0") == 0) index = 26; else if (strcmp(regName, "$k1") == 0) index = 27;

    // $gp : 28
    else if (strcmp(regName, "$gp") == 0) index = 28;
    
    // $sp : 29
    else if (strcmp(regName, "$sp") == 0) index = 29;

    // $fp : 30
    else if (strcmp(regName, "$fp") == 0) index = 30;

    // $ra : 31
    else if (strcmp(regName, "$ra") == 0) index = 31;

    return index;
}

char *imm_to_binary(const char* decimalString) {
    char* binary = malloc(17);
    int decimal = atoi(decimalString);
    int numBits = sizeof(int) * 4;
    int i = 0;
    int negative = 0;

    // negative
    if (decimal < 0) {
        negative = 1;
        decimal = -decimal; // convert to positive
    }

    if (decimal == 0)
        binary[i++] = '0';
    else {
        while (decimal > 0) {
            binary[i++] = (decimal % 2) + '0';
            decimal /= 2;
        }
    }

    // leading zeros
    while (i < 16)
        binary[i++] = '0';

    // apply 2C if the number was negative
    if (negative) {
        for (int j = 0; j < 16; j++)
            binary[j] = (binary[j] == '0') ? '1' : '0';
        int carry = 1;
        for (int j = 0; j < 16; j++) {
            int bit = (binary[j] - '0') + carry;
            binary[j] = (bit % 2) + '0';
            carry = bit / 2;
        }
    }
    return binary;
}

char *imm_int_to_binary(int decimal) {
    char* binary = malloc(17);
    int numBits = sizeof(int) * 4;
    int i = 0;
    int negative = 0;

    // negative
    if (decimal < 0) {
        negative = 1;
        decimal = -decimal; // convert to positive
    }

    if (decimal == 0)
        binary[i++] = '0';
    else {
        while (decimal > 0) {
            binary[i++] = (decimal % 2) + '0';
            decimal /= 2;
        }
    }

    // leading zeros
    while (i < 16)
        binary[i++] = '0';

    // apply 2C if the number was negative
    if (negative) {
        for (int j = 0; j < 16; j++)
            binary[j] = (binary[j] == '0') ? '1' : '0';
        int carry = 1;
        for (int j = 0; j < 16; j++) {
            int bit = (binary[j] - '0') + carry;
            binary[j] = (bit % 2) + '0';
            carry = bit / 2;
        }
    }
    return binary;
}

char *hex_digit_to_binary(char hexDigit) {
    switch(hexDigit) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
        default: return NULL;
    }
}

char *hex_to_binary(const char *hex_string) {
    size_t hex_len = strlen(hex_string);
    size_t binary_len = hex_len * 4;
    char *binary_string = (char *)malloc(binary_len + 1);

    binary_string[binary_len] = '\0';

    for (size_t i = 0; i < hex_len; i++) {
        char *binary_digit = hex_digit_to_binary(hex_string[i]);
        strcat(binary_string, binary_digit);
    }

    return binary_string;
}

char *prep_JTA(const char* hexAddress) {
    char* binary = malloc(32);
    char* JTA = malloc(27);
    char* fullHA = malloc(8); 

    fullHA = strdup("00");
    strcat(fullHA, hexAddress);
    
    binary = strdup(hex_to_binary(fullHA));
    // binary = strdup(hexAddress);

    strncpy(JTA, binary+4, 26);
    JTA[26] = '\0';

    free(binary);

    return JTA;
}

int hex_to_decimal(const char* hexString) {
    int decimal = 0;
    sscanf(hexString, "%x", &decimal);
    return decimal;
}

char *address_to_binary(const char* decimalString) {
    char *binary = (char *)malloc(16 * sizeof(char));
    int decimal = atoi(decimalString);
    int numBits = sizeof(int) * 4;
    int i = 0;
    int negative = 0;

    // negative
    if (decimal < 0) {
        negative = 1;
        decimal = -decimal; // convert to positive
    }

    if (decimal == 0)
        binary[i++] = '0';
    else {
        while (decimal > 0) {
            binary[i++] = (decimal % 2) + '0';
            decimal /= 2;
        }
    }

    // leading zeros
    while (i < 26)
        binary[i++] = '0';

    // apply 2C if the number was negative
    if (negative) {
        for (int j = 0; j < 26; j++)
            binary[j] = (binary[j] == '0') ? '1' : '0';
        int carry = 1;
        for (int j = 0; j < 26; j++) {
            int bit = (binary[j] - '0') + carry;
            binary[j] = (bit % 2) + '0';
            carry = bit / 2;
        }
    }
    
    binary[26] = '\0';

    int start = 0;
    int end = strlen(binary) - 1;
    while (start < end) {
        char temp = binary[start];
        binary[start] = binary[end];
        binary[end] = temp;
        start++;
        end--;
    }

    return binary;
}

int main(int argc, char* argv[]) {
    FILE* file = fopen("mips2.txt", "r");
    int final_total_lines = 0;

    int num_lines = 0;
    fscanf(file, "%d", &num_lines);

    for (int i = 0; i < MAX_ARR_SIZE; i++) {
        for (int j = 0; j < 2; j++) {
            memory_array[i][j] = 0;
        }
    }

    for (int i = 0; i < MAX_ARR_SIZE; i++) {
        for (int j = 0; j < 2; j++) {
            data_array[i][j] = NULL;
        }
    }
    
    final_total_lines = parse_input_file("mips2.txt");

    Inst** instructions = (Inst**)malloc(final_total_lines * sizeof(Inst*));
    parse_instructions("temp.txt", instructions, final_total_lines);
    
    symbol_table(final_total_lines, instructions);
    print_execute(final_total_lines, instructions);
    execute(final_total_lines, instructions);

    free(instructions);

    return 0;
}
