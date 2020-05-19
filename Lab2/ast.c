#include "def.h"
#include "parser.tab.h"
struct ASTNode * mknode(int num,int kind,int pos,...);
void display(struct ASTNode *T,int indent);
void semantic_Analysis0(struct ASTNode *T);
void semantic_Analysis(struct ASTNode *T);
struct ASTNode * mknode(int num,int kind,int pos,...){
    struct ASTNode *T=(struct ASTNode *)calloc(sizeof(struct ASTNode),1);
    int i=0;
    T->kind=kind;
    T->pos=pos;
    va_list pArgs = NULL;
    va_start(pArgs, pos);
    for(i=0;i<num;i++)
        T->ptr[i]= va_arg(pArgs, struct ASTNode *);
    while (i<4) T->ptr[i++]=NULL;
    va_end(pArgs);
    return T;
}
void semantic_Analysis0(struct ASTNode *T){
    printf("\n\n");
    printf("单词序列展示：\n");
    semantic_Analysis(T);
}
void semantic_Analysis(struct ASTNode *T){
    int i;
    struct ASTNode *T0;
    switch(T->kind){
        case ID:        printf("(%s, ID)\n", T->type_id);
                        return;
        case ARRAY:     T0 = T->ptr[0];
                        while(T0->kind == ARRAY){
                            T0 = T0->ptr[0];
                        }
                        printf("(%s, ARRAY)\n", T0->type_id);
                        return;
        // case STRUCT_DEF:
        //                 semantic_Analysis(T->ptr[1]);
        //                 return;
        case FUNC_CALL:
        case FUNC_DEC:  printf("(%s, FUNC)\n", T->type_id);
                        break;
        case ASSIGNOP:  printf("(=, ASSIGNOP)\n");
                        break;
        case AND:       printf("($$, AND)\n");
                        break;
        case OR:        printf("(||, OR)\n");
                        break;
        case RELOP:     printf("(%s, RELOP)\n", T->type_id);
                        break;
        case PLUS:      printf("(+, PLUS)\n");
                        break;
        case MINUS:     printf("(-, MINUS)\n");
                        break;
        case STAR:      printf("(*, STAR)\n");
                        break;
        case DIV:       printf("(/, DIV)\n");
                        break;
        case UMINUS:    printf("(-, UMINUS)\n");
                        break;
        case NOT:       printf("(!, NOT)\n");
                        break;
        case INT:       printf("(%d, CONSTANT INT)\n", T->type_int);
                        break;
        case FLOAT:     printf("(%f, CONSTANT FLOAT)\n", T->type_float);
                        break;
        case CHAR:      printf("(%c, CONSTANT CHAR)\n", T->type_char);
                        break;
        case INC:       printf("(++, INC)\n");
                        break;
        case DEC:       printf("(--, DEC)\n");
                        break; 
        case SELFPLUS:  printf("(+=, SELFPLUS)\n");
                        break;
        case SELFMINUS: printf("(-=, SELFMINUS)\n");
                        break;
        case BREAK:     printf("(break, BREAK)\n");
                        break;
        case CONTINUE:  printf("(continue, CONTINUE)\n");
                        break;                            
    }
    for(i = 0; i < 3; i ++){
        if(T->ptr[i]){
            semantic_Analysis(T->ptr[i]);
        }
    }
    return;
}

void display(struct ASTNode *T,int indent)
{//对抽象语法树的先根遍历
  int i=1;
  struct ASTNode *T0;
  if (T)
	{
	switch (T->kind) {
	case EXT_DEF_LIST:  display(T->ptr[0],indent);    //显示该外部定义（外部变量和函数）列表中的第一个
                        display(T->ptr[1],indent);    //显示该外部定义列表中的其它外部定义
                        break;
	case EXT_VAR_DEF:   printf("%*c外部变量定义：(%d)\n",indent,' ',T->pos);
                        display(T->ptr[0],indent+3);        //显示外部变量类型
                        printf("%*c变量名：\n",indent+3,' ');
                        display(T->ptr[1],indent+6);        //显示变量列表
                        break;
	case TYPE:          printf("%*c类型： %s\n",indent,' ',T->type_id);
                        break;
    case STRUCT_TYPE:   printf("%*c类型： STURCT\n",indent,' ');
                        display(T->ptr[0], indent + 3);
                        break;
    case EXT_DEC_LIST:  display(T->ptr[0],indent);     //依次显示外部变量名，
                        display(T->ptr[1],indent);     //后续还有相同的，仅显示语法树此处理代码可以和类似代码合并
                        break;
	case FUNC_DEF:      printf("%*c函数定义：(%d)\n",indent,' ',T->pos);
                        display(T->ptr[0],indent+3);      //显示函数返回类型
                        display(T->ptr[1],indent+3);      //显示函数名和参数
                        display(T->ptr[2],indent+3);      //显示函数体
                        break;
	case FUNC_DEC:      printf("%*c函数名：%s\n",indent,' ',T->type_id);
                        if (T->ptr[0]) {
                            printf("%*c函数形参：\n",indent,' ');
                            display(T->ptr[0],indent+3);  //显示函数参数列表
                        }
                        else printf("%*c无参函数\n",indent+3,' ');
                        break;
	case PARAM_LIST:    display(T->ptr[0],indent);     //依次显示全部参数类型和名称，
                        display(T->ptr[1],indent);
                        break;
	case PARAM_DEC:     printf("%*c类型：%s, 参数名：%s\n",indent,' ',T->ptr[0]->type==INT?"int":"float",T->ptr[1]->type_id);
                        break;
	case EXP_STMT:      printf("%*c表达式语句：(%d)\n",indent,' ',T->pos);
                        display(T->ptr[0],indent+3);
                        break;
	case RETURN:        printf("%*c返回语句：(%d)\n",indent,' ',T->pos);
                        display(T->ptr[0],indent+3);
                        break;
	case COMP_STM:      printf("%*c复合语句：(%d)\n",indent,' ',T->pos);
                        printf("%*c复合语句的变量定义部分：\n",indent+3,' ');
                        display(T->ptr[0],indent+6);      //显示定义部分
                        printf("%*c复合语句的语句部分：\n",indent+3,' ');
                        display(T->ptr[1],indent+6);      //显示语句部分
                        break;
	case STM_LIST:      display(T->ptr[0],indent);      //显示第一条语句
                        display(T->ptr[1],indent);        //显示剩下语句
                        break;
	case WHILE:         printf("%*cwhile循环语句：(%d)\n",indent,' ',T->pos);
                        printf("%*cwhile循环条件：\n",indent+3,' ');
                        display(T->ptr[0],indent+6);      //显示循环条件
                        printf("%*cwhile循环体：(%d)\n",indent+3,' ',T->pos);
                        display(T->ptr[1],indent+6);      //显示循环体
                        break;
    case FOR:           printf("%*cfor循环语句：(%d)\n",indent,' ',T->pos);
                        printf("%*cfor循环起始状态:\n",indent+3, ' ');
                        display(T->ptr[0],indent+6);      //显示循环起始状态
                        printf("%*cfor循环判断:\n",indent+3, ' ');
                        display(T->ptr[1],indent+6);      //显示循环判断
                        printf("%*cfor循环更新:\n",indent+3, ' ');
                        display(T->ptr[2],indent+6);      //显示循环判断
                        printf("%*cfor循环体：(%d)\n",indent+3,' ',T->pos);
                        display(T->ptr[3],indent+6);      //显示循环体
                        break;
	case IF_THEN:       printf("%*c条件语句(IF_THEN)：(%d)\n",indent,' ',T->pos);
                        printf("%*c条件：\n",indent+3,' ');
                        display(T->ptr[0],indent+6);      //显示条件
                        printf("%*cIF子句：(%d)\n",indent+3,' ',T->pos);
                        display(T->ptr[1],indent+6);      //显示if子句
                        break;
	case IF_THEN_ELSE:  printf("%*c条件语句(IF_THEN_ELSE)：(%d)\n",indent,' ',T->pos);
                        printf("%*c条件：\n",indent+3,' ');
                        display(T->ptr[0],indent+6);      //显示条件
                        printf("%*cIF子句：(%d)\n",indent+3,' ',T->pos);
                        display(T->ptr[1],indent+6);      //显示if子句
                        printf("%*cELSE子句：(%d)\n",indent+3,' ',T->pos);
                        display(T->ptr[2],indent+6);      //显示else子句
                        break;
    case DEF_LIST:      display(T->ptr[0],indent);    //显示该局部变量定义列表中的第一个
                        display(T->ptr[1],indent);    //显示其它局部变量定义
                        break;
    case VAR_DEF:       printf("%*c局部变量定义：(%d)\n",indent,' ',T->pos);
                        display(T->ptr[0],indent+3);   //显示变量类型
                        display(T->ptr[1],indent+3);   //显示该定义的全部变量名
                        break;
    case DEC_LIST:      printf("%*c变量名：\n",indent,' ');
                        T0=T;
                        while (T0) {
                            if (T0->ptr[0]->kind==ID ||T0->ptr[0]->kind == ARRAY)
                                //printf("%*c %s\n",indent+6,' ',T0->ptr[0]->type_id);
                                display(T0->ptr[0], indent+6);
                            else if (T0->ptr[0]->kind==ASSIGNOP)
                                {
                                printf("%*c %s ASSIGNOP\n ",indent+6,' ',T0->ptr[0]->ptr[0]->type_id);
                                display(T0->ptr[0]->ptr[1],indent+strlen(T0->ptr[0]->ptr[0]->type_id)+7);        //显示初始化表达式
                                }
                            T0=T0->ptr[1];
                            }
                        break;
	case ID:	        printf("%*cID： %s\n",indent,' ',T->type_id);
                        break;
	case INT:	        printf("%*cINT：%d\n",indent,' ',T->type_int);
                        break;
	case FLOAT:	        printf("%*cFLOAT：%f\n",indent,' ',T->type_float);
                        break;
    case CHAR:	        printf("%*cCHAR：%c\n",indent,' ',T->type_char);
                        break;
    case ARRAY:         printf("%*c%s\n",indent,' ',T->type_id);
                        display(T->ptr[0],indent+3);
                        display(T->ptr[1],indent+3);
                        break;
    case VARIABLE:      display(T->ptr[0],indent);
                        break;
    case BREAK:         printf("%*c%s\n",indent,' ',T->type_id);
                        break;
    case CONTINUE:      printf("%*c%s\n",indent,' ',T->type_id);
                        break;
    // case SWITCH:        printf("%*c%s\n",indent,' ',T->type_id);
    //                     printf("%*c控制语句：\n",indent,' ');
    //                     display(T->ptr[0],indent+3);
    //                     printf("%*cCase语句：\n",indent,' ');
    //                     display(T->ptr[1],indent+3);
    //                     break;
    // case CASE_STMT_LIST:printf("Case条件：\n");
    //                     display(T->ptr[0],indent+3);
    //                     printf("对应操作：\n");
    //                     display(T->ptr[1],indent+3);
    //                     display(T->ptr[2],indent);
    //                     break;
    // case DEFAULT:       printf("%*c%s\n",indent,' ',T->type_id);
    //                     printf("对应操作：\n");
    //                     display(T->ptr[1],indent+3);
    //                     break;
    case STRUCT_DEF:    printf("%*c结构名：\n", indent, ' ');
                        display(T->ptr[0], indent + 3);
                        if(T->ptr[1]){
                            printf("%*c结构定义：\n", indent, ' ');
                            display(T->ptr[1], indent + 3);
                        }else{
                            printf("%*c结构定义：无\n", indent, ' ');
                        }
                        break;
    case STRUCT_VISIT:  printf("%*c结构名：\n", indent, ' ');
                        display(T->ptr[0], indent + 3);
                        printf("%*c结构内变量名：\n", indent, ' ');
                        display(T->ptr[1], indent + 3);
                        break;
	case ASSIGNOP:
	case AND:
	case OR:
	case RELOP:
	case PLUS:
	case MINUS:
	case STAR:
	case DIV:
                    printf("%*c%s\n",indent,' ',T->type_id);
                    display(T->ptr[0],indent+3);
                    display(T->ptr[1],indent+3);
                    break;
    case INC:       
    case DEC:       display(T->ptr[0],indent);
                    printf("%*c%s\n",indent,' ',T->type_id);
                    return;
    case SELFSTAR:
    case SELFDIV:
    case SELFPLUS:  
    case SELFMINUS: display(T->ptr[0],indent);
                    printf("%*c%s\n",indent,' ',T->type_id);
                    display(T->ptr[1],indent+3);
                    break;
	case NOT:
	case UMINUS:    printf("%*c%s\n",indent,' ',T->type_id);
                    display(T->ptr[0],indent+3);
                    break;
    case FUNC_CALL: printf("%*c函数调用：(%d)\n",indent,' ',T->pos);
                    printf("%*c函数名：%s\n",indent+3,' ',T->type_id);
                    display(T->ptr[0],indent+3);
                    break;
	case ARGS:      i=1;
                    while (T) {  //ARGS表示实际参数表达式序列结点，其第一棵子树为其一个实际参数表达式，第二棵子树为剩下的
                        struct ASTNode *T0=T->ptr[0];
                        printf("%*c第%d个实际参数表达式：\n",indent,' ',i++);
                        display(T0,indent+3);
                        T=T->ptr[1];
                        }
//                    printf("%*c第%d个实际参数表达式：\n",indent,' ',i);
  //                  display(T,indent+3);
                    printf("\n");
                    break;
         }
      }
}
