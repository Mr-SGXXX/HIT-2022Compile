#include "translate.h"

static int tempValueNum = 1;
static int labelNum = 1;

char* newTemp()
{
    char* rst = (char *)calloc(32, sizeof(char));
    sprintf(rst, "t%d", tempValueNum++);
    return rst;
}

char* newLabel()
{
    char* rst = (char *)calloc(32, sizeof(char));
    sprintf(rst, "label%d", labelNum++);
    return rst;
}

TempCodeNode genTempCode(OpType op, char* con, char* arg1, char* arg2, char* arg3)
{
    TempCodeNode node = (TempCodeNode)calloc(1, sizeof(struct tempCodeNode));
    node->code = (OpNode)calloc(1, sizeof(struct opNode));
    node->code->opType = op;
    if (con != NULL)
        strncpy(node->code->con, con, 4);
    ArgNode argNode, tempNode;
    argNode = (ArgNode)calloc(1, sizeof(struct argNode));
    argNode->val = (char*)calloc(32, sizeof(char));
    strncpy(argNode->val, arg1, 32);
    node->code->next = argNode;
    tempNode = argNode;
    if (arg2 != NULL) 
    {
        argNode = (ArgNode)calloc(1, sizeof(struct argNode));
        argNode->val = (char*)calloc(32, sizeof(char));
        strncpy(argNode->val, arg2, 32);
        tempNode->next = argNode;
        tempNode = argNode;
    }
    if (arg3 != NULL) 
    {
        argNode = (ArgNode)calloc(1, sizeof(struct argNode));
        argNode->val = (char*)calloc(32, sizeof(char));
        strncpy(argNode->val, arg3, 32);
        tempNode->next = argNode;
    }
    return node;
}

TempCodeNode concatTempCode(TempCodeNode node1, TempCodeNode node2)
{
    TempCodeNode node = node1;
    if (node1 == NULL) return node2;
    if (node2 == NULL) return node1;
    while (node->next != NULL)
        node = node->next;
    node->next = node2;
    return node1;
}

void printTempCode(TempCodeNode startNode)
{
    TempCodeNode node = startNode;
    OpNode opNode;
    while(node != NULL)
    {
        opNode = node->code;
        switch(opNode->opType)
        {
        case LABEL:
            printf("LABEL %s :\n", opNode->next->val);
            break;
        case FUNCTION:
            printf("FUNCTION %s :\n", opNode->next->val);
            break;
        case ASSIGN:
            printf("%s := %s\n", opNode->next->val, opNode->next->next->val);
            break;
        case PLUS:
            printf("%s := %s + %s\n", opNode->next->val,\
             opNode->next->next->val, opNode->next->next->next->val);
            break;
        case MINUS:
            printf("%s := %s - %s\n", opNode->next->val,\
             opNode->next->next->val, opNode->next->next->next->val);
            break;
        case MULTIFY:
            printf("%s := %s * %s\n", opNode->next->val,\
             opNode->next->next->val, opNode->next->next->next->val);
            break;
        case DIV:
            printf("%s := %s / %s\n", opNode->next->val,\
             opNode->next->next->val, opNode->next->next->next->val);
            break;
        case GETADDR:
            printf("%s := &%s\n", opNode->next->val, opNode->next->next->val);
            break;
        case GETADDRVAL:
            printf("%s := *%s\n", opNode->next->val, opNode->next->next->val);
            break;
        case SETADDRVAL:
            printf("*%s := %s\n", opNode->next->val, opNode->next->next->val);
            break;
        case GOTO:
            printf("GOTO %s\n", opNode->next->val);
            break;
        case CONGOTO:
            printf("IF %s %s %s GOTO %s\n", opNode->next->val, opNode->con,\
            opNode->next->next->val, opNode->next->next->next->val);
            break;
        case RET:
            printf("RETURN %s\n", opNode->next->val);
            break;
        case DEC:
            printf("DEC %s %s\n", opNode->next->val, opNode->next->next->val);
            break;
        case ARG:
            printf("ARG %s\n", opNode->next->val);
            break;
        case CALL:
            printf("%s := CALL %s\n", opNode->next->val, opNode->next->next->val);
            break;
        case PARAM:
            printf("PARAM %s\n", opNode->next->val);
            break;
        case READ:
            printf("READ %s\n", opNode->next->val);
            break;
        case WRITE:
            printf("WRITE %s\n", opNode->next->val);
            break; 
        }
        node = node->next;
    }
}

void freeTempCode(TempCodeNode startNode)
{
    TempCodeNode node = startNode;
    TempCodeNode temp;
    ArgNode argNode, tempArgNode;
    OpNode opNode;
    while (node != NULL)
    {
        opNode = node->code;
        argNode = opNode->next;
        while (argNode != NULL)
        {
            tempArgNode = argNode->next;
            free(argNode);
            argNode = tempArgNode;
        }
        free(opNode);
        temp = node->next;
        free(node);
        node = temp;
    }
        
}

TempCodeNode translate(TreeNode root)
{
    return translateExtDefList(root->child);
}

TempCodeNode translateExtDefList(TreeNode subTree)
{
    TempCodeNode code1, code2;
    if(subTree->child == NULL)
    {
        return NULL;
    }
    else
    {   
        code1 = translateExtDef(subTree->child);
        code2 = translateExtDefList(subTree->child->right);
        code1 = concatTempCode(code1, code2);
        return code1;
    }
}

TempCodeNode translateExtDef(TreeNode subTree)
{
    TreeNode node = subTree->child;
    TempCodeNode code1, code2;
    if (!strcmp(node->right->type, "FunDec"))
    {
        code1 = translateFunDec(node->right);
        code2 = translateCompSt(node->right->right);
        code1 = concatTempCode(code1, code2);
        return code1;
    }
    else return NULL;
}

TempCodeNode translateFunDec(TreeNode subTree)
{
    TreeNode node = subTree->child;
    TempCodeNode code1, code2;
    char* id = getTermValue(node);
    code1 = genTempCode(FUNCTION, NULL, id, NULL, NULL);
    if (node->right->right->right != NULL)
    {
        code2 = translateVarList(node->right->right);
        code1 = concatTempCode(code1, code2);
    }
    return code1;
}

TempCodeNode translateVarList(TreeNode subTree)
{
    TreeNode node = subTree->child;
    TempCodeNode code1, code2;
    if (node->right == NULL) return translateParamDec(node);
    else 
    {
        code1 = translateParamDec(node);
        code2 = translateVarList(node->right->right);
        code1 = concatTempCode(code1, code2);
        return code1;
    }
}

TempCodeNode translateParamDec(TreeNode subTree)
{
    TreeNode node = subTree->child;
    char* id = subTree->msg4.p;
    return genTempCode(PARAM, NULL, id, NULL, NULL);
}

TempCodeNode translateCompSt(TreeNode subTree)
{
    TreeNode node = subTree->child;
    TempCodeNode code1, code2;
    code1 = translateDefList(node->right);
    code2 = translateStmtList(node->right->right);
    code1 = concatTempCode(code1, code2);
    return code1;
}

TempCodeNode translateDefList(TreeNode subTree)
{
    TreeNode node = subTree->child;
    TempCodeNode code1, code2;
    if (node == NULL)
        return NULL;
    else
    {
        code1 = translateDef(node);
        code2 = translateDefList(node->right);
        code1 = concatTempCode(code1, code2);
        return code1;
    }
}

int getArraySize(ArrMsg arrMsg, int typeSize)
{
    int size = 1;
    if (arrMsg.valid)
        for(int j = 0; j < arrMsg.dim_size; j++)
            size *= arrMsg.dim[j];
    size *= typeSize;
    return size;
}

int getStructSize(char* structName)
{
    int size = 0;
    Symbol sym;
    checkInTable(structName, &sym);
    StructMsg structMsg = *((StructMsg*)(sym.moreMsg));
    for(int i = 0; i < structMsg.var_num; i++)
    {
        switch(structMsg.typeList[i])
        {
        case Int:
            size += getArraySize(structMsg.arrayMsg[i], 4);
            break;
        case Struct:
            size += getStructSize(structMsg.structNameList[i]);
            break;
        default:
            break;
        }
    }
    return size;
}

TempCodeNode translateDef(TreeNode subTree)
{
    TreeNode node = subTree->child;
    Symbol sym;
    ArrMsg arrMsg;
    int size = 1;
    char str_size[32];
    int dec_num = subTree->msg1.i;
    char(* dec_list)[32] = (char(*)[32])calloc(dec_num, sizeof(char) * 32);
    TempCodeNode code1 = NULL, code2, tempCode;
    code2 = translateDecList(node->right, dec_list, 0);
    for(int i = 0; i< dec_num; i++)
    {
        checkInTable(dec_list[i], &sym);
        switch (((VarMsg*)(sym.moreMsg))->type)
        {
        case Int:
            arrMsg = ((VarMsg*)(sym.moreMsg))->arrayMsg;
            if (arrMsg.valid)
            {
                size = getArraySize(arrMsg, 4);
                sprintf(str_size, "%d", size);
                tempCode = genTempCode(DEC, NULL, dec_list[i], str_size, NULL);
            }
            else tempCode = NULL;
            break;
        case Struct:
            size = getStructSize(((VarMsg*)(sym.moreMsg))->structName);
            sprintf(str_size, "%d", size);
            tempCode = genTempCode(DEC, NULL, dec_list[i], str_size, NULL);
            break;
        default:
            tempCode = NULL;
            break;
        }
        code1 = concatTempCode(code1, tempCode);
    }
    code1 = concatTempCode(code1, code2);
    return code1;
}

TempCodeNode translateDecList(TreeNode subTree, char(* dec_list)[32], int deep)
{
    TreeNode node = subTree->child;
    TempCodeNode code1, code2;
    if(node->right != NULL)
    {
        code1 = translateDec(node, dec_list[deep]);
        code2 = translateDecList(node->right->right, dec_list, deep + 1);
        code1 = concatTempCode(code1, code2);
    }
    else
    {
        code1 = translateDec(node, dec_list[deep]);
    }
    return code1;
}

TempCodeNode translateDec(TreeNode subTree, char dec[32])
{
    TreeNode node = subTree->child;
    TempCodeNode code1, code2;
    translateVarDec(node, dec);
    if (node->right != NULL)
    {
        char* t1 = newTemp();
        code1 = translateExp(node->right->right, t1);
        code2 = genTempCode(ASSIGN, NULL, dec, t1, NULL);
        code1 = concatTempCode(code1, code2);
        free(t1);
        return code1;
    }
    return NULL;
}

TempCodeNode translateVarDec(TreeNode subTree, char dec[32])
{
    TreeNode node = subTree->child;
    if (!strcmp(node->type, "ID"))
        strncpy(dec, getTermValue(node), 32);
    else translateVarDec(node, dec);
    return NULL;
}

TempCodeNode translateStmtList(TreeNode subTree)
{
    TempCodeNode code1, code2;
    if (subTree->child == NULL) return NULL;
    else
    {
        code1 = translateStmt(subTree->child);
        code2 = translateStmtList(subTree->child->right);
        code1 = concatTempCode(code1, code2);
        return code1;
    }
}

TempCodeNode _translateStmt_Exp(TreeNode subTree)
{
    return translateExp(subTree->child, NULL);
}

TempCodeNode _translateStmt_CompSt(TreeNode subTree)
{
    return translateCompSt(subTree->child);
}

TempCodeNode _translateStmt_Return(TreeNode subTree)
{
    char* t1 = newTemp();
    TempCodeNode code1, code2;
    code1 = translateExp(subTree->child->right, t1);
    code2 = genTempCode(RET, NULL, t1, NULL, NULL);
    code1 = concatTempCode(code1, code2);
    free(t1);
    return code1;
}

TempCodeNode _translateStmt_If(TreeNode subTree)
{
    char* label1 = newLabel();
    char* label2 = newLabel();
    TempCodeNode code1, code2, tempCode;
    code1 = translateCond(subTree->child->right->right, label1, label2);
    code2 = translateStmt(subTree->child->right->right->right->right);
    tempCode = genTempCode(LABEL, NULL, label1, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    code1 = concatTempCode(code1, code2);
    tempCode = genTempCode(LABEL, NULL, label2, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    free(label1);
    free(label2);
    return code1;
}

TempCodeNode _translateStmt_If_Else(TreeNode subTree)
{
    char* label1 = newLabel();
    char* label2 = newLabel();
    char* label3 = newLabel();
    TempCodeNode code1, code2, code3, tempCode;
    code1 = translateCond(subTree->child->right->right, label1, label2);
    code2 = translateStmt(subTree->child->right->right->right->right);
    code3 = translateStmt(subTree->child->right->right->right->right->right->right);
    tempCode = genTempCode(LABEL, NULL, label1, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    code1 = concatTempCode(code1, code2);
    tempCode = genTempCode(GOTO, NULL, label3, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    tempCode = genTempCode(LABEL, NULL, label2, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    code1 = concatTempCode(code1, code3);
    tempCode = genTempCode(LABEL, NULL, label3, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    free(label1);
    free(label2);
    free(label3);
    return code1;
}

TempCodeNode _translateStmt_While(TreeNode subTree)
{
    char* label1 = newLabel();
    char* label2 = newLabel();
    char* label3 = newLabel();
    TempCodeNode code1, code2, tempCode;
    code1 = translateCond(subTree->child->right->right, label2, label3);
    code2 = translateStmt(subTree->child->right->right->right->right);
    tempCode = genTempCode(LABEL, NULL, label1, NULL, NULL);
    code1 = concatTempCode(tempCode, code1);
    tempCode = genTempCode(LABEL, NULL, label2, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    code1 = concatTempCode(code1, code2);
    tempCode = genTempCode(GOTO, NULL, label1, NULL, NULL); 
    code1 = concatTempCode(code1, tempCode);
    tempCode = genTempCode(LABEL, NULL, label3, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    free(label1);
    free(label2);
    free(label3);
    return code1;
}

TempCodeNode translateStmt(TreeNode subTree)
{
    TreeNode node = subTree->child;
    if(!strcmp(node->type, "Exp"))
        return _translateStmt_Exp(subTree);
    else if(!strcmp(node->type, "CompSt"))
        return _translateStmt_CompSt(subTree);
    else if(!strcmp(node->type, "RETURN"))
        return _translateStmt_Return(subTree);
    else if(!strcmp(node->type, "IF"))
        if (node->right->right->right->right->right == NULL)
            return _translateStmt_If(subTree);
        else return _translateStmt_If_Else(subTree);
    else if(!strcmp(node->type, "WHILE"))
        return _translateStmt_While(subTree);
}

TempCodeNode _tranlateCond_Relop(TreeNode subTree, char* labelTrue, char* labelFalse)
{
    TreeNode node = subTree->child;
    char* t1 = newTemp();
    char* t2 = newTemp();
    TempCodeNode code1, code2, code3;
    code1 = translateExp(node, t1);
    code2 = translateExp(node->right->right, t2);
    char* con = getTermValue(node->right);
    code3 = genTempCode(CONGOTO, con, t1, t2, labelTrue);
    code1 = concatTempCode(code1, code2);
    code1 = concatTempCode(code1, code3);
    TempCodeNode tempCode = genTempCode(GOTO, NULL, labelFalse, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    free(t1);
    free(t2);
    return code1;
}

TempCodeNode _tranlateCond_Not(TreeNode subTree, char* labelTrue, char* labelFalse)
{
    return translateCond(subTree->child->right, labelFalse, labelTrue);
}

TempCodeNode _tranlateCond_And(TreeNode subTree, char* labelTrue, char* labelFalse)
{
    char* label1 = newLabel();
    TempCodeNode code1, code2, tempCode;
    code1 = translateCond(subTree->child, label1, labelFalse);
    code2 = translateCond(subTree->child->right->right, labelTrue, labelFalse);
    tempCode = genTempCode(LABEL, NULL, label1, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    code1 = concatTempCode(code1, code2);
    free(label1);
    return code1;
}

TempCodeNode _tranlateCond_Or(TreeNode subTree, char* labelTrue, char* labelFalse)
{
    char* label1 = newLabel();
    TempCodeNode code1, code2, tempCode;
    code1 = translateCond(subTree->child, labelTrue, label1);
    code2 = translateCond(subTree->child->right->right, labelTrue, labelFalse);
    tempCode = genTempCode(LABEL, NULL, label1, NULL, NULL);
    code1 = concatTempCode(code1, tempCode);
    code1 = concatTempCode(code1, code2);
    free(label1);
    return code1;
}

TempCodeNode _tranlateCond_Other(TreeNode subTree, char* labelTrue, char* labelFalse)
{
    char* t1 = newTemp();
    TempCodeNode code1, code2;
    code1 = translateExp(subTree, t1);
    code2 = genTempCode(CONGOTO, "!=", t1, "#0", labelTrue);
    code1 = concatTempCode(code1, code2);
    code2 = genTempCode(GOTO, NULL, labelFalse, NULL, NULL);
    code1 = concatTempCode(code1, code2);
    free(t1);
    return code1;
}

TempCodeNode translateCond(TreeNode subTree, char* labelTrue, char* labelFalse)
{ 
    TreeNode node = subTree->child;
    if(!strcmp(node->type, "NOT"))
        return _tranlateCond_Not(subTree, labelTrue, labelFalse);
    else if(!strcmp(node->type, "Exp"))
    {
        node = node->right;
        if(!strcmp(node->type, "RELOP"))
            return _tranlateCond_Relop(subTree, labelTrue, labelFalse);
        else if(!strcmp(node->type, "AND"))
            return _tranlateCond_And(subTree, labelTrue, labelFalse);
        else if(!strcmp(node->type, "OR"))
            return _tranlateCond_Or(subTree, labelTrue, labelFalse);
        else
            return _tranlateCond_Other(subTree, labelTrue, labelFalse);
    }
    else return _tranlateCond_Other(subTree, labelTrue, labelFalse);
}

TempCodeNode _translateExp_Arr(TreeNode subTree, char* place, int setFlag)
{
    TreeNode node = subTree->child;
    TempCodeNode code1, code2, tempCode = NULL;
    Symbol sym;
    int dim_size, type_size;
    int size = 0;
    char str_size[32];
    int* dim;
    char* offset = newTemp();
    char** tempIndex;
    char** index;
    char* id = (char*)(node->msg3.p);
    checkInTable(id, &sym);
    if (((VarMsg*)(sym.moreMsg))->type == Int)
        type_size = 4;
    else if (((VarMsg*)(sym.moreMsg))->type == Struct)
        type_size = getStructSize(((VarMsg*)(sym.moreMsg))->structName);
    dim = ((VarMsg*)(sym.moreMsg))->arrayMsg.dim;
    dim_size = ((VarMsg*)(sym.moreMsg))->arrayMsg.dim_size;
    index = (char**)calloc(dim_size, sizeof(char*));
    tempIndex = (char**)calloc(dim_size, sizeof(char*));
    for(int i = 0; i < dim_size; i++)
    {
        size = 1;
        for(int j = dim_size - 1; j > i + 1; j++)
        {
            size *= dim[j];
        }
        size *= type_size;
        sprintf(str_size, "#%d", size);
        index[dim_size - i - 1] = newTemp();
        tempIndex[dim_size - i - 1] = newTemp();
        code1 = translateExp(node->right->right, index[dim_size - i - 1]);
        code2 = genTempCode(MULTIFY, NULL, tempIndex[dim_size - i - 1], index[dim_size - i - 1], str_size);
        code1 = concatTempCode(code1, code2);
        code2 = genTempCode(PLUS, NULL, offset, offset, tempIndex[dim_size - i - 1]);
        code1 = concatTempCode(code1, code2);
        code1 = concatTempCode(code1, tempCode);
        tempCode = code1;
        node = node->child;
    }
    sprintf(str_size, "#%d", 0);
    tempCode = genTempCode(ASSIGN, NULL, offset, str_size, NULL);
    code1 = concatTempCode(tempCode, code1);
    char* t1 = newTemp();
    tempCode = genTempCode(ASSIGN, NULL, t1, id, NULL);
    code1 = concatTempCode(code1, tempCode);
    char* t2 = newTemp();
    tempCode = genTempCode(PLUS, NULL, t2, t1, offset);
    code1 = concatTempCode(code1, tempCode);
    if(setFlag)
        tempCode = genTempCode(SETADDRVAL, NULL, t2, place, NULL);
    else
        tempCode = genTempCode(GETADDRVAL, NULL, place, t2, NULL);
    code1 = concatTempCode(code1, tempCode);
    for(int i = 0; i < dim_size; i++)
    {
        free(index[i]);
        free(tempIndex[i]);
    }
    free(index);
    free(t1);
    free(t2);
    return code1;
}

TempCodeNode _translateExp_Dot(TreeNode subTree, char* place, int setFlag)
{
    TreeNode node = subTree->child;
    TempCodeNode code1, code2;
    Symbol sym, structSym;
    char* structId = (char*)(node->msg3.p);
    char* var = getTermValue(node->right->right);
    checkInTable(structId, &sym);
    checkInTable(((VarMsg*)(sym.moreMsg))->structName, &structSym);
    int var_num = ((StructMsg*)(structSym.moreMsg))->var_num;
    int size = 0;
    char str_size[32];
    int index = 0;
    for(int i = 0; i < var_num; i++)
        if (!strcmp(((StructMsg*)(structSym.moreMsg))->nameList[i], var))
        {
            index = i;
            break;
        }
    for(int i = 0; i < index; i++)
    {
        if (((StructMsg*)(structSym.moreMsg))->typeList[i] == Int)
            size += 4;
        else if (((StructMsg*)(structSym.moreMsg))->typeList[i] == Struct)
            size += getStructSize(((StructMsg*)(structSym.moreMsg))->structNameList[i]);
    }
    sprintf(str_size, "#%d", size);
    char* t1 = newTemp();
    char* t2 = newTemp();
    code1 = genTempCode(ASSIGN, NULL, t1, structId, NULL);
    code2 = genTempCode(PLUS, NULL, t2, t1, str_size);
    code1 = concatTempCode(code1, code2);
    if (setFlag)
        code2 = genTempCode(SETADDRVAL, NULL, t2, place, NULL);
    else
        code2 = genTempCode(GETADDRVAL, NULL, place, t2, NULL);
    code1 = concatTempCode(code1, code2);
    free(t1);
    free(t2);
    return code1;
}

TempCodeNode _translateExp_Int(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    if (place == NULL) return NULL;
    char* val = getTermValue(node);
    char num[32];
    sprintf(num, "#%s", val);
    return genTempCode(ASSIGN, NULL, place, num, NULL);
}

TempCodeNode _translateExp_Id(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    Symbol sym;
    if (place == NULL) return NULL;
    char* ID = getTermValue(node);
    checkInTable(ID, &sym);
    if (((VarMsg*)(sym.moreMsg))->type != Struct)
        return genTempCode(ASSIGN, NULL, place, ID, NULL);
    else
        return genTempCode(GETADDR, NULL, place, ID, NULL);
}

TempCodeNode _translateExp_Assign(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    if(!strcmp(node->child->type, "ID") && node->child->right == NULL)
    // Exp1 -> ID
    {
        char* var = (char*)(node->msg3.p);
        char* t1 = newTemp();
        TempCodeNode code1, code2, code3;
        code1 = translateExp(node->right->right, t1);
        code2 = genTempCode(ASSIGN, NULL, var, t1, NULL);
        if (place != NULL)
        {
            code3 = genTempCode(ASSIGN, NULL, place, var, NULL);
            code2 = concatTempCode(code2, code3);
        }
        free(t1);
        return concatTempCode(code1, code2);
    }
    else if (!strcmp(node->child->type, "Exp") && !strcmp(node->child->right->type, "LB"))
    //Exp1 -> Exp LB Exp RB
    {
        char* t1 = newTemp();
        TempCodeNode code1, code2;
        code1 = translateExp(node->right->right, t1);
        code2 = _translateExp_Arr(node, t1, 1);
        free(t1);
        return concatTempCode(code1, code2);
    }
    else if(!strcmp(node->child->type, "Exp") && !strcmp(node->child->right->type, "DOT"))
    {
        char* t1 = newTemp();
        TempCodeNode code1, code2;
        code1 = translateExp(node->right->right, t1);
        code2 = _translateExp_Dot(node, t1, 1);
        free(t1);
        return concatTempCode(code1, code2);
    }
    return NULL;
}

TempCodeNode _translateExp_Plus(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    char* t1 = newTemp();
    char* t2 = newTemp();
    TempCodeNode code1, code2, code3;
    code1 = translateExp(node, t1);
    code2 = translateExp(node->right->right, t2);
    if (place != NULL)
    {
        code3 = genTempCode(PLUS, NULL, place, t1, t2);
        code2 = concatTempCode(code2, code3);
    }
    code1 = concatTempCode(code1, code2);
    free(t1);
    free(t2);
    return code1;
}

TempCodeNode _translateExp_Min(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    char* t1 = newTemp();
    char* t2 = newTemp();
    TempCodeNode code1, code2, code3;
    code1 = translateExp(node, t1);
    code2 = translateExp(node->right->right, t2);
    if (place != NULL)
    {
        code3 = genTempCode(MINUS, NULL, place, t1, t2);
        code2 = concatTempCode(code2, code3);
    }
    code1 = concatTempCode(code1, code2);
    free(t1);
    free(t2);
    return code1;
}

TempCodeNode _translateExp_Mul(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    char* t1 = newTemp();
    char* t2 = newTemp();
    TempCodeNode code1, code2, code3;
    code1 = translateExp(node, t1);
    code2 = translateExp(node->right->right, t2);
    if (place != NULL)
    {   
        code3 = genTempCode(MULTIFY, NULL, place, t1, t2);
        code2 = concatTempCode(code2, code3);
    }
    code1 = concatTempCode(code1, code2);
    free(t1);
    free(t2);
    return code1;
}

TempCodeNode _translateExp_Div(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    char* t1 = newTemp();
    char* t2 = newTemp();
    TempCodeNode code1, code2, code3;
    code1 = translateExp(node, t1);
    code2 = translateExp(node->right->right, t2);
    if (place != NULL)
    {
        code3 = genTempCode(DIV, NULL, place, t1, t2);
        code2 = concatTempCode(code2, code3);
    }
    code1 = concatTempCode(code1, code2);
    free(t1);
    free(t2);
    return code1;
}

TempCodeNode _translateExp_Neg(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    char* t1 = newTemp();
    TempCodeNode code1, code2;
    code1 = translateExp(node->right, t1);
    if (place != NULL)
    {
        code2 = genTempCode(MINUS, NULL, place, "#0", t1);
        code1 = concatTempCode(code1, code2);
    }
    free(t1);
    return code1;
}

TempCodeNode _translateExp_Lop(TreeNode subTree, char* place)
{
    char* label1 = newLabel();
    char* label2 = newLabel();
    TempCodeNode code1, code2;
    if (place != NULL)
        code1 = genTempCode(ASSIGN, NULL, place, "#0", NULL);
    else code1 = NULL;
    code2 = translateCond(subTree, label1, label2);
    code1 = concatTempCode(code1, code2);
    code2 = genTempCode(LABEL, NULL, label1, NULL, NULL);
    code1 = concatTempCode(code1, code2);
    if (place != NULL)
        code2 = genTempCode(ASSIGN, NULL, place, "#1", NULL);
    else code2 = NULL;
    code1 = concatTempCode(code1, code2);
    code2 = genTempCode(LABEL, NULL, label2, NULL, NULL);
    code1 = concatTempCode(code1, code2);
    free(label1);
    free(label2);
    return code1;
}

TempCodeNode _translateExp_Func1(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    char* func = getTermValue(node);
    int arg_num = node->right->right->msg1.i;
    char(* arg_list)[32] = (char(*)[32])calloc(arg_num, sizeof(char) * 32);
    TempCodeNode code1, code2, tempCode;
    code1 = translateArgs(node->right->right, arg_list, 0);
    if(!strcmp(func, "write"))
    {
        tempCode = genTempCode(WRITE, NULL, arg_list[0], NULL, NULL);
        code1 = concatTempCode(code1, tempCode);
    }
    else
    {
        code2 = NULL;
        for(int i = 0; i < arg_num; i++)
        {
            tempCode = genTempCode(ARG, NULL, arg_list[i], NULL, NULL);
            code2 = concatTempCode(code2, tempCode);
        }
        code1 = concatTempCode(code1, code2);
        tempCode = genTempCode(CALL, NULL, place, func, NULL);
        code1 = concatTempCode(code1, tempCode);
    }
    free(arg_list);
    return code1;
}

TempCodeNode _translateExp_Func2(TreeNode subTree, char* place)
{
    char* func = getTermValue(subTree->child);
    TempCodeNode code;
    if(!strcmp(func, "read"))
        code = genTempCode(READ, NULL, place, NULL, NULL);
    else code = genTempCode(CALL, NULL, place, func, NULL);
    return code;
}

TempCodeNode translateExp(TreeNode subTree, char* place)
{
    TreeNode node = subTree->child;
    if(!strcmp("Exp", node->type))
    {
        node = node->right;
        if(!strcmp("ASSIGNOP", node->type))
            return _translateExp_Assign(subTree, place);
        else if(!strcmp("AND", node->type))
            return _translateExp_Lop(subTree, place);
        else if(!strcmp("OR", node->type))
            return _translateExp_Lop(subTree, place);
        else if(!strcmp("RELOP", node->type))
            return _translateExp_Lop(subTree, place);
        else if(!strcmp("PLUS", node->type))
            return _translateExp_Plus(subTree, place);
        else if(!strcmp("MINUS", node->type))
            return _translateExp_Min(subTree, place);
        else if(!strcmp("STAR", node->type))
            return _translateExp_Mul(subTree, place);
        else if(!strcmp("DIV", node->type))
            return _translateExp_Div(subTree, place);
        else if(!strcmp("LB", node->type))
            return _translateExp_Arr(subTree, place, 0);
        else if(!strcmp("DOT", node->type))
            return _translateExp_Dot(subTree, place, 0);
    }
    else
    {
        if(!strcmp("ID", node->type))
        {
            if(node->right != NULL)
            {
                if(node->right->right->right == NULL)
                    return _translateExp_Func2(subTree, place);
                else return _translateExp_Func1(subTree, place);
            }
            else return _translateExp_Id(subTree, place);
        }
        else if(!strcmp("MINUS", node->type))
            return _translateExp_Neg(subTree, place);
        else if(!strcmp("NOT", node->type))
            return _translateExp_Lop(subTree, place);
        else if(!strcmp("INT", node->type))
            return _translateExp_Int(subTree, place);
        else if(!strcmp("LP", node->type))
            return translateExp(node->right, place);
    }
}

TempCodeNode translateArgs(TreeNode subTree, char(* arg_list)[32], int deep)
{
    TreeNode node = subTree->child;
    char* t1 = newTemp();
    TempCodeNode code1, code2;
    if(node->right != NULL)
    {
        code1 = translateExp(node, t1);
        strncpy(arg_list[deep], t1, 32);
        code2 = translateArgs(node->right->right, arg_list, deep + 1);
        code1 = concatTempCode(code1, code2);
    }
    else
    {
        code1 = translateExp(node, t1);
        strncpy(arg_list[deep], t1, 32);
    }
    free(t1);
    return code1;
}