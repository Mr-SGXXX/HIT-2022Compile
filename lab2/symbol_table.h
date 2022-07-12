#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define HASH_TABLE_SIZE 16384

extern int level;

typedef enum symbolType
{
    Variable, Constraint, Function, Struct_type
}SymbolType;

typedef enum variableType
{
    Int, Float, Struct
}VariableType;

typedef union variableValue
{
    /* data */
    float f;
    int i;
    void* sVal;
}VariableValue;

typedef struct arrMsg
{
    int valid;
    int dim_size;
    int* dim;
}ArrMsg;

typedef struct symbol
{
    SymbolType type;
    int level;
    char name[32];
    void* moreMsg;
}Symbol;

typedef struct varMsg
{
    VariableType type;
    char structName[32];
    ArrMsg arrayMsg;
}VarMsg;

typedef struct constMsg
{
    VariableType type;
    VariableValue val;
}ConstMsg;

typedef struct funcMsg
{
    int arg_num;
    VariableType ret_type;
    char* retStructName;
    VariableType* args;
    char(* structNameList)[32];
    ArrMsg* arrayMsg;
    char(* varNameList)[32];
}FuncMsg;

typedef struct structMsg
{
    int var_num;
    VariableType* typeList;
    char(* structNameList)[32];
    ArrMsg* arrayMsg;   
    char(* nameList)[32];
}StructMsg;

typedef struct hashNode
{
    Symbol sym;
    int valid;
    struct hashNode* next;
}HashNode;

//哈希函数
unsigned int hash(char* name);

//填表操作
//指针操作都是安全的

void insert(SymbolType type, char* name, void* moreMsg);

void insertFloatVar(char* name, ArrMsg arrayMsg);

void insertIntVar(char* name, ArrMsg arrayMsg);

void insertStructVar(char* name, char* structName, ArrMsg arrayMsg);

void insertFloatConst(char* name, float val);

void insertIntConst(char* name, int val);

void insertFunction(char* name, VariableType retType, char* retStructName, int arg_num, ArrMsg* arrayMsg, \
     VariableType* args, char(* structNameList)[32], char(* varNameList)[32]);

void insertStructMsg(char* name, int var_num, VariableType* typeList, \
    ArrMsg* arrayMsg, char(* structNameList)[32], char(* nameList)[32]);

//查表操作

int checkInTable(char* name, Symbol* sym);

//初始化表
void initHashTable();

//清空表
void freeTable();

//作用域更新
void updateTable();