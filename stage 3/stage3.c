#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>   

#define MAX_LINE_LENGTH 256
#define MAX_ARR_SIZE 100

int reg_to_index(const char*);
char* concat_str(const char*, const char*);
char* convert_register(const char*);
char* imm_to_binary(const char*);
char* address_to_binary(const char*);
char** split(char*);

int final_total_lines = 0;
char* data_array[MAX_ARR_SIZE][3];

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

char* concat_str(const char* str1, const char* str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char *result = (char*)malloc(len1 + len2 + 1);

    if (result) {
        strcpy(result, str1);
        strcat(result, str2);
    }

    return result;
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
    else if (!strcmp(str[temp], "lw")) {
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
    else if (!strcmp(str[temp], "print_str")) {
        return_str = (char**)malloc(4 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,4");
        return_str[1] = "li $a0,";
        return_str[1] = concat_str(return_str[1], str[temp+1]);
        return_str[2] = "syscall";
        return_str[3] = "end str";
        return return_str;
    }
    else if (!strcmp(str[temp], "read_str")) {
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,8");
        return_str[1] = "syscall";
        return_str[2] = "end str";
        return return_str;
    }
    else if (!strcmp(str[temp], "print_integer")) {
        return_str = (char**)malloc(4 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,1");
        return_str[1] = "li $a0,<parameter>";
        return_str[1] = concat_str(return_str[1], str[temp+1]);
        return_str[2] = "syscall";
        return_str[3] = "end str";
        return return_str;
    }
    else if (!strcmp(str[temp], "read_integer")) {
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,8");
        return_str[1] = "syscall";
        return_str[2] = "end str";
        return return_str;
    }
    else if (!strcmp(str[temp], "exit")) {
        return_str = (char**)malloc(3 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "li $v0,10");
        return_str[1] = "syscall";
        return_str[2] = "end str";
        return return_str;
    }
    else if (!strcmp(str[temp], "GCD")) {
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
        for (int i = 0; i < 2; i++) {
            data_array[num_data_line][i] = parsed_line[i+1];
        }
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
                if (strstr(to_parse[a], ".text") == NULL && strstr(to_parse[a], ".data") == NULL ) {
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

    int i=-1;
    while (fgets(line, sizeof(line), file)) {
        int len_pseudo = 0;

        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';

        if (i == -1) {
            i++;
            continue;
        }

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
                (strcmp(instructions[i]->mnemonic, "slt") == 0) || (strcmp(instructions[i]->mnemonic, "move") == 0)) {
                instructions[i]->type = "R";
            }
            else if ((strcmp(instructions[i]->mnemonic, "addi") == 0) || (strcmp(instructions[i]->mnemonic, "addiu") == 0)  ||
                (strcmp(instructions[i]->mnemonic, "li") == 0) || (strcmp(instructions[i]->mnemonic, "ori") == 0)) {
                instructions[i]->type = "I";
            }
            else if ((strcmp(instructions[i]->mnemonic, "beq") == 0) || (strcmp(instructions[i]->mnemonic, "bne") == 0) ||
                    (strcmp(instructions[i]->mnemonic, "beqz") == 0)) {
                instructions[i]->type = "branch";
            }
            else if ((strcmp(instructions[i]->mnemonic, "lw") == 0) || (strcmp(instructions[i]->mnemonic, "sw") == 0)) {
                instructions[i]->type = "T"; // transfer
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

    char *temp_str = (char*)malloc(8 * sizeof(char*));
    int temp = 0;

    for (int i=0; i<(num_lines-1); i++) {
        temp = (i+1)*4 + 4194304 - 4;
        snprintf(temp_str, sizeof(temp_str), "%X", temp);
        instructions[i]->address = strdup(temp_str);
        
        if (strcmp(instructions[i]->label, "0") != 0) {
            fprintf(symboltable, "%s", instructions[i]->label);
            fprintf(symboltable, "\t");
            fprintf(symboltable, "0x");
            fprintf(symboltable, "%s", instructions[i]->address);
            fprintf(symboltable, "\n");
        }
    }
    fclose(symboltable);
}

void execute(int num_lines, Inst** instructions) {
    FILE* execute = fopen("execute.txt", "w");
    ftruncate(fileno(execute), 0);

    int regFile[32];

    for (int i = 0; i < 31; i++) {
        regFile[i] = 0;
    }

    char *temp_str = (char*)malloc(33 * sizeof(char*));

    for (int i = 0; i < (num_lines-1); i++) {
        // printf("%d: %s\n", i, instructions[i]->mnemonic);
        // fflush(stdout);

        fprintf(execute, "%d ", i);

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
            
            // opcode and shamrt
            instructions[i]->binaryArr[0] = strdup("000000");
            instructions[i]->binaryArr[4] = strdup("00000");

            // funct
            if (strcmp(instructions[i]->mnemonic, "add") == 0) {
                instructions[i]->binaryArr[5] = "100000";
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] + regFile[reg_to_index(instructions[i]->operands[2])];
            }
            else if (strcmp(instructions[i]->mnemonic, "move") == 0) {         // 20
                instructions[i]->binaryArr[5] = "100000";
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])];
            }
            else if (strcmp(instructions[i]->mnemonic, "addu") == 0)  {    // 21
                instructions[i]->binaryArr[5] = "100001";
                // NO OPERATIONS LOGIC YET
            }
            else if (strcmp(instructions[i]->mnemonic, "sub") == 0) {     // 22
                instructions[i]->binaryArr[5] = "100010";
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] - regFile[reg_to_index(instructions[i]->operands[2])];
            }
            else if (strcmp(instructions[i]->mnemonic, "subu") == 0) {    // 23
                instructions[i]->binaryArr[5] = "100011";
                // NO OPERATIONS LOGIC YET
            }
            else if (strcmp(instructions[i]->mnemonic, "and") == 0) {    // 24
                instructions[i]->binaryArr[5] = "100100";
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] & regFile[reg_to_index(instructions[i]->operands[2])];
            }
            else if (strcmp(instructions[i]->mnemonic, "or") == 0) {      // 25   
                instructions[i]->binaryArr[5] = "100101";
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] | regFile[reg_to_index(instructions[i]->operands[2])];
            }
            else if (strcmp(instructions[i]->mnemonic, "slt") == 0) {     // 2a
                instructions[i]->binaryArr[5] = "101010";

                regFile[reg_to_index(instructions[i]->operands[0])] =
                    (regFile[reg_to_index(instructions[i]->operands[1])] < regFile[reg_to_index(instructions[i]->operands[2])])
                    ? 1 : 0;
            }

            // registers
            instructions[i]->binaryArr[1] = convert_register(instructions[i]->operands[1]);   // rs

            if (strcmp(instructions[i]->mnemonic, "move") == 0)
                instructions[i]->binaryArr[2] = "00000";   // rt
            else
                instructions[i]->binaryArr[2] = convert_register(instructions[i]->operands[2]);   // rt

            instructions[i]->binaryArr[3] = convert_register(instructions[i]->operands[0]);   // rd

            for (int j=0; j<6; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // I type
        else if (strcmp(instructions[i]->type, "I") == 0) {
            instructions[i]->binaryArr = (char**)malloc(4 * sizeof(char*));

            // opcode
            if (strcmp(instructions[i]->mnemonic, "addi") == 0) {           // 8
                instructions[i]->binaryArr[0] = "001000";
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] + atoi(instructions[i]->operands[2]);
            }
            else if (strcmp(instructions[i]->mnemonic, "addiu") == 0) {     // 9
                instructions[i]->binaryArr[0] = "001001";
                // NO OPERATIONS LOGIC YET
            }
            else if (strcmp(instructions[i]->mnemonic, "li") == 0) {    // d
                instructions[i]->binaryArr[0] = "001101";
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    0 | atoi(instructions[i]->operands[1]);
            }
            else if (strcmp(instructions[i]->mnemonic, "ori") == 0) {     // d
                instructions[i]->binaryArr[0] = "001101";
                regFile[reg_to_index(instructions[i]->operands[0])] =
                    regFile[reg_to_index(instructions[i]->operands[1])] | atoi(instructions[i]->operands[2]);
            }

            // operands 
            if (strcmp(instructions[i]->mnemonic, "li") == 0) {
                instructions[i]->binaryArr[2] = convert_register(instructions[i]->operands[0]);
                instructions[i]->binaryArr[1] = "00000";
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
        
            // opcode
            if (strcmp(instructions[i]->mnemonic, "beq") == 0) {            // 4
                instructions[i]->binaryArr[0] = "000100";

                if (regFile[reg_to_index(instructions[i]->operands[0])] == regFile[reg_to_index(instructions[i]->operands[1])]) {
                    regFile[29] = 11111111;    // sp
                    // change i
                    // WIP
                }
                else
                    ; // do nothing
            }
            else if (strcmp(instructions[i]->mnemonic, "beqz") == 0) {
                instructions[i]->binaryArr[0] = "000100";

                if (regFile[reg_to_index(instructions[i]->operands[0])] == 0) {
                    regFile[29] = 11111111;    // sp
                    // change i
                    // WIP
                }
                else
                    ; // do nothing
            }
            else if (strcmp(instructions[i]->mnemonic, "bne") == 0) {       // 5
                instructions[i]->binaryArr[0] = "000101";

                if (regFile[reg_to_index(instructions[i]->operands[0])] != regFile[reg_to_index(instructions[i]->operands[1])]) {
                    regFile[29] = 11111111;    // sp
                    // change i
                    // WIP
                }
                else
                    ; // do nothing
            }

            // operands
            instructions[i]->binaryArr[1] = convert_register(instructions[i]->operands[0]);
            instructions[i]->binaryArr[2] = convert_register(instructions[i]->operands[1]);

            /*for (int j=0; j<num_lines; j++) {
                // if (instructions[j] != NULL && instructions[j]->label != NULL && instructions[i]->operands[2] != NULL) {
                    if (strcmp(instructions[j]->label, instructions[i]->operands[2]) == 0) {
                        int BTA = atoi(instructions[i]->address) - atoi(instructions[j]->address);
                        char BTA_string[16];
                        sprintf(BTA_string, "%d", BTA);
                        instructions[i]->binaryArr[3] = imm_to_binary(BTA_string);

                        continue;
                    }
                // }
                continue;
            }*/

            instructions[i]->binaryArr[3] = "0000000000000000";

            for (int j=0; j<4; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // transfer
        else if (strcmp(instructions[i]->type, "T") == 0) { // transfer
            instructions[i]->binaryArr = (char**)malloc(4 * sizeof(char*));
        
            // opcode
            if (strcmp(instructions[i]->mnemonic, "lw") == 0) {             // 23
                instructions[i]->binaryArr[0] = "100011";
                
                int memAddress = 0;
                memAddress = atoi(instructions[i]->operands[1]) + regFile[reg_to_index(instructions[i]->operands[2])];

                regFile[reg_to_index(instructions[i]->operands[0])] = memAddress;
                // WIP -- need to actually access the value of the mem address first
            }
            else if (strcmp(instructions[i]->mnemonic, "sw") == 0) {        // 2b
                instructions[i]->binaryArr[0] = "101011";

                int memAddress = 0;
                memAddress = atoi(instructions[i]->operands[1]) + regFile[reg_to_index(instructions[i]->operands[2])];

                regFile[reg_to_index(instructions[i]->operands[0])] = memAddress;
                // WIP -- need to actually access the value of the mem address first
            }

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
                instructions[i]->binaryArr[0] = "000010";

                regFile[29] = 11111111;    // sp
                // change i
                // WIP
            }
            else if (strcmp(instructions[i]->mnemonic, "jal") == 0) {     // 9
                instructions[i]->binaryArr[0] = "000011";

                regFile[29] = 11111111;    // sp
                // change i
                // WIP
            }

            // operands
            /* for (int j=0; j<num_lines; j++) {
                if (strcmp(instructions[j]->label, instructions[i]->operands[0]) == 0) {
                    int JTA = atoi(instructions[j]->address);
                    char JTA_string[16];
                    sprintf(JTA_string, "%d", JTA);
                    instructions[i]->binaryArr[1] = address_to_binary(JTA_string);
                    break;
                }
            } */
        
            for (int j=0; j<2; j++) {
                fprintf(execute, "%s", instructions[i]->binaryArr[j]);
            }
            fprintf(execute, "\n");
        }

        // jr
        else if (strcmp(instructions[i]->type, "jr") == 0) {
            instructions[i]->binaryArr = (char**)malloc(4 * sizeof(char*));
        
            for (int j=0; j<4; j++) {
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

    for (int j = 8; j < 16; j++) {
        printf("%d: %d\n", j, regFile[j]);
    }
    printf("%d: %d\n", 24, regFile[24]);
    printf("%d: %d\n", 25, regFile[25]);
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
    char *binary = (char *)malloc(17 * sizeof(char));
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
    
    final_total_lines = parse_input_file("mips2.txt");
    // printf("%d\n", final_total_lines);
    Inst** instructions = (Inst**)malloc(final_total_lines * sizeof(Inst*));
    parse_instructions("temp.txt", instructions, final_total_lines);
    
    symbol_table(final_total_lines, instructions);
    
    execute(final_total_lines, instructions);

    /* printf("LABEL\tTYPE\tMNEMONIC ADDRESS\tOPERANDS\n");
    for (int i=0; i<final_total_lines-1; i++) {
        printf("%s\t%s\t%s\t%s\t", instructions[i]->label, instructions[i]->type, instructions[i]->mnemonic, instructions[i]->address);
        
        for (int j=0; j<3; j++)
            printf("%s ", instructions[i]->operands[j]);
        printf("\n");
    } */

    free(instructions);
    return 0;
}
