#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

void parse_line(const char* line) {
    printf("%s\n", line);
}

void parse_input_file(const char* filename) {
    FILE* file = fopen(filename, "r");

    int num_words;
    fscanf(file, "%d", &num_words);

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        parse_line(line);
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    
    parse_input_file(argv[1]);

    return 0;
}
