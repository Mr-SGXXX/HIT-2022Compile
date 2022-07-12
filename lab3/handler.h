#include "symbol_table.h"
#include "syntax_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void SEMANTIC_ERROR(int error_num, char* s, int lineno);

void OptTagHandler(TreeNode subTree, char** name);

void StructSpecifierHandler(TreeNode subTree);

void DecHandler(TreeNode subTree);

void DefHandler(TreeNode subTree);

void DefListHandler(TreeNode subTree, int* var_num, VariableType** typeList, \
    ArrMsg** arrayMsg, char(** structNameList)[32], char (** nameList)[32], int** lineNo);

void FunDecHandler(TreeNode subTree, VariableType retType, char* retStructName);

void VarListHandler(TreeNode subTree, int* arg_num, ArrMsg** arrayMsg,\
    VariableType** args, char(** structNameList)[32], char(** varNameList)[32]);

void ParamDecHandler(TreeNode subTree, char valName[32]);

void CompStHandler(TreeNode subTree, char* FuncName);

void StmtListHandler(TreeNode subTree, VariableType retType);

void StmtHandler(TreeNode subTree, VariableType retType);

void ExpHandler(TreeNode subTree);

void ArgsHandler(TreeNode subTree, int* arg_num, VariableType** type, char(** structNameList)[32], ArrMsg** arrayMsg);