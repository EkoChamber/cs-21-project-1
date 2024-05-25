#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


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

char** convert_macro(char** str){
    char** return_str;
    int temp = 0;
    char* label;
    
    if (strstr(str[0], ":") != NULL) {
        label = str[0];
        label = concat_str(label, " ");
        temp++;
    }
    else {
        label = "";
        temp = 0;
    }
    
    if (!strcmp(str[temp], "print_str")) {
        return_str = (char**)malloc(5 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "lui $at,4097");
        return_str[1] = "ori $a0,$at,0";
        return_str[2] = "addiu $v0,$0,4";
        return_str[3] = "syscall";
        return_str[4] = "end str";
        return return_str;
    }
    else if (!strcmp(str[temp], "read_str")) {
        return_str = (char**)malloc(6 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "la $a0,");
        return_str[0] = concat_str(return_str[0], str[temp+1]);
        return_str[1] = "li $a1,";
        return_str[1] = concat_str(return_str[1], str[temp+2]);
        return_str[2] = "li $v0,8";
        return_str[3] = "syscall";
        return_str[4] = "end str";
        return return_str;
    }
    else if (!strcmp(str[temp], "print_integer")) {
        return_str = (char**)malloc(5 * sizeof(char*));
        return_str[0] = label;
        return_str[0] = concat_str(return_str[0], "lui $at,4097");
        return_str[1] = "ori $a0,$at,0";
        return_str[2] = "addiu $v0,$0,1";
        return_str[3] = "syscall";
        return_str[4] = "end str";
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
        return_str[7] = "add $a0, $0, $s1";
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
}