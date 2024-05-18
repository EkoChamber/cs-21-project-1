#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>   

#define MAX_LINE_LENGTH 256
#define MAX_ARR_SIZE 100

char* concat_str(const char* str1, const char* str2);
char** split(char*);

int final_total_lines = 0;
char* data_array[MAX_ARR_SIZE][3];

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

        if (to_parse[0] == "not macro") {
            to_parse[0] = strdup(line);
        }
        
        if (strstr(to_parse[0], ".data") != NULL) {
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

        if(i == -1){
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
    int final_total_lines = 0;

    int num_lines = 0;
    fscanf(file, "%d", &num_lines);
    
    Inst** instructions = (Inst**)malloc(num_lines * sizeof(Inst*));
    
    final_total_lines = parse_input_file("mips.txt");
    printf("%d\n", final_total_lines);

    parse_instructions("temp.txt", instructions, final_total_lines);
    
    /*
    symbol_table(final_total_lines, instructions);

    printf("LABEL\tTYPE\tMNEMONIC ADDRESS\n");
    for (int i=0; i<3; i++) {
        printf("%s\t%s\t%s\n", instructions[i]->label, instructions[i]->type, instructions[i]->mnemonic);
    }
    */

    free(instructions);
    return 0;
}
