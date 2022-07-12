#include "handler.h"

static int noNameStruct;

void OptTagHandler(TreeNode subTree, char** name)
{
    if (subTree->child != NULL)
    {
        *name = getTermValue(subTree->child);
    }
    else
    {
        *name = (char*)calloc(32, sizeof(char));
        sprintf(*name, "%dNoNameStruct", noNameStruct++);
    }
}

void StructSpecifierHandler(TreeNode subTree)
{
    char* name = NULL;
    int var_num;
    VariableType* typeList;
    ArrMsg* arrayMsg;
    char(* structNameList)[32];
    char(* nameList)[32];
    int* lineNo;
    TreeNode node = subTree->child;
    Symbol sym;
    int repeat = 0;
    if (subTree->msg2.i == 1)   //定义性质
    {
        node = node->right->right->right;
        level += 1;
        DefListHandler(node, &var_num, &typeList, \
            &arrayMsg, &structNameList, &nameList, &lineNo);
        level -= 1;
        //updateTable();
        name = subTree->msg1.p;
        if (checkInTable(name, &sym) && sym.level == level)
        {
            char err[64];
            sprintf(err, "Duplicated name \"%s\"", subTree->msg1.p);
            SEMANTIC_ERROR(16, err, getNonTermLineNo(subTree));
        }
        for (int i = 0; i < var_num; i++)
            for (int j = i + 1; j < var_num; j++)
            {
                if(!strcmp(nameList[i], nameList[j]))
                {
                    char err[64];
                    sprintf(err, "Redefined field \"%s\"", nameList[j]);
                    SEMANTIC_ERROR(15, err, lineNo[j]);
                    repeat = 1;
                }
            }
        if (!repeat) insertStructMsg(name, var_num, typeList, arrayMsg, structNameList, nameList);
        free(typeList);
        free(arrayMsg);
        free(structNameList);
        free(nameList);
        free(lineNo);
    }
    else    //非定义性质
    {
        if(!checkInTable(subTree->msg1.p, NULL))
        {
            char err[64];
            sprintf(err, "Undefined Structure \"%s\"", subTree->msg1.p);
            SEMANTIC_ERROR(17, err, getNonTermLineNo(subTree));
        }
    }
}

void DecHandler(TreeNode subTree)
{
    int dim_size = 0;
    TreeNode node = subTree->child;
    TreeNode exp = NULL; 
    if(node->right != NULL) //定义时赋值
    {
        exp = node->right->right;
        subTree->msg3.i = exp->msg1.i;
    }
    else 
        subTree->msg3.i = -1;
    node = node->child;
    subTree->msg1.p = calloc(32, sizeof(char));
    subTree->msg2.p = calloc(1, sizeof(ArrMsg));
    while(node->right != NULL)
    {
        dim_size += 1;
        node = node->child;
    }
    strncpy((char*)(subTree->msg1.p), getTermValue(node), 32);
    int i = 0;
    if (dim_size == 0)
    {
        ((ArrMsg*)(subTree->msg2.p))->valid = 0;
    }
    else
    {
        ((ArrMsg*)(subTree->msg2.p))->valid = 1;
        ((ArrMsg*)(subTree->msg2.p))->dim_size = dim_size;
        ((ArrMsg*)(subTree->msg2.p))->dim = (int*)calloc(dim_size, sizeof(int));
        node = subTree->child->child;
        while(node->right != NULL)
        {
            int num = atoi(getTermValue(node->right->right));
            ((ArrMsg*)(subTree->msg2.p))->dim[i] = num;
            i++;
            node = node->child;
        }
    }
}

void DefHandler(TreeNode subTree)
{
    TreeNode node = subTree->child->right->child;
    int n = 0;
    while(node->right != NULL)
    {
        n += 1;
        node = node->right->right->child;
    }
    n += 1;
    subTree->msg1.i = n;
    subTree->msg2.p = calloc(n, sizeof(char) * 32);
    subTree->msg3.p = calloc(n, sizeof(ArrMsg));
    node = subTree->child->right->child;
    int i = 0;
    while(node->right != NULL)
    {
        if(!(node->msg3.i == -1 || node->msg3.i == subTree->child->msg1.i))
        {
            char err[64];
            sprintf(err, "Type mismatched for assignment", "");
            SEMANTIC_ERROR(5, err, getNonTermLineNo(node));
        }
        strncpy(((char(*)[32])(subTree->msg2.p))[i], (char *)(node->msg1.p), 32);
        memcpy(subTree->msg3.p, node->msg2.p, sizeof(ArrMsg));
        free((char(*)[32])(node->msg1.p));
        free((ArrMsg*)(node->msg2.p));
        i++;
        node = node->right->right->child;
    }
    if(!(node->msg3.i == -1 || node->msg3.i == subTree->child->msg1.i))
        {
            char err[64];
            sprintf(err, "Type mismatched for assignment", "");
            SEMANTIC_ERROR(5, err, getNonTermLineNo(node));
        }
    strncpy(((char(*)[32])(subTree->msg2.p))[i], (char *)(node->msg1.p), 32);
    memcpy(subTree->msg3.p, node->msg2.p, sizeof(ArrMsg));
    free((char(*)[32])(node->msg1.p));
    free((ArrMsg*)(node->msg2.p));
}

void DefListHandler(TreeNode subTree, int* var_num, VariableType** typeList, \
    ArrMsg** arrayMsg, char(** structNameList)[32], char (** nameList)[32], int** lineNo)
{
    TreeNode node = subTree;
    int n = 0;
    while(node->child != NULL)
    {
        node = node->child;
        n += node->msg1.i;
        node = node->right;
    }
    *typeList = (VariableType*)calloc(n, sizeof(VariableType));
    *arrayMsg = (ArrMsg*)calloc(n, sizeof(ArrMsg));
    *structNameList = (char(*)[32])calloc(n, sizeof(char) * 32);
    *nameList = (char(*)[32])calloc(n, sizeof(char) * 32);
    *lineNo = (int*)calloc(n, sizeof(int));
    *var_num = n;
    int i = 0;
    node = subTree;
    while(node->child != NULL)
    {
        node = node->child;
        for (int j = 0; j < node->msg1.i; j++, i++)
        {
            (*typeList)[i] = node->child->msg1.i;    //对应变量的类型
            if ((*typeList)[i] == Struct) strncpy((*structNameList)[i], (char*)(node->child->msg2.p), 32);
            strncpy((*nameList)[i], ((char(*)[32])(node->msg2.p))[j], 32);  //对应变量名
            (*lineNo)[i] = getNonTermLineNo(node->child);
            memcpy((*arrayMsg) + i,((ArrMsg*)(node->msg3.p)) + j, sizeof(ArrMsg));  //对应数组信息
        }
        free((char(*)[32])(node->msg2.p));
        free((ArrMsg*)(node->msg3.p));
        node = node->right;
    }
}

void FunDecHandler(TreeNode subTree, VariableType retType, char* retStructName)
{
    TreeNode node = subTree->child;
    VariableType* args = NULL;
    ArrMsg* arrayMsg = NULL;
    char(* varNameList)[32] = NULL;
    int arg_num = 0;
    char(* structNameList)[32] = NULL;
    char* name = getTermValue(node);
    Symbol sym;
    if(checkInTable(name, &sym) && sym.level == level)
    {
        char err[64];
        sprintf(err, "Redefined function \"%s\"", name);
        SEMANTIC_ERROR(4, err, getNonTermLineNo(subTree));
    }
    if (node->right->right->right == NULL)
    {
        insertFunction(name, retType, retStructName, arg_num, arrayMsg, args, structNameList, varNameList);
    }
    else
    {
        VarListHandler(node->right->right, &arg_num, &arrayMsg, &args, &structNameList, &varNameList);
        insertFunction(name, retType, retStructName, arg_num, arrayMsg, args, structNameList, varNameList);
        free(args);
        free(arrayMsg);
        free(structNameList);
        free(varNameList);
    }
}

void CompStHandler(TreeNode subTree, char* FuncName)
{
    int var_num;
    TreeNode node = subTree->child;
    VariableType* typeList;
    ArrMsg* arrayMsg;
    char(* structNameList)[32];
    char(* nameList)[32];
    int* lineNo;
    int repeat = 0;
    Symbol sym;
    level += 1;
    if(FuncName != NULL)
    {
        checkInTable(FuncName, &sym);
        int arg_num = ((FuncMsg*)(sym.moreMsg))->arg_num;
        for(int i = 0; i < arg_num; i++)
        {
            switch(((FuncMsg*)(sym.moreMsg))->args[i])
            {
            case Int:
                insertIntVar(((FuncMsg*)(sym.moreMsg))->varNameList[i], \
                 ((FuncMsg*)(sym.moreMsg))->arrayMsg[i]);
                break;
            case Float:
                insertFloatVar(((FuncMsg*)(sym.moreMsg))->varNameList[i],\
                 ((FuncMsg*)(sym.moreMsg))->arrayMsg[i]);
                break;
            case Struct:
                insertStructVar(((FuncMsg*)(sym.moreMsg))->varNameList[i],\
                 ((FuncMsg*)(sym.moreMsg))->structNameList[i], ((FuncMsg*)(sym.moreMsg))->arrayMsg[i]);
                break;
            }
        }
    }
    DefListHandler(node->right, &var_num, &typeList, &arrayMsg, &structNameList, &nameList, &lineNo);
    for(int i = 0; i < var_num; i++)
    {
        repeat = 0;
        if (checkInTable(nameList[i], &sym) && sym.level == level)
        {
            char err[64];
            sprintf(err, "Redefined variable \"%s\"", nameList[i]);
            SEMANTIC_ERROR(3, err, lineNo[i]);
            repeat = 1;
        }
        if (!repeat)
            switch(typeList[i])
            {
            case Int:
                insertIntVar(nameList[i], arrayMsg[i]);
                break;
            case Float:
                insertFloatVar(nameList[i], arrayMsg[i]);
                break;
            case Struct:
                insertStructVar(nameList[i], structNameList[i], arrayMsg[i]);
                break;
            }
    }
    StmtListHandler(node->right->right, ((FuncMsg*)(sym.moreMsg))->ret_type);
    level -= 1;
    //updateTable();
}

void StmtListHandler(TreeNode subTree, VariableType retType)
{
    TreeNode node = subTree->child;
    while(node != NULL)
    {
        StmtHandler(node, retType);
        node = node->right->child;
    }
}

void StmtHandler(TreeNode subTree, VariableType retType)
{
    TreeNode stNode = subTree->child;
    while(stNode != NULL)
    {
        if(!strcmp(stNode->type, "Exp"))
            ExpHandler(stNode);
        else if (!strcmp(stNode->type, "CompSt"))
            CompStHandler(stNode, NULL);
        else if (!strcmp(stNode->type, "Stmt"))
            StmtHandler(stNode, retType);
        else if(!strcmp(stNode->type, "RETURN"))
        {
            stNode = stNode->right;
            ExpHandler(stNode);
            if(stNode->msg1.i != retType)
            {
                char err[64];
                sprintf(err, "Type mismatched for return", "");
                SEMANTIC_ERROR(8, err, getNonTermLineNo(stNode));
            }
        }
        stNode = stNode->right;
    }
}

void VarListHandler(TreeNode subTree, int* arg_num, ArrMsg** arrayMsg,\
    VariableType** args, char(** structNameList)[32], char(** varNameList)[32])
{
    TreeNode node = subTree->child;
    int n = 1;
    while(node->right != NULL)
    {
        n += 1;
        node = node->right->right->child;
    }
    *arg_num = n;
    *varNameList = (char(*)[32])calloc(n, sizeof(char) * 32);
    *arrayMsg = (ArrMsg*)calloc(n, sizeof(ArrMsg));
    *args = (VariableType*)calloc(n, sizeof(VariableType));
    *structNameList = (char(*)[32])calloc(n, sizeof(char) * 32);
    int i = 0;
    node = subTree->child;
    while(node->right != NULL)
    {
        ParamDecHandler(node,(*varNameList)[i]);
        (*args)[i] = (VariableType)(node->msg1.i);
        strncpy((*structNameList)[i], (char*)(node->msg2.p), 32);
        memcpy((*arrayMsg) + i,(ArrMsg*)(node->msg3.p), sizeof(ArrMsg));
        i += 1;
        free(node->msg3.p);
        node = node->right->right->child;
    }
    ParamDecHandler(node,(*varNameList)[i]);
    (*args)[i] = (VariableType)(node->msg1.i);
    if (node->msg2.p != NULL) strncpy((*structNameList)[i], (char*)(node->msg2.p), 32);
    memcpy((*arrayMsg) + i,(ArrMsg*)(node->msg3.p), sizeof(ArrMsg));
    free(node->msg3.p);
}

void ParamDecHandler(TreeNode subTree, char valName[32])
{
    TreeNode node = subTree->child;
    int dim_size = 0;
    subTree->msg1.i = node->msg1.i;
    if (subTree->msg1.i == 2) subTree->msg2.p = node->msg2.p;
    else subTree->msg2.p = NULL;
    node = node->right->child;
    subTree->msg3.p = calloc(1, sizeof(ArrMsg));
    ArrMsg* arrMsg = subTree->msg3.p;
    while(node->right != NULL)
    {
        dim_size += 1;
        node = node->child;
    }
    int i = 0;
    if (dim_size == 0)
    {
        node = subTree->child->right->child;
        strncpy(valName, getTermValue(node), 32);
        arrMsg->valid = 0;
        subTree->msg4.p = getTermValue(node);
    }
    else
    {
        node = subTree->child->right->child;
        arrMsg->dim_size = dim_size;
        arrMsg->valid = 1;
        arrMsg->dim = (int*)calloc(dim_size, sizeof(int));
        while(node->right != NULL)
        {
            arrMsg->dim[i] = atoi(getTermValue(node->right->right));
            i++;
            node = node->child;
        }
        strncpy(valName, getTermValue(node), 32);
        subTree->msg4.p = getTermValue(node);
    }
}

//node 生成 Exp ASSIGNOP Exp
void _ExpHandler_assign(TreeNode subTree) 
{
    TreeNode node = subTree;
    node = node->child;
    ExpHandler(node);
    ExpHandler(node->right->right);
    if(node->msg1.i != node->right->right->msg1.i && node->msg1.i != -1)
    {
        char err[64];
        sprintf(err, "Type mismatched for assignment", "");
        SEMANTIC_ERROR(5, err, getNonTermLineNo(node));
        return;
    }
    if(node->msg1.i == 2 && strcmp((char*)(node->msg2.p),(char*)(node->right->right->msg2.p)))
    {
        char err[64];
        sprintf(err, "Type mismatched for assignment", "");
        SEMANTIC_ERROR(5, err, getNonTermLineNo(node));
        return;
    }
    if(node->msg4.p != NULL && node->right->right->msg4.p == NULL || \
        node->right->right->msg4.p != NULL && node->msg4.p == NULL)
    {
        char err[64];
        sprintf(err, "Type mismatched for assignment", "");
        SEMANTIC_ERROR(5, err, getNonTermLineNo(node));
        return;
    }
    if(node->msg4.p != NULL && node->right->right->msg4.p != NULL && \
        ((ArrMsg*)(node->msg4.p))->dim_size != ((ArrMsg*)(node->right->right->msg4.p))->dim_size)
    {
        char err[64];
        sprintf(err, "Type mismatched for assignment", "");
        SEMANTIC_ERROR(5, err, getNonTermLineNo(node));
        return;
    }
    if(node->msg3.p == NULL)
    {
        char err[64];
        sprintf(err, "The left-hand side of an assignment must be a variable", "");
        SEMANTIC_ERROR(6, err, getNonTermLineNo(node));
        return;
    }
}

//node 生成 Exp RELOP/OR/AND Exp
void _ExpHandler_Lop(TreeNode subTree) 
{
    TreeNode node = subTree;
    node = node->child;
    ExpHandler(node);
    ExpHandler(node->right->right);
    if(!strcmp("RELOP", node->right->type))
    {
        if((node->msg1.i != node->right->right->msg1.i || node->msg1.i == Struct) && node->msg1.i != -1)
        {
            char err[64];
            sprintf(err, "Type mismatched for compare operation", "");
            SEMANTIC_ERROR(7, err, getNonTermLineNo(node));
            return;
        }
        if(!(node->msg4.p == NULL && node->right->right->msg4.p == NULL))
        {
            char err[64];
            sprintf(err, "Type mismatched for compare operation", "");
            SEMANTIC_ERROR(7, err, getNonTermLineNo(node));
            return;
        }
    }
    else
    {
        if((node->msg1.i != node->right->right->msg1.i || node->msg1.i == Int) && node->msg1.i != -1)
        {
            char err[64];
            sprintf(err, "Type mismatched for logical operation", "");
            SEMANTIC_ERROR(7, err, getNonTermLineNo(node));
            return;
        }
        if(!(node->msg4.p == NULL && node->right->right->msg4.p == NULL))
        {
            char err[64];
            sprintf(err, "Type mismatched for logical operation", "");
            SEMANTIC_ERROR(7, err, getNonTermLineNo(node));
            return;
        }
    }
    subTree->msg1.i = Int;
}

//node 生成 Exp DIV/STAR/MINUS/PLUS Exp
void _ExpHandler_Aop(TreeNode subTree) 
{
    TreeNode node = subTree;
    node = node->child;
    ExpHandler(node);
    ExpHandler(node->right->right);
    if((node->msg1.i != node->right->right->msg1.i || node->msg1.i == Struct) && node->msg1.i != -1)
    {
        char err[64];
        sprintf(err, "Type mismatched for operands", "");
        SEMANTIC_ERROR(7, err, getNonTermLineNo(node));
        return;
    }
    if(!(node->msg4.p == NULL && node->right->right->msg4.p == NULL))
    {
        char err[64];
        sprintf(err, "Type mismatched for operands", "");
        SEMANTIC_ERROR(7, err, getNonTermLineNo(node));
        return;
    }
    subTree->msg1.i = node->msg1.i;
}

//node 生成 LP Exp RP
void _ExpHandler_LP(TreeNode subTree) 
{
    TreeNode node = subTree;
    node = node->child->right;
    ExpHandler(node);
    subTree->msg1.i = node->msg1.i;
    subTree->msg2.p = node->msg2.p;
    subTree->msg3.p = node->msg3.p;
}

//node 生成 MINUS Exp
void _ExpHandler_neg(TreeNode subTree) 
{
    TreeNode node = subTree;
    node = node->child;
    ExpHandler(node->right);
}

//node 生成 NOT Exp
void _ExpHandler_not(TreeNode subTree) 
{
    TreeNode node = subTree;
    node = node->child;
    ExpHandler(node->right);
}

//node 生成 ID LP Args RP
void _ExpHandler_func1(TreeNode subTree) 
{
    TreeNode node = subTree->child;
    Symbol sym;
    if(!checkInTable(getTermValue(node), &sym))
    {
        char err[64];
        sprintf(err, "Undefined function \"%s\"", getTermValue(subTree->child));
        SEMANTIC_ERROR(2, err, getNonTermLineNo(subTree));
        return;
    }
    subTree->msg1.i = ((FuncMsg*)(sym.moreMsg))->ret_type;
    if(subTree->msg1.i == 2) subTree->msg2.p = ((FuncMsg*)(sym.moreMsg))->retStructName;
    if(sym.type != Function)
    {
        char err[64];
        sprintf(err, "\"%s\" is not a function", getTermValue(subTree->child));
        SEMANTIC_ERROR(11, err, getNonTermLineNo(subTree));
        return;
    }
    int arg_num;
    ArrMsg* arrayMsg;
    VariableType * type;
    char(* structNameList)[32];
    node = node->right->right;
    ArgsHandler(node, &arg_num, &type, &structNameList, &arrayMsg);
    if(arg_num == ((FuncMsg*)(sym.moreMsg))->arg_num)
    {
        for(int i = 0; i < arg_num; i++)
        {
            if(type[i] != ((FuncMsg*)(sym.moreMsg))->args[i])
            {
                char err[64];
                sprintf(err, "Function \"%s\" arguments wrong", getTermValue(subTree->child));
                SEMANTIC_ERROR(9, err, getNonTermLineNo(subTree));
            }
            if(type[i] == Struct && strcmp(structNameList[i], ((FuncMsg*)(sym.moreMsg))->structNameList[i]))
            {
                char err[64];
                sprintf(err, "Function \"%s\" arguments wrong", getTermValue(subTree->child));
                SEMANTIC_ERROR(9, err, getNonTermLineNo(subTree));
            }
            if(((FuncMsg*)(sym.moreMsg))->arrayMsg[i].dim_size != arrayMsg[i].dim_size)
            {
                char err[64];
                sprintf(err, "Function \"%s\" arguments wrong", getTermValue(subTree->child));
                SEMANTIC_ERROR(9, err, getNonTermLineNo(subTree));
            }
        }
    }
    else
    {
        char err[64];
        sprintf(err, "Function \"%s\" arguments wrong", getTermValue(subTree->child));
        SEMANTIC_ERROR(9, err, getNonTermLineNo(subTree));
    }
    subTree->msg1.i = ((FuncMsg*)(sym.moreMsg))->ret_type;
    if (subTree->msg1.i == Struct)
        subTree->msg2.p = ((FuncMsg*)(sym.moreMsg))->retStructName;
}

//node 生成 ID LP RP
void _ExpHandler_func2(TreeNode subTree) 
{
    TreeNode node = subTree;
    Symbol sym;
    node = node->child;
    if(!checkInTable(getTermValue(node), &sym))
    {
        char err[64];
        sprintf(err, "Undefined function \"%s\"", getTermValue(node));
        SEMANTIC_ERROR(2, err, getNonTermLineNo(subTree));
        return;
    }
    subTree->msg1.i = ((FuncMsg*)(sym.moreMsg))->ret_type;
    if(subTree->msg1.i == Struct) subTree->msg2.p = ((FuncMsg*)(sym.moreMsg))->retStructName;
    if(sym.type != Function)
    {
        char err[64];
        sprintf(err, "\"%s\" is not a function", getTermValue(node));
        SEMANTIC_ERROR(11, err, getNonTermLineNo(subTree));
        return;
    }
    if(((FuncMsg*)(sym.moreMsg))->arg_num != 0)
    {
        char err[64];
        sprintf(err, "Function \"%s\" arguments wrong", getTermValue(node));
        SEMANTIC_ERROR(9, err, getNonTermLineNo(subTree));
        return;
    }
    subTree->msg1.i = ((FuncMsg*)(sym.moreMsg))->ret_type;
}

//node 生成 Exp LB Exp RB 
void _ExpHandler_arr(TreeNode subTree) 
{
    TreeNode node = subTree->child;
    Symbol sym;
    ExpHandler(node);
    ExpHandler(node->right->right);
    ArrMsg* arrMsg = (ArrMsg*)(node->msg4.p);
    subTree->msg1.i = subTree->child->msg1.i;
    subTree->msg2.p = subTree->child->msg2.p;
    subTree->msg3.p = subTree->child->msg3.p;
    checkInTable((char*)(node->msg3.p), &sym);
    if(!((VarMsg*)(sym.moreMsg))->arrayMsg.valid)
    {
        char err[64];
        sprintf(err, "\"%s\" is not an array", (char*)(node->msg3.p));
        SEMANTIC_ERROR(10, err, getNonTermLineNo(node));
        return;
    }
    if(arrMsg == NULL)
    {
        char err[64];
        sprintf(err, "\"%s\" has a wrong dim size", (char*)(node->msg3.p));
        SEMANTIC_ERROR(10, err, getNonTermLineNo(node));
        return;
    }
    arrMsg->dim_size -= 1;
    if (arrMsg->dim_size == 0) 
    {
        free(arrMsg);
        arrMsg = NULL;
    }
    subTree->msg4.p = arrMsg;
    node = node->right->right;
    if(node->msg1.i != Int)
    {
        char err[64];
        sprintf(err, "Array index is not an integer", "");
        SEMANTIC_ERROR(12, err, getNonTermLineNo(node));
        return;
    }
}

//node 生成 Exp DOT ID 
void _ExpHandler_dot(TreeNode subTree) 
{
    TreeNode node = subTree;
    Symbol sym;
    int i;
    node = node->child;
    ExpHandler(node);
    if (node->msg1.i == 2 && node->msg4.p == NULL)
    {
        checkInTable((char*)(node->msg2.p), &sym);
        int n = ((StructMsg*)(sym.moreMsg))->var_num;
        char(* nameList)[32] = ((StructMsg*)(sym.moreMsg))->nameList;
        for (i = 0; i < ((StructMsg*)(sym.moreMsg))->var_num; i++)
        {
            if(!strcmp(nameList[i], getTermValue(node->right->right))) 
            {
                subTree->msg1.i = ((StructMsg*)(sym.moreMsg))->typeList[i];
                if(subTree->msg1.i == Struct)
                    subTree->msg2.p = ((StructMsg*)(sym.moreMsg))->structNameList[i];
                subTree->msg3.p = node->msg3.p;
                break;
            }
        }
        if (i == ((StructMsg*)(sym.moreMsg))->var_num)
        {
            char err[64];
            sprintf(err, "Non-existent field \"%s\"", getTermValue(node->right->right));
            SEMANTIC_ERROR(14, err, getNonTermLineNo(node));
            subTree->msg1.i = -1;
        }
    }
    else
    {
        char err[64];
        sprintf(err, "Illegal use of \"%s\"", ".");
        SEMANTIC_ERROR(13, err, getNonTermLineNo(node));
    }
}

//node 生成 ID
void _ExpHandler_id(TreeNode subTree) 
{
    TreeNode node = subTree;
    Symbol sym;
    if (!checkInTable(getTermValue(node->child), &sym))
    {
        char err[64];
        sprintf(err, "Undefined variable \"%s\"", getTermValue(node->child));
        SEMANTIC_ERROR(1, err, getNonTermLineNo(node));
        node->msg3.p = "";
        node->msg1.i = -1;
        return;
    }
    node->msg1.i = ((VarMsg*)sym.moreMsg)->type;
    if (((VarMsg*)(sym.moreMsg))->type == 2)
        node->msg2.p = ((VarMsg*)(sym.moreMsg))->structName;
    node->msg3.p = getTermValue(node->child);
    if(((VarMsg*)(sym.moreMsg))->arrayMsg.valid)
    {
        node->msg4.p = calloc(1, sizeof(ArrMsg)); // dim不重要
        memcpy(node->msg4.p, &(((VarMsg*)(sym.moreMsg))->arrayMsg), sizeof(ArrMsg));
    }
    else node->msg4.p = NULL;
}

void ExpHandler(TreeNode subTree)
{
    TreeNode node = subTree->child;
    if(!strcmp("Exp", node->type))
    {
        node = node->right;
        if(!strcmp("ASSIGNOP", node->type))
            _ExpHandler_assign(subTree);
        else if(!strcmp("AND", node->type))
            _ExpHandler_Lop(subTree);
        else if(!strcmp("OR", node->type))
            _ExpHandler_Lop(subTree);
        else if(!strcmp("RELOP", node->type))
            _ExpHandler_Lop(subTree);
        else if(!strcmp("PLUS", node->type))
            _ExpHandler_Aop(subTree);
        else if(!strcmp("MINUS", node->type))
            _ExpHandler_Aop(subTree);
        else if(!strcmp("STAR", node->type))
            _ExpHandler_Aop(subTree);
        else if(!strcmp("DIV", node->type))
            _ExpHandler_Aop(subTree);
        else if(!strcmp("LB", node->type))
            _ExpHandler_arr(subTree);
        else if(!strcmp("DOT", node->type))
            _ExpHandler_dot(subTree);
    }
    else
    {
        if(!strcmp("ID", node->type))
        {
            if(node->right != NULL)
            {
                if(node->right->right->right == NULL)
                    _ExpHandler_func2(subTree);
                else _ExpHandler_func1(subTree);
            }
            else _ExpHandler_id(subTree);
        }
        else if(!strcmp("MINUS", node->type))
            _ExpHandler_neg(subTree);
        else if(!strcmp("NOT", node->type))
            _ExpHandler_not(subTree);
        else if(!strcmp("LP", node->type))
            _ExpHandler_LP(subTree);
    }
}

void ArgsHandler(TreeNode subTree, int* arg_num, VariableType** type, char(** structNameList)[32], ArrMsg** arrayMsg)
{
    TreeNode node = subTree->child;
    *arg_num = 0;
    while(node->right != NULL)
    {
        ExpHandler(node);
        (*arg_num)++;
        node = node->right->right->child;
    }
    ExpHandler(node);
    (*arg_num)++;
    *arrayMsg = (ArrMsg*)calloc(*arg_num, sizeof(ArrMsg));
    *type = (VariableType*)calloc(*arg_num, sizeof(VariableType));
    *structNameList = (char(*)[32])calloc(*arg_num, sizeof(char) * 32);
    int i = 0;
    node = subTree->child;
    while(node->right != NULL)
    {
        (*type)[i] = node->msg1.i;
        if((*type)[i] == Struct)
        {
            strncpy((*structNameList)[i], (char*)(node->msg2.p), 32);
        }
        if(node->msg4.p != NULL)
        {
            memcpy(&(*arrayMsg)[i], (ArrMsg*)(node->msg4.p), sizeof(ArrMsg));
            free(node->msg4.p);
        }
        i++;    // dim不重要
        node = node->right->right->child;
    }
    (*type)[i] = node->msg1.i;
    if((*type)[i] == Struct)
    {
        strncpy((*structNameList)[i], (char*)(node->msg2.p), 32);
    }
    if(node->msg4.p != NULL)
    {
        memcpy(&(*arrayMsg)[i], (ArrMsg*)(node->msg4.p), sizeof(ArrMsg));
        free(node->msg4.p);
    }
    subTree->msg1.i = *arg_num;
}



