#include <stdio.h>
#include "syntax_tree.h"

extern void yyrestart (FILE *);
extern int yyparse (void);
extern int errorFlag;
extern TreeNode root;

int main(int argc, char** argv)
{
    FILE* fp;
    if(argc <= 1) return 1;
    if(!(fp = fopen(argv[1],"r"))) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(fp);
    yyparse();
    printf("Root address of Tree:%p\n", root);
    if (!errorFlag) printTree(root);
    freeTree(root);
    return 0;
}
 