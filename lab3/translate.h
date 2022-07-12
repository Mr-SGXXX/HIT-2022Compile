#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"
#include "syntax_tree.h"

typedef enum opType
{
    LABEL,      //定义标号
    FUNCTION,   //定义函数
    ASSIGN,     //赋值操作
    PLUS,       //加法操作
    MINUS,      //减法操作
    MULTIFY,    //乘法操作
    DIV,        //除法操作
    GETADDR,    //取y的地址赋给x
    GETADDRVAL, //取以y为地址的内存单元的内容赋给x
    SETADDRVAL, //取y值赋给以x值为地址的内存单元
    GOTO,       //无条件跳转
    CONGOTO,    //有条件跳转
    RET,        //退出当前函数并返回x值
    DEC,        //内存空间申请，大小为四的倍数
    ARG,        //传实参x
    CALL,       //调用函数,并将其返回值赋给x
    PARAM,      //函数参数声明
    READ,       //从控制台读取x的值
    WRITE       //向控制台打印x的值
}OpType;

typedef struct opNode
//赋值操作时第一个参数为被赋值对象
{
    OpType opType;      //运算类型
    char con[4];    //条件类型
    struct argNode* next;
}* OpNode;

typedef struct argNode
{
    char* val;
    struct argNode* next;
}* ArgNode;

typedef struct tempCodeNode
{
    /* data */
    struct opNode* code;
    struct tempCodeNode* next;
}* TempCodeNode;

//生成新中间变量，用完记得free
char* newTemp();

//生成新标号，用完记得free
char* newLabel();

//生成一个中间代码条目，op不为CONGOTO时，con应置为NULL，多余的参数应置为NULL
TempCodeNode genTempCode(OpType op, char* con, char* arg1, char* arg2, char* arg3);

//连接两个中间代码条目
TempCodeNode concatTempCode(TempCodeNode node1, TempCodeNode node2);

//打印中间代码
void printTempCode(TempCodeNode startNode);

//清空中间代码内存
void freeTempCode(TempCodeNode startNode);

//全部代码翻译
TempCodeNode translate(TreeNode root);

//所有全局定义翻译
TempCodeNode translateExtDefList(TreeNode subTree);

//全局定义翻译
TempCodeNode translateExtDef(TreeNode subTree);

//函数定义翻译
TempCodeNode translateFunDec(TreeNode subTree);

//全部局部变量声明翻译
TempCodeNode translateDefList(TreeNode subTree);

//局部变量声明翻译
TempCodeNode translateDef(TreeNode subTree);

//函数参数列表翻译
TempCodeNode translateVarList(TreeNode subTree);

//函数参数翻译
TempCodeNode translateParamDec(TreeNode subTree);

//局部变量名列表翻译
TempCodeNode translateDecList(TreeNode subTree, char(* dec_list)[32], int deep);

//局部变量名翻译
TempCodeNode translateDec(TreeNode subTree, char dec[32]);

//获取变量名
TempCodeNode translateVarDec(TreeNode subTree, char dec[32]);

//代码块翻译模式
TempCodeNode translateCompSt(TreeNode subTree);

//全部语句翻译模式
TempCodeNode translateStmtList(TreeNode subTree);

//语句翻译模式
TempCodeNode translateStmt(TreeNode subTree);

//条件表达式翻译模式
TempCodeNode translateCond(TreeNode subTree, char* labelTrue, char* labelFalse);

//表达式翻译模式 
TempCodeNode translateExp(TreeNode subTree, char* place);

//函数参数翻译模式
TempCodeNode translateArgs(TreeNode subTree, char(* arg_list)[32], int deep);