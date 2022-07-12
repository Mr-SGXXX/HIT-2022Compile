#include <stdio.h>
int errorFlag = 0;

void BASE_ERROR(char type, int lineno, char* Message) {
    printf("Error type %c at line %d: %s\n", type, lineno, Message);
}

void UNDEFIEND_CHAR(char c, int lineno) {
    char str[30];
    sprintf(str, "Mysterious character \"%c\"", c);
    BASE_ERROR('A', lineno, str);
}

void MISSING_SYMBOL(char* s, int lineno) {
    char str[30];
    sprintf(str, "MISSING \"%s\"", s);
    BASE_ERROR('B', lineno, str);
}

void SYNTAX_ERROR(char* s, int lineno) {
    char str[30];
    sprintf(str, "Syntax Error: \"%s\"", s);
    BASE_ERROR('B', lineno, str);
}