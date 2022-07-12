#include <stdio.h>
#include "syntax_tree.h"
#include "symbol_table.h"
#include "translate.h"

extern void yyrestart (FILE *);
extern int yyparse (void);
extern int errorFlag;
extern TreeNode root;
TempCodeNode startNode;

int main(int argc, char** argv)
{ 
    FILE* fp;
    initHashTable();
    //if(argc <= 1) return 1;
    //printf("Program Start\n");
    char* path = "/home/pi/Code/C/BianYi/lab3/test/3.c";
    if(!(fp = fopen(argv[1],"r"))) {
        fp = fopen(path,"r");
        //perror(argv[1]);
        //return 1;
    }
    yyrestart(fp);
    yyparse();
    //printf("Root address of Tree:%p\n", root);
    //if (!errorFlag) printTree(root);
    startNode = translate(root);
    printTempCode(startNode);
    freeTree(root);
    freeTable();
    freeTempCode(startNode);
    return 0;
}
 