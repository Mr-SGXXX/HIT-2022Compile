#include "symbol_table.h"

static HashNode SymbolTable[HASH_TABLE_SIZE];

unsigned int hash(char* name)
{
    unsigned val = 0, i;
    for(; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

void initHashTable()
{
    memset(SymbolTable, 0, sizeof(HashNode) * HASH_TABLE_SIZE);
}

// 填表操作

void insert(SymbolType type, char* name, void* moreMsg)
{
    unsigned int index = hash(name);
    void* msg;
    HashNode* node = &SymbolTable[index];
    HashNode* temp;
    switch(type)
    {
    case Variable:
        msg = calloc(1, sizeof(VarMsg));
        memcpy(msg, moreMsg, sizeof(VarMsg));
        break;
    case Constraint:
        msg = calloc(1, sizeof(ConstMsg));
        memcpy(msg, moreMsg, sizeof(ConstMsg));
        break;
    case Function:
        msg = calloc(1, sizeof(FuncMsg));
        memcpy(msg, moreMsg, sizeof(FuncMsg));
        break;
    case Struct_type:
        msg = calloc(1, sizeof(StructMsg));
        memcpy(msg, moreMsg, sizeof(StructMsg));
        break;
    }
    if (*(int*)&node->sym != 0){
        while (node->next != NULL && node->valid)
            node = node->next;
        if(node->next == NULL)
        {
            node->next = (HashNode*)calloc(1, sizeof(HashNode));
            node = node->next;
        }
    }
    node->valid = 1;
    node->sym.moreMsg = msg;
    node->sym.type = type;
    node->sym.level = level;
    strncpy(node->sym.name, name, 32);
}

void insertFloatVar(char* name, ArrMsg arrayMsg)
{
    VarMsg msg;
    msg.type = Float;
    memcpy(&msg.arrayMsg, &arrayMsg, sizeof(arrayMsg));
    insert(Variable, name, &msg);
}

void insertIntVar(char* name, ArrMsg arrayMsg)
{
    VarMsg msg;
    msg.type = Int;
    memcpy(&msg.arrayMsg, &arrayMsg, sizeof(arrayMsg));
    insert(Variable, name, &msg);
}

void insertStructVar(char* name, char* structName, ArrMsg arrayMsg)
{
    VarMsg msg;
    msg.type = Struct;
    strncpy(msg.structName, structName, 32);
    memcpy(&msg.arrayMsg, &arrayMsg, sizeof(arrayMsg));
    insert(Variable, name, &msg);
}

void insertFloatConst(char* name, float val)
{
    ConstMsg msg;
    msg.type = Float;
    msg.val.f = val;
    insert(Constraint, name, &msg);
}

void insertIntConst(char* name, int val)
{
    ConstMsg msg;
    msg.type = Int;
    msg.val.i = val;
    insert(Constraint, name, &msg);
}

void insertFunction(char* name, VariableType retType, char* retStructName, int arg_num, ArrMsg* arrayMsg, \
    VariableType* args, char(* structNameList)[32], char(* varNameList)[32])
{
    FuncMsg msg;
    msg.arg_num = arg_num;
    msg.ret_type = retType;
    msg.retStructName = (char*)calloc(32, sizeof(char));
    msg.arrayMsg = (ArrMsg*)calloc(arg_num, sizeof(ArrMsg));
    msg.structNameList = (char(*)[32])calloc(arg_num, sizeof(char) * 32);
    msg.args = (VariableType*)calloc(arg_num, sizeof(VariableType));
    msg.varNameList = (char(*)[32])calloc(arg_num, sizeof(char) * 32);
    if (retStructName != NULL) memcpy(msg.retStructName, retStructName, sizeof(char) * 32);
    if (arrayMsg != NULL) memcpy(msg.arrayMsg, arrayMsg, sizeof(ArrMsg) * arg_num);
    if (structNameList != NULL) memcpy(msg.structNameList, structNameList, sizeof(char) * 32 * arg_num);
    if (args != NULL) memcpy(msg.args, args, sizeof(VariableType) * arg_num);
    if (varNameList != NULL) memcpy(msg.varNameList, varNameList, sizeof(char) * 32 * arg_num);
    insert(Function, name, &msg);
}

void insertStructMsg(char* name, int var_num, VariableType* typeList,\
     ArrMsg* arrayMsg,  char(* structNameList)[32], char(* nameList)[32])
{
    StructMsg msg;
    msg.var_num = var_num;
    msg.typeList = (VariableType*)calloc(var_num, sizeof(VariableType));
    msg.arrayMsg = (ArrMsg*)calloc(var_num, sizeof(ArrMsg));
    msg.structNameList = (char(*)[32])calloc(var_num, sizeof(char) * 32);
    msg.nameList = (char(*)[32])calloc(var_num, sizeof(char) * 32);
    memcpy(msg.typeList, typeList, sizeof(VariableType) * var_num);
    memcpy(msg.arrayMsg, arrayMsg, sizeof(ArrMsg) * var_num);
    memcpy(msg.structNameList, structNameList, sizeof(char) * 32 * var_num);
    memcpy(msg.nameList, nameList, sizeof(char) * 32 * var_num);
    insert(Struct_type, name, &msg);
}

// 查表操作

int checkInTable(char* name, Symbol* sym)
{
    int index = hash(name);
    Symbol* best = NULL;
    int best_level = -1;
    HashNode* node = &SymbolTable[index];
    while(node != NULL)
    {
        if(!strcmp(node->sym.name, name) && node->sym.level > best_level && node->valid == 1)
        {
            best_level = node->sym.level;
            best = &(node->sym);
        }
        node = node->next;
    }
    if (best == NULL)
        return 0;
    else
    {
        if (sym != NULL) *sym = *best;
        return 1;
    }
}

void freeTable()
{
    SymbolType type;
    HashNode* node;
    HashNode* temp;
    void* msg;
    int firstFlag;
    for(int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        node = &SymbolTable[i];
        firstFlag = 1;
        if(*(int*)&node->sym == 0) continue;
        while (node != NULL)
        {
            temp = node->next;
            type = node->sym.type;
            msg = node->sym.moreMsg;
            switch(type)
            {
            case Variable:
                free(((VarMsg*)msg)->arrayMsg.dim);
                free((VarMsg*)msg);
                break;
            case Constraint:
                free((ConstMsg*)msg);
                break;
            case Function:
                free(((FuncMsg*)msg)->args);
                free(((FuncMsg*)msg)->retStructName);
                free(((FuncMsg*)msg)->arrayMsg);
                free(((FuncMsg*)msg)->structNameList);
                free(((FuncMsg*)msg)->varNameList);
                free((FuncMsg*)msg);
                break;
            case Struct_type:
                free(((StructMsg*)msg)->arrayMsg);
                free(((StructMsg*)msg)->structNameList);
                free(((StructMsg*)msg)->nameList);
                free(((StructMsg*)msg)->typeList);
                free((StructMsg*)msg);
                break;
            }
            if (firstFlag == 0)
                free(node);
            else firstFlag = 0;
            node = temp;
        }
        
    }
}

void updateTable()
{
    VariableType type;
    HashNode* node;
    void* msg;
    for(int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        node = &SymbolTable[i];
        while (node != NULL)
        {
            if (node->sym.level > level && node->valid == 1)
            {
                node->valid = 0;
                type = node->sym.type;
                msg = node->sym.moreMsg;
                switch(type)
                {
                case Variable:
                    free(((VarMsg*)msg)->arrayMsg.dim);
                    free((VarMsg*)msg);
                    break;
                case Constraint:
                    free((ConstMsg*)msg);
                    break;
                case Function:
                    free(((FuncMsg*)msg)->args);
                    free(((FuncMsg*)msg)->retStructName);
                    free(((FuncMsg*)msg)->arrayMsg);
                    free(((FuncMsg*)msg)->structNameList);
                    free(((FuncMsg*)msg)->varNameList);
                    free((FuncMsg*)msg);
                    break;
                case Struct_type:
                    free(((StructMsg*)msg)->arrayMsg);
                    free(((StructMsg*)msg)->structNameList);
                    free(((StructMsg*)msg)->nameList);
                    free(((StructMsg*)msg)->typeList);
                    free((StructMsg*)msg);
                    break;
                }
            }
            node = node->next;
        }
    }
}