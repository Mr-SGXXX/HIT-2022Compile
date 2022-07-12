#include <stdio.h>
#include "syntax_tree.h"
#include "symbol_table.h"

extern void yyrestart (FILE *);
extern int yyparse (void);
extern int errorFlag;
extern TreeNode root;

int main(int argc, char** argv)
{ 
    FILE* fp;
    initHashTable();
    //if(argc <= 1) return 1;
    //printf("Program Start\n");
    char* path = "/home/pi/Code/C/BianYi/lab2/test/20.c";
    if(!(fp = fopen(argv[1],"r"))) {
        fp = fopen(path,"r");
        //perror(argv[1]);
        //return 1;
    }
    yyrestart(fp);
    yyparse();
    //printf("Root address of Tree:%p\n", root);
    //if (!errorFlag) printTree(root);
    freeTree(root);
    freeTable();
    return 0;
}
 