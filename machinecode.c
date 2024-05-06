#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **splitBySpace(char*);
char **splitByComma(char*);

int main() {
    char input[1000];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0'; // remove newline character if present

    machineCode(input);

    return 0;
}

int machineCode(char* line) {
    if (line == ".include \"macros.asm\"" ||
        line == ".data" || line == ".text") {
            printf("%s\n", "WIP");
    }
    else {
        char **spaceSplit = splitBySpace(line);
        char **registers = splitByComma(line);

        // R-type : add, sub, and, or, slt
        if (spaceSplit[0] == "add" || spaceSplit[0] == "sub" || 
            spaceSplit[0] == "and" || spaceSplit[0] == "or" || spaceSplit[0] == "slt") {
            
            char* binary[6];

            binary[0] = "000000";
            binary[4] = "00000";

            // funct - bits 5:0
            if (spaceSplit[0] == "add")             // 20
                binary[5] = "100000";
            else if (spaceSplit[0] == "sub")        // 22
                binary[5] = "100010";
            else if (spaceSplit[0] == "and")        // 24
                binary[5] = "100100";
            else if (spaceSplit[0] == "or")         // 25   
                binary[5] = "100101";
            else if (spaceSplit[0] == "slt")        // 2a
                binary[5] = "101010";

            // registers
            /* for (int i=0; i<3; i++) {
                //
            } */
            
        }

        // I-type : addi, addiu, beq, bne, lw, sw, move
        else if (spaceSplit[0] == "addi" || spaceSplit[0] == "addiu" || 
            spaceSplit[0] == "beq" || spaceSplit[0] == "bne" ||
            spaceSplit[0] == "lw" || spaceSplit[0] == "sw" || spaceSplit[0] == "move") {
            
            char* binary[4];

            // op code - bits 31:26
            if (spaceSplit[0] == "addi")            // 8
                binary[0] = "001000";
            else if (spaceSplit[0] == "addiu")      // 9
                binary[0] = "001001";
            else if (spaceSplit[0] == "beq")        // 4
                binary[0] = "000100";
            else if (spaceSplit[0] == "bne")        // 5   
                binary[0] = "000101";
            else if (spaceSplit[0] == "lw")         // 23
                binary[0] = "100011";
            else if (spaceSplit[0] == "sw")         // 2b
                binary[0] = "101011";
            else if (spaceSplit[0] == "move")       //
                binary[0] = "XXXXXX";
            
            // registers

            // imm
        }

        // J-type : j, jal, jar
        else {
            char* binary[2];
        }
    }
}

char **splitBySpace(char* line) {
    char **splitString = (char **)malloc(5 * sizeof(char *));
    char* token = strtok(line, " ");

    int i = 0;
    while (token != NULL && i < 5) {
        splitString[i] = strdup(token);

        token = strtok(NULL, " ");
        i++;
    }
    splitString[i] = NULL; // NULL terminate the array
    return splitString;
}

char **splitByComma(char* line) {
    char **splitString = (char **)malloc(3 * sizeof(char *));
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
