#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256
#define MAX_ARR_SIZE 100

char** split_by_char(char*);

typedef struct instruction_line Inst;
struct instruction_line {
    char* type;
    char* address;
    char* label;
    char* mnemonic;
    char* binary;
    char** operands;
};

char** split_by_char(char* line);
char** check_macro_pseudo(char* str);
void parse_input_file(const char* filename, Inst** instructions);
void symbol_table(int num_lines, Inst** instructions);
void execute(int num_lines, Inst** instructions);
void instructions_to_binary(int num_lines, Inst** instructions);
char *imm_to_binary(const char* decimalString);
char *address_to_binary(const char* decimalString);
char *convert_register(const char* regName);

char** split_by_char(char* line) {
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

char** check_macro_pseudo(char* str){
    char** return_str;

    if (!strcmp(str, "print_str")) {
        // printf("%s\n", str);
        return_str = (char**)malloc(4 * sizeof(char*));
        return_str[0] = "li $v0,4";
        return_str[1] = "li $a0,<parameter>";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else if (!strcmp(str, "read_str")) {
        // printf("%s\n", str);
        return_str = (char**)malloc(3* sizeof(char*));
        return_str[0] = "li $v0,8";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else if (!strcmp(str, "print_integer")) {
        // printf("%s\n", str);
        return_str = (char**)malloc(4 * sizeof(char*));
        return_str[0] = "li $v0,1";
        return_str[1] = "li $a0,<parameter>";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else if (!strcmp(str, "read_integer")) {
        // printf("%s\n", str);
        return_str = (char**)malloc(3* sizeof(char*));
        return_str[0] = "li $v0,8";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else if (!strcmp(str, "exit")) {
        // printf("%s\n", str);
        return_str = (char**)malloc(3* sizeof(char*));
        return_str[0] = "li $v0,8";
        return_str[2] = "syscall";
        return_str[3] = '\0';
        return return_str;
    }
    else {
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

        // macros_pseudo = check_macro_pseudo(split_by_char(lines[i])[0]);
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

        for (int a = 0; a <= len_pseudo; a++) {
            char** parsedInst = (char**)malloc(num_lines * sizeof(char*));
            parsedInst = split_by_char(to_parse[len_pseudo]); 

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
                (strcmp(instructions[i]->mnemonic, "and") == 0) || (strcmp(instructions[i]->mnemonic, "or") == 0) ||
                (strcmp(instructions[i]->mnemonic, "slt") == 0) || (strcmp(instructions[i]->mnemonic, "move") == 0)) {
                instructions[i]->type = "R";
            }
            else if ((strcmp(instructions[i]->mnemonic, "addi") == 0) || (strcmp(instructions[i]->mnemonic, "addiu") == 0)  ||
                (strcmp(instructions[i]->mnemonic, "li") == 0)) {
                instructions[i]->type = "I";
            }
            else if ((strcmp(instructions[i]->mnemonic, "beq") == 0) || (strcmp(instructions[i]->mnemonic, "bne") == 0)) {
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

            for (int j=0; j<1; j++)
                free(parsedInst[j]);
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

void execute(int num_lines, Inst** instructions) {
    FILE* execute = fopen("execute.txt", "w");
    ftruncate(fileno(execute), 0);

    char* output[32];

    for (int i=0; i<num_lines; i++) {
        if (strcmp(instructions[i]->mnemonic, "syscall") == 0)
            instructions[i]->binary = "00000000000000000000000000000000";

        // R type
        else if ((strcmp(instructions[i]->type, "R") == 0) || (strcmp(instructions[i]->mnemonic, "move") == 0)) {
            char* binary[6];
            char* temp[32];

            // opcode and shamt
            binary[0] = "000000";
            binary[4] = "00000";

            // funct
            if ((strcmp(instructions[i]->mnemonic, "add") == 0) || strcmp(instructions[i]->mnemonic, "move") == 0)          // 20
                binary[5] = "100000";
            else if (strcmp(instructions[i]->mnemonic, "sub") == 0)     // 22
                binary[5] = "100010";
            else if (strcmp(instructions[i]->mnemonic, "and") == 0)     // 24
                binary[5] = "100100";
            else if (strcmp(instructions[i]->mnemonic, "or") == 0)      // 25   
                binary[5] = "100101";
            else if (strcmp(instructions[i]->mnemonic, "slt") == 0)     // 2a
                binary[5] = "101010";

            // registers
            binary[1] = convert_register(instructions[i]->operands[1]);   // rs

            if (strcmp(instructions[i]->mnemonic, "move") == 0)
                binary[2] = "00000";   // rt
            else
                binary[2] = convert_register(instructions[i]->operands[2]);   // rt

            binary[3] = convert_register(instructions[i]->operands[0]);   // rd

            strcpy(temp, "");

            for (int j=0; j<6; j++) {
                strcat(temp, binary[j]);
            }

            instructions[i]->binary = strdup(temp);

            for (int k=0; k<6; k++)
                free(binary[k]);
        }

        // I type
        else if (strcmp(instructions[i]->type, "I") == 0) {
            char* binary[4];
            char* temp[32];

            // opcode
            if (strcmp(instructions[i]->mnemonic, "addi") == 0) {           // 8
                binary[0] = "001000";
            }
            else if (strcmp(instructions[i]->mnemonic, "addiu") == 0) {     // 9
                binary[0] = "001001";
            }
            else if (strcmp(instructions[i]->mnemonic, "li") == 0) {     // 9
                binary[0] = "001101";
            }

            // operands 
            if (strcmp(instructions[i]->mnemonic, "li") == 0) {
                binary[2] = convert_register(instructions[i]->operands[0]);
                binary[1] = "00000";
                binary[3] = imm_to_binary(instructions[i]->operands[1]);    // imm
            }
            else {
                binary[1] = convert_register(instructions[i]->operands[1]);     // rs
                binary[2] = convert_register(instructions[i]->operands[0]);     // rt
                binary[3] = imm_to_binary(instructions[i]->operands[2]);    // imm
            }

            strcpy(temp, "");

            for (int j=0; j<4; j++) {
                strcat(temp, binary[j]);
            }

            instructions[i]->binary = strdup(temp);

            for (int k=0; k<4; k++)
                free(binary[k]);
        }

        // branch
        else if (strcmp(instructions[i]->type, "branch") == 0) {
            char* binary[4];
            char* temp[32];

            // opcode
            if (strcmp(instructions[i]->mnemonic, "beq") == 0) {            // 4
                binary[0] = "000100";
            }
            else if (strcmp(instructions[i]->mnemonic, "bne") == 0) {       // 5
                binary[0] = "000101";
            }

            // operands
            binary[1] = convert_register(instructions[i]->operands[0]);
            binary[2] = convert_register(instructions[i]->operands[1]);

            for (int j=0; j<num_lines; j++) {
                if (strcmp(instructions[j]->label, instructions[i]->operands[2]) == 0) {
                    int BTA = instructions[i]->address - instructions[j]->address;
                    char BTA_string[16];
                    sprintf(BTA_string, "%d", BTA);
                    binary[3] = imm_to_binary(BTA_string);

                    continue;
                }
                continue;
            }

            strcpy(temp, "");

            for (int j=0; j<4; j++) {
                strcat(temp, binary[j]);
            }

            instructions[i]->binary = strdup(temp);

            for (int k=0; k<4; k++)
                free(binary[k]);
        }

        // transfer
        else if (strcmp(instructions[i]->type, "T") == 0) { // transfer
            char* binary[4];
            char* temp[32];

            // opcode
            if (strcmp(instructions[i]->mnemonic, "lw") == 0) {             // 23
                binary[0] = "100011";
            }
            else if (strcmp(instructions[i]->mnemonic, "sw") == 0) {        // 2b
                binary[0] = "101011";
            }

            // operands
            binary[1] = convert_register(instructions[i]->operands[2]);     // rs
            binary[2] = convert_register(instructions[i]->operands[0]);     // rt
            binary[3] = imm_to_binary(instructions[i]->operands[1]);    // imm

            strcpy(temp, "");

            for (int j=0; j<4; j++) {
                strcat(temp, binary[j]);
            }

            instructions[i]->binary = strdup(temp);

            for (int k=0; k<4; k++)
                free(binary[k]);
        }

        // jump
        else if (strcmp(instructions[i]->type, "J") == 0) {
            char* binary[2];
            char* temp[32];

            // opcode
            if (strcmp(instructions[i]->mnemonic, "j") == 0) {           // 8
                binary[0] = "000010";
            }
            else if (strcmp(instructions[i]->mnemonic, "jal") == 0) {     // 9
                binary[0] = "000011";
            }

            // operands
            for (int j=0; j<num_lines; j++) {
                if (strcmp(instructions[j]->label, instructions[i]->operands[0]) == 0) {
                    int JTA = instructions[j]->address;
                    char JTA_string[16];
                    sprintf(JTA_string, "%d", JTA);
                    binary[1] = address_to_binary(JTA_string);
                    break;
                }
            }

            strcpy(temp, "");

            for (int j=0; j<2; j++) {
                strcat(temp, binary[j]);
            }

            instructions[i]->binary = strdup(temp);

            for (int k=0; k<2; k++)
                free(binary[k]);
        }

        // jr
        else if (strcmp(instructions[i]->type, "jr") == 0) {
            instructions[i]->binary = "JRJRJR";
        }

        sprintf(output, "%s\n", instructions[i]->binary);
        fprintf(execute, output);
    }
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

char *imm_to_binary(const char* decimalString) {
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
    
    binary[16] = '\0';

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
    FILE* file = fopen("mips.txt", "r");

    int num_lines = 0;
    fscanf(file, "%d", &num_lines);
    
    Inst** instructions;

    instructions = calloc(MAX_ARR_SIZE, MAX_LINE_LENGTH);
    
    parse_input_file("mips.txt", instructions);
    symbol_table(num_lines, instructions);
    execute(num_lines, instructions);

    printf("LABEL\tTYPE\tMNEMONIC\tADDRESS\tOPERANDS\n");
    for (int i=0; i<num_lines; i++) {
        printf("%s\t%s\t%s\t\t%X\t", instructions[i]->label, instructions[i]->type, instructions[i]->mnemonic, instructions[i]->address);
        
        for (int j=0; j<3; j++)
            printf("%s ", instructions[i]->operands[j]);

        printf("\n");
    }

    free(instructions);
    return 0;
}