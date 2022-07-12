%{
    #include <stdio.h>
    #include "handler.h"
    #include "syntax_tree.h"
    #include "symbol_table.h"
    extern void MISSING_SYMBOL(char* s, int lineno);
    extern void BASE_ERROR(char type, int lineno, char* Message);
    extern void SYNTAX_ERROR(char* s, int lineno);
    extern int yylex (void);
    extern int yylineno;
    extern char* yytext;
    void yyerror(char* msg);
    TreeNode nonTerm(int lineno, char* type);
    TreeNode term(char* val, char* type);
    extern int errorFlag;
    int level = 0;
    TreeNode root;
    StackNode top = NULL;
    int error_pos = -1;
%}

%union {
    int type_int;
    float type_float;
    double type_double;
    char* type_str;
}

%token <type_int> INT 
%token <type_float> FLOAT
%token <type_str> ID TYPE RELOP
%token SEMI COMMA ASSIGNOP PLUS MINUS STAR DIV AND OR DOT NOT LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE

%right ASSIGNOP
%left  OR
%left  AND
%left  RELOP
%left  PLUS MINUS
%left  STAR DIV
%right NOT NEG
%left  DOT LB RB LP RP

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/* High-level Definitions */

Program : ExtDefList {
    TreeNode edl;
    top = pop(top, &edl);
    root = nonTerm(@$.first_line, "Program");
    connectParentNode(edl, root);
}

ExtDefList : {
    TreeNode edl = nonTerm(@$.first_line, NULL);
    top = push(top, edl);
}
    | ExtDef ExtDefList {
    TreeNode ed, edl1;
    top = pop(top, &edl1);
    top = pop(top, &ed);
    TreeNode edl = nonTerm(@$.first_line, "ExtDefList");
    connectLeftNode(edl1, ed);
    connectParentNode(ed, edl);
    top = push(top, edl);
}

ExtDef : Specifier ExtDecList SEMI {
    TreeNode sp, edl;
    top = pop(top, &edl);
    top = pop(top, &sp);
    TreeNode ed = nonTerm(@$.first_line, "ExtDef");
    TreeNode semi = term(NULL, "SEMI");
    connectLeftNode(edl ,sp);
    connectLeftNode(semi, sp);
    connectParentNode(sp, ed);
    top = push(top, ed);
}
    | Specifier ExtDecList error {error_pos = @3.first_line;yyerror("m;");}
    | Specifier ExtDecList error RP SEMI {error_pos = @3.first_line;yyerror("m(");}
    | Specifier ExtDecList error RB SEMI {error_pos = @3.first_line;yyerror("m[");}
    | error SEMI {error_pos = @1.first_line;yyerror("eSpecifier Error");}
    | Specifier error SEMI {error_pos = @2.first_line;yyerror("eExtDecList Error");}
    | Specifier SEMI {
    TreeNode sp;
    top = pop(top, &sp);
    TreeNode ed = nonTerm(@$.first_line, "ExtDef");
    TreeNode semi = term(NULL, "SEMI");
    connectLeftNode(semi, sp);
    connectParentNode(sp, ed);
    top = push(top, ed);
}
    | Specifier FunDec CompSt {
    TreeNode sp, fd, cs;
    top = pop(top, &cs);
    top = pop(top, &fd);
    top = pop(top, &sp);
    TreeNode ed = nonTerm(@$.first_line, "ExtDef");
    connectLeftNode(fd, sp);
    connectLeftNode(cs, sp);
    connectParentNode(sp, ed);
    top = push(top, ed);
    FunDecHandler(fd, sp->msg1.i, (char*)(sp->msg2.p));
    CompStHandler(cs, getTermValue(fd->child));
}
    | Specifier FunDec error RC {error_pos = @3.first_line;yyerror("m{");}
    | error FunDec CompSt {error_pos = @1.first_line;yyerror("eSpecifier Error");}

ExtDecList : VarDec {
    TreeNode vd;
    top = pop(top, &vd);
    TreeNode edl = nonTerm(@$.first_line, "ExtDecList");
    connectParentNode(vd, edl);
    top = push(top, edl);
}
    | VarDec COMMA ExtDecList {
    TreeNode vd, edl1;
    top = pop(top, &edl1);
    top = pop(top, &vd);
    TreeNode edl = nonTerm(@$.first_line, "ExtDecList");
    TreeNode comma = term(NULL, "COMMA");
    connectLeftNode(comma, vd);
    connectLeftNode(edl, vd);
    connectParentNode(vd, edl);
    top = push(top, edl);
}

/* Specifier */

Specifier : TYPE {
    TreeNode type = term($1, "TYPE");
    TreeNode sp = nonTerm(@$.first_line, "Specifier");
    connectParentNode(type, sp);
    top = push(top, sp);
    if(type->val[2] == 'i')
        sp->msg1.i = 0;
    else if(type->val[2] == 'f')
        sp->msg1.i = 1;
}
    | StructSpecifier {
    TreeNode ss;
    top = pop(top, &ss);
    TreeNode sp = nonTerm(@$.first_line, "Specifier");
    connectParentNode(ss, sp);
    top = push(top, sp);
    sp->msg1.i = 2;
    sp->msg2.p = ss->msg1.p;
    StructSpecifierHandler(ss);
    if(((char*)(ss->msg1.p))[0] >= 48 && ((char*)(ss->msg1.p))[0] <= 57)
        free(ss->msg1.p);
}

StructSpecifier : STRUCT OptTag LC DefList RC {
    TreeNode ot, dl;
    top = pop(top, &dl);
    top = pop(top, &ot);
    TreeNode ss = nonTerm(@$.first_line, "StructSpecifier");
    TreeNode st = term(NULL, "STRUCT");
    TreeNode lc = term(NULL, "LC");
    TreeNode rc = term(NULL, "RC");
    connectLeftNode(ot, st);
    connectLeftNode(lc, st);
    connectLeftNode(dl, st);
    connectLeftNode(rc, st);
    connectParentNode(st, ss);
    top = push(top, ss);
    OptTagHandler(ot, ((char**)(&(ss->msg1.p))));
    ss->msg2.i = 1;
}
    | STRUCT OptTag LC DefList error {error_pos = @5.first_line;yyerror("m}");}
    | STRUCT Tag {
    TreeNode tag;
    top = pop(top, &tag);
    TreeNode ss = nonTerm(@$.first_line, "StructSpecifier");
    TreeNode st = term(NULL, "STRUCT");
    connectLeftNode(tag, st);
    connectParentNode(st, ss);
    top = push(top, ss);
    char* structName = getTermValue(tag->child);
    ss->msg2.i = 0;
    ss->msg1.p = (void*)structName;
}

OptTag : {
    TreeNode ot = nonTerm(@$.first_line, NULL);
    top = push(top, ot);
}
    | ID {
    TreeNode i = term($1, "ID");
    TreeNode ot = nonTerm(@$.first_line, "OptTag");
    connectParentNode(i, ot);
    top = push(top, ot);
}

Tag: ID {
    TreeNode i = term($1, "ID");
    TreeNode t = nonTerm(@$.first_line, "Tag");
    connectParentNode(i, t);
    top = push(top, t);
}

/* Declarators */

VarDec : ID {
    TreeNode i = term($1, "ID");
    TreeNode vd = nonTerm(@$.first_line, "VarDec");
    connectParentNode(i, vd);
    top = push(top, vd);
}
    | VarDec LB INT RB {
    TreeNode vd1;
    top = pop(top, &vd1);
    TreeNode vd = nonTerm(@$.first_line, "Tag");
    TreeNode lb = term(NULL, "LB");
    char str[12];
    sprintf(str, "%d", $3);
    TreeNode i = term(str, "INT");
    TreeNode rb = term(NULL, "RB");
    connectLeftNode(lb, vd1);
    connectLeftNode(i, vd1);
    connectLeftNode(rb, vd1);
    connectParentNode(vd1, vd);
    top = push(top, vd);
}
    | VarDec LB INT error {error_pos = @4.first_line;yyerror("m]");}
    | VarDec LB error RB {error_pos = @3.first_line;yyerror("eIndex Error");}

FunDec : ID LP VarList RP {
    TreeNode vl;
    top = pop(top, &vl);
    TreeNode fd = nonTerm(@$.first_line, "FunDec");
    TreeNode id = term($1, "ID");
    TreeNode lp = term(NULL, "LP");
    TreeNode rp = term(NULL, "RP");
    connectLeftNode(lp, id);
    connectLeftNode(vl, id);
    connectLeftNode(rp, id);
    connectParentNode(id, fd);
    top = push(top, fd);
}
    | ID LP RP {
    TreeNode fd = nonTerm(@$.first_line, "FunDec");
    TreeNode id = term($1, "ID");
    TreeNode lp = term(NULL, "LP");
    TreeNode rp = term(NULL, "RP");
    connectLeftNode(lp, id);
    connectLeftNode(rp, id);
    connectParentNode(id, fd);
    top = push(top, fd);
}
    | ID LP error {error_pos = @3.first_line;yyerror("m)");}

VarList : ParamDec COMMA VarList {
    TreeNode pd, vl1;
    top = pop(top, &vl1);
    top = pop(top, &pd);
    TreeNode vl = nonTerm(@$.first_line, "VarList");
    TreeNode comma = term(NULL, "COMMA");
    connectLeftNode(comma, pd);
    connectLeftNode(vl1, pd);
    connectParentNode(pd, vl);
    top = push(top, vl);
}
    | ParamDec {
    TreeNode pd;
    top = pop(top, &pd);
    TreeNode vl = nonTerm(@$.first_line, "VarList");
    connectParentNode(pd, vl);
    top = push(top, vl);
}

ParamDec : Specifier VarDec {
    TreeNode sp, vd;
    top = pop(top, &vd);
    top = pop(top, &sp);
    TreeNode pd = nonTerm(@$.first_line, "ParamDec");
    connectLeftNode(vd, sp);
    connectParentNode(sp, pd);
    top = push(top, pd);
}

/* Statements */

CompSt : LC DefList StmtList RC {
    TreeNode dl, sl;
    top = pop(top, &sl);
    top = pop(top, &dl);
    TreeNode cs = nonTerm(@$.first_line, "CompSt");
    TreeNode lc = term(NULL, "LC");
    TreeNode rc = term(NULL, "RC");
    connectLeftNode(dl, lc);
    connectLeftNode(sl, lc);
    connectLeftNode(rc, lc);
    connectParentNode(lc, cs);
    top = push(top, cs);
}
    | LC DefList error StmtList RC {error_pos = @3.last_line;yyerror("eCompST Syntax Error");}

StmtList : {
    TreeNode sl = nonTerm(@$.first_line, NULL);
    top = push(top, sl);
}
    | Stmt StmtList {
    TreeNode stmt, sl1;
    top = pop(top, &sl1);
    top = pop(top, &stmt);
    TreeNode sl = nonTerm(@$.first_line, "StmtList");
    connectLeftNode(sl1, stmt);
    connectParentNode(stmt, sl);
    top = push(top, sl);
}

Stmt : Exp SEMI {
    TreeNode exp;
    top = pop(top, &exp);
    TreeNode stmt = nonTerm(@$.first_line, "Stmt");
    TreeNode semi = term(NULL, "SEMI");
    connectLeftNode(semi, exp);
    connectParentNode(exp, stmt);
    top = push(top, stmt);
}
    | Exp error {error_pos = @2.first_line;yyerror("m;");}
    | Exp LB error SEMI{error_pos = @3.first_line;yyerror("m]");}
    | CompSt {
    TreeNode cs;
    top = pop(top, &cs);
    TreeNode stmt = nonTerm(@$.first_line, "Stmt");
    connectParentNode(cs, stmt);
    top = push(top, stmt);
}
    | RETURN Exp SEMI {
    TreeNode exp;
    top = pop(top, &exp);
    TreeNode stmt = nonTerm(@$.first_line, "Stmt");
    TreeNode ret = term(NULL, "RETURN");
    TreeNode semi = term(NULL, "SEMI");
    connectLeftNode(exp, ret);
    connectLeftNode(semi, ret);
    connectParentNode(ret, stmt);
    top = push(top, stmt);
}
    | RETURN Exp error {error_pos = @3.first_line;yyerror("m;");}
    | IF LP Exp RP Stmt  %prec LOWER_THAN_ELSE {
    TreeNode exp, stmt1;
    top = pop(top, &stmt1);
    top = pop(top, &exp);
    TreeNode stmt = nonTerm(@$.first_line, "Stmt");
    TreeNode i = term(NULL, "IF");
    TreeNode lp = term(NULL, "LP");
    TreeNode rp = term(NULL, "RP");
    connectLeftNode(lp, i);
    connectLeftNode(exp, i);
    connectLeftNode(rp, i);
    connectLeftNode(stmt1, i);
    connectParentNode(i, stmt);
    top = push(top, stmt);
}
    | IF LP Exp RP Stmt ELSE Stmt {
    TreeNode exp, stmt1, stmt2;
    top = pop(top, &stmt2);
    top = pop(top, &stmt1);
    top = pop(top, &exp);
    TreeNode stmt = nonTerm(@$.first_line, "Stmt");
    TreeNode i = term(NULL, "IF");
    TreeNode lp = term(NULL, "LP");
    TreeNode rp = term(NULL, "RP");
    TreeNode el = term(NULL, "ELSE");
    connectLeftNode(lp, i);
    connectLeftNode(exp, i);
    connectLeftNode(rp, i);
    connectLeftNode(stmt1, i);
    connectLeftNode(el, i);
    connectLeftNode(stmt2, i);
    connectParentNode(i, stmt);
    top = push(top, stmt);
}
    | IF error SEMI {error_pos = @2.first_line;yyerror("m(");}
    | WHILE LP Exp RP Stmt {
    TreeNode exp, stmt1;
    top = pop(top, &stmt1);
    top = pop(top, &exp);
    TreeNode stmt = nonTerm(@$.first_line, "Stmt");
    TreeNode whi = term(NULL, "WHILE");
    TreeNode lp = term(NULL, "LP");
    TreeNode rp = term(NULL, "RP");
    connectLeftNode(lp, whi);
    connectLeftNode(exp, whi);
    connectLeftNode(rp, whi);
    connectLeftNode(stmt1, whi);
    connectParentNode(whi, stmt);
    top = push(top, stmt);
}
    | ID LP Args error SEMI {error_pos = @3.first_line;yyerror("m)");}
    | Exp error SEMI {error_pos = @2.last_line;yyerror("eStmt Syntax Error");}


/* Local Definitions */

DefList : {
    TreeNode dl = nonTerm(@$.first_line, NULL);
    top = push(top, dl);
}
    | Def DefList {
    TreeNode def, dl1;
    top = pop(top, &dl1);
    top = pop(top, &def);
    TreeNode dl = nonTerm(@$.first_line, "DefList");
    connectLeftNode(dl1, def);
    connectParentNode(def, dl);
    top = push(top, dl);
    DefHandler(def);
}

Def : Specifier DecList SEMI {
    TreeNode sp, dl;
    top = pop(top, &dl);
    top = pop(top, &sp);
    TreeNode def = nonTerm(@$.first_line, "Def");
    TreeNode semi = term(NULL, "SEMI");
    connectLeftNode(dl, sp);
    connectLeftNode(semi, sp);
    connectParentNode(sp, def);
    top = push(top, def);

}
    | Specifier error SEMI {error_pos = @2.first_line;yyerror("m;");}

DecList : Dec {
    TreeNode dec;
    top = pop(top, &dec);
    TreeNode dl = nonTerm(@$.first_line, "DecList");
    connectParentNode(dec, dl);
    top = push(top, dl);
    DecHandler(dec);
}
    | Dec COMMA DecList {
    TreeNode dec, dl1;
    top = pop(top, &dl1);
    top = pop(top, &dec);
    TreeNode dl = nonTerm(@$.first_line, "DecList");
    TreeNode comma = term(NULL, "COMMA");
    connectLeftNode(comma, dec);
    connectLeftNode(dl1, dec);
    connectParentNode(dec, dl);
    top = push(top, dl);
    DecHandler(dec);
}

Dec : VarDec {
    TreeNode vd;
    top = pop(top, &vd);
    TreeNode dec = nonTerm(@$.first_line, "Dec");
    connectParentNode(vd, dec);
    top = push(top, dec);
}
    | VarDec ASSIGNOP Exp {
    TreeNode vd, exp;
    top = pop(top, &exp);
    top = pop(top, &vd);
    TreeNode ass = term(NULL, "ASSIGNOP");
    TreeNode dec = nonTerm(@$.first_line, "Dec");
    connectLeftNode(ass, vd);
    connectLeftNode(exp, vd);
    connectParentNode(vd, dec);
    top = push(top, dec);
}

/* Expressions */

Exp : Exp ASSIGNOP Exp {
    TreeNode exp1, exp2;
    top = pop(top, &exp2);
    top = pop(top, &exp1);
    TreeNode ass = term(NULL, "ASSIGNOP");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(ass, exp1);
    connectLeftNode(exp2, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | Exp AND Exp {
    TreeNode exp1, exp2;
    top = pop(top, &exp2);
    top = pop(top, &exp1);
    TreeNode and = term(NULL, "AND");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(and, exp1);
    connectLeftNode(exp2, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | Exp OR Exp {
    TreeNode exp1, exp2;
    top = pop(top, &exp2);
    top = pop(top, &exp1);
    TreeNode or = term(NULL, "OR");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(or, exp1);
    connectLeftNode(exp2, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | Exp RELOP Exp {
    TreeNode exp1, exp2;
    top = pop(top, &exp2);
    top = pop(top, &exp1);
    TreeNode rel = term($2, "RELOP");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(rel, exp1);
    connectLeftNode(exp2, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | Exp PLUS Exp {
    TreeNode exp1, exp2;
    top = pop(top, &exp2);
    top = pop(top, &exp1);
    TreeNode plus = term(NULL, "PLUS");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(plus, exp1);
    connectLeftNode(exp2, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | Exp MINUS Exp {
    TreeNode exp1, exp2;
    top = pop(top, &exp2);
    top = pop(top, &exp1);
    TreeNode min = term(NULL, "MINUS");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(min, exp1);
    connectLeftNode(exp2, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | Exp STAR Exp {
    TreeNode exp1, exp2;
    top = pop(top, &exp2);
    top = pop(top, &exp1);
    TreeNode star = term(NULL, "STAR");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(star, exp1);
    connectLeftNode(exp2, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | Exp DIV Exp {
    TreeNode exp1, exp2;
    top = pop(top, &exp2);
    top = pop(top, &exp1);
    TreeNode div = term(NULL, "DIV");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(div, exp1);
    connectLeftNode(exp2, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | LP Exp RP {
    TreeNode exp1;
    top = pop(top, &exp1);
    TreeNode lp = term(NULL, "LP");
    TreeNode rp = term(NULL, "RP");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(exp1, lp);
    connectLeftNode(rp, lp);
    connectParentNode(lp, exp);
    top = push(top, exp);
}
    | MINUS Exp %prec NEG {
    TreeNode exp1;
    top = pop(top, &exp1);
    TreeNode min = term(NULL, "MINUS");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(exp1, min);
    connectParentNode(min, exp);
    top = push(top, exp);
}
    | NOT Exp {
    TreeNode exp1;
    top = pop(top, &exp1);
    TreeNode not = term(NULL, "NOT");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(exp1, not);
    connectParentNode(not, exp);
    top = push(top, exp);
}
    | ID LP Args RP {
    TreeNode args;
    top = pop(top, &args);
    TreeNode id = term($1, "ID");
    TreeNode lp = term(NULL, "LP");
    TreeNode rp = term(NULL, "RP");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(lp, id);
    connectLeftNode(args, id);
    connectLeftNode(rp, id);
    connectParentNode(id, exp);
    top = push(top, exp);
}
    | ID LP RP {
    TreeNode id = term($1, "ID");
    TreeNode lp = term(NULL, "LP");
    TreeNode rp = term(NULL, "RP");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(lp, id);
    connectLeftNode(rp, id);
    connectParentNode(id, exp);
    top = push(top, exp);
}
    | Exp LB Exp RB {
    TreeNode exp1, exp2;
    top = pop(top, &exp2);
    top = pop(top, &exp1);
    TreeNode lb = term(NULL, "LB");
    TreeNode rb = term(NULL, "RB");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(lb, exp1);
    connectLeftNode(exp2, exp1);
    connectLeftNode(rb, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | Exp DOT ID {
    TreeNode exp1;
    top = pop(top, &exp1);
    TreeNode dot = term(NULL, "DOT");
    TreeNode id = term($3, "ID");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectLeftNode(dot, exp1);
    connectLeftNode(id, exp1);
    connectParentNode(exp1, exp);
    top = push(top, exp);
}
    | ID {
    TreeNode i = term($1, "ID");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectParentNode(i, exp);
    top = push(top, exp);
}
    | INT {
    char str[12];
    sprintf(str, "%d", $1);
    TreeNode i = term(str, "INT");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectParentNode(i, exp);
    top = push(top, exp);
    exp->msg1.i = 0;
}
    | FLOAT {
    char str[12];
    sprintf(str, "%f", $1);
    TreeNode f = term(str, "FLOAT");
    TreeNode exp = nonTerm(@$.first_line, "Exp");
    connectParentNode(f, exp);
    top = push(top, exp);
    exp->msg1.i = 1;
} 

Args : Exp COMMA Args {
    TreeNode args1, exp;
    top = pop(top, &args1);
    top = pop(top, &exp);
    TreeNode comma = term(NULL, "COMMA");
    TreeNode args = nonTerm(@$.first_line, "Args");
    connectLeftNode(comma, exp);
    connectLeftNode(args1, exp);
    connectParentNode(exp, args);
    top = push(top, args);
}
    | Exp {
    TreeNode exp;
    top = pop(top, &exp);
    TreeNode args = nonTerm(@$.first_line, "Args");
    connectParentNode(exp, args);
    top = push(top, args);
}
%%

void yyerror(char* msg) {
    if (msg != NULL)
        if (msg[0] == 'm') MISSING_SYMBOL(msg + 1, error_pos);
        else if (msg[0] == 'e') SYNTAX_ERROR(msg + 1, error_pos);
        //else BASE_ERROR('B', yylineno, msg);
    errorFlag = 1;
}

/*
TreeNode term(char* val, char* type, TreeNode left) {
    char T[12];
    if (val == NULL) sprintf(T, ": %s", val);
    else sprintf(T, "");
    TreeNode n = insertRightNode(T, type, left, NULL);
    return n;
}

TreeNode nonTerm(int lineno, char* type, TreeNode left, TreeNode first_child) {
    char NT[12];
    sprintf(NT, " (%d)", lineno);
    TreeNode n = insertRightNode(NT, type, left, first_child);
    return n;
}
*/

TreeNode term(char* val, char* type) {
    char T[32];
    if (val != NULL) sprintf(T, ": %s", val);
    else sprintf(T, "");
    TreeNode n = makeNode(T, type);
    return n;
}

TreeNode nonTerm(int lineno, char* type) {
    char NT[32];
    sprintf(NT, " (%d)", lineno);
    TreeNode n = makeNode(NT, type);
    return n;
}