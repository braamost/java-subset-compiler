#include <stdio.h>

int yylex(void);
extern FILE *yyin;
extern int flex_error_count;

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Cannot open input file");
            return 1;
        }
        yyin = file;
    }
    yylex();
    if (flex_error_count > 0) {
        fprintf(stderr, "Total lexical errors: %d\n", flex_error_count);
    }
    return 0;
}