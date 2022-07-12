%option noyywrap
%option yylineno

%{
#include <stdio.h>
#include <string.h>
#include "syntax.tab.h"
int yycolumn = 1;
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
extern void UNDEFIEND_CHAR(char c, int lineno);
extern void BASE_ERROR(char type, int lineno, char* Message);
extern int errorFlag;
%}

digit  [0-9]
letter  [_a-zA-Z]

%%

float {
    /* float */
    char* str = (char *)calloc(10, sizeof(char));
    strcpy(str, yytext);
    yylval = (YYSTYPE)str;
    return TYPE;
}

int {
    /* int */ 
    char* str = (char *)calloc(10, sizeof(char));
    strcpy(str, yytext);
    yylval = (YYSTYPE)str;
    return TYPE;
}

if {/* if */ return IF;}

else {/* else */ return ELSE;}

while {/* while */ return WHILE;}

struct {/* struct */ return STRUCT;}

return {/* return */ return RETURN;}

0|[1-9]{digit}* {
    /* 十进制数 */
    yylval = (YYSTYPE)atoi(yytext); 
    return INT;
}

0[0-7]+ {
    /* 八进制数 */
    char* p = yytext + 1;
    int rst = 0;
    while (*p != '\0') {
        rst = rst * 8 + (int)*p - 48;
        p++;
    }
    yylval = (YYSTYPE)rst; 
    return INT;
}

0x({digit}|[a-fA-F])+ {
    /* 十六进制数 */
    char* p = yytext + 2;
    int rst = 0;
    while (*p != '\0') {
        if (*p >= 48 && *p <= 57) rst = rst * 16 + (int)*p - 48;
        else if(*p >= 97 && *p <= 102) rst = rst * 16 + (int)*p - 87;
        else if(*p >= 65 && *p <= 70) rst = rst * 16 + (int)*p - 55;
        p++;
    } 
    yylval = (YYSTYPE)rst; 
    return INT;
}

({digit}*"."{digit}+|{digit}+"."{digit}*)((e|E)("+"|"-")?{digit}+)? {
    /* 浮点数 */
    yylval = (YYSTYPE)(float)atof(yytext); 
    return FLOAT;
} 

{letter}({digit}|{letter})* {
    /* 标识符 */ 
    char* str = (char *)calloc(10, sizeof(char));
    strcpy(str, yytext);
    yylval = (YYSTYPE)str;
    return ID;
}

"+" {/* + */ return PLUS;}

"-" {/* - */ return MINUS;}

"*" {/* * */ return STAR;}

"/" {/* / */ return DIV;}

";" {/* ; */ return SEMI;}

"," {/* , */ return COMMA;}

"=" {/* = */ return ASSIGNOP;}

">" {/* > */ return RELOP;}

"<" {/* < */ return RELOP;}

">=" {/* >= */ return RELOP;}

"<=" {/* <= */ return RELOP;}

"==" {/* == */ return RELOP;}

"!=" {/* != */ return RELOP;}

"&&" {/* AND */ return AND;}

"||" {/* OR */ return OR;}

"." {/* DOT */ return DOT;}

"!" {/* NOT */ return NOT;}

"(" {/* ( */ return LP;}

")" {/* ) */ return RP;}

"[" {/* [ */ return LB;}

"]" {/* ] */ return RB;}

"{" {/* { */ return LC;}

"}" {/* } */ return RC;}

\n {/* 换行符 */ yycolumn = 1;}

\t {/* 制表符 */}

" " {/* 空格 */}

"//"  {
    /* 单行注释 */
    char c = input();
    while (c != '\n') c = input();
}

"/*"  {
    /* 多行注释 */
    char c = input();
    while (1) {
        if (c == '*') {
            c = input();
            if (c == '/') break;
        }
        c = input();
    }
}

. {
    /* 未定义符号情况 */
    UNDEFIEND_CHAR(yytext[0], yylineno);
    errorFlag = 1;
}

%%


