#include <stdio.h>
#include <string.h>
int errorFlag = 0;

void BASE_ERROR(char type, int lineno, char* Message) {
    errorFlag = 1;
    printf("Error type %c at line %d: %s\n", type, lineno, Message);
}

void UNDEFIEND_CHAR(char c, int lineno) {
    char str[64];
    sprintf(str, "Mysterious character \"%c\"", c);
    BASE_ERROR('A', lineno, str);
}

void MISSING_SYMBOL(char* s, int lineno) {
    char str[64];
    sprintf(str, "MISSING \"%s\"", s);
    BASE_ERROR('B', lineno, str);
}

void SYNTAX_ERROR(char* s, int lineno) {
    char str[64];
    sprintf(str, "Error: \"%s\"", s);
    BASE_ERROR('B', lineno, str);
}

void SEMANTIC_ERROR(int error_num, char* s, int lineno) {
    char str[64];
    char num[8];
    errorFlag = 1;
    sprintf(num, "%d", error_num);
    sprintf(str, "Error: %s", s);
    printf("Error type %s at line %d: %s\n", num, lineno, str);
}