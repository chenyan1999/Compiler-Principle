#include "def.h"
#include "parser.tab.h"
#define DEBUG 1
int INLOOP=0;
int blockid = 0;
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

char *strcat0(char *s1,char *s2){
    static char result[10];
    strcpy(result,s1);
    strcat(result,s2);
    return result;
}

char *newAlias() {
    static int no=1;
    char s[10];
    itoa(no++,s,10);
    return strcat0("v",s);
}

char *newLabel() {
    static int no=1;
    char s[10];
    itoa(no++,s,10);
    return strcat0("label",s);
}

char *newTemp(){
    static int no=1;
    char s[10];
    itoa(no++,s,10);
    return strcat0("temp",s);
}

//生成一条TAC代码的结点组成的双向循环链表，返回头指针
struct codenode *genIR(int op,struct opn opn1,struct opn opn2,struct opn result){
    struct codenode *h=(struct codenode *)malloc(sizeof(struct codenode));
    h->op=op;
    h->opn1=opn1;
    h->opn2=opn2;
    h->result=result;
    h->next=h->prior=h;
    return h;
}

//生成一条标号语句，返回头指针
struct codenode *genLabel(char *label){
    struct codenode *h=(struct codenode *)malloc(sizeof(struct codenode));
    h->op=LABEL;
    strcpy(h->result.id,label);
    h->next=h->prior=h;
    return h;
}

//生成GOTO语句，返回头指针
struct codenode *genGoto(char *label){
    struct codenode *h=(struct codenode *)malloc(sizeof(struct codenode));
    h->op=GOTO;
    strcpy(h->result.id,label);
    h->next=h->prior=h;
    return h;
}

//合并多个中间代码的双向循环链表，首尾相连
struct codenode *merge(int num,...){
    struct codenode *h1,*h2,*p,*t1,*t2;
    va_list ap;
    va_start(ap,num);
    h1=va_arg(ap,struct codenode *);
    while (--num>0) {
        h2=va_arg(ap,struct codenode *);
        if (h1==NULL) h1=h2;
        else if (h2){
            t1=h1->prior;
            t2=h2->prior;
            t1->next=h2;
            t2->next=h1;
            h1->prior=t2;
            h2->prior=t1;
            }
        }
    va_end(ap);
    return h1;
}

//输出中间代码
void prnIR(struct codenode *head){
    char opnstr1[32],opnstr2[32],resultstr[32];
    struct codenode *h=head;
    do {
        if (h->opn1.kind==INT)
             sprintf(opnstr1,"#%d",h->opn1.const_int);
        if (h->opn1.kind==FLOAT)
             sprintf(opnstr1,"#%f",h->opn1.const_float);
        if (h->opn1.kind==ID)
             sprintf(opnstr1,"%s",h->opn1.id);
        if (h->opn2.kind==INT)
             sprintf(opnstr2,"#%d",h->opn2.const_int);
        if (h->opn2.kind==FLOAT)
             sprintf(opnstr2,"#%f",h->opn2.const_float);
        if (h->opn2.kind==ID)
             sprintf(opnstr2,"%s",h->opn2.id);
        sprintf(resultstr,"%s",h->result.id);
        switch (h->op) {
            case ASSIGNOP:  printf("  %s := %s\n",resultstr,opnstr1);
                            break;
            case INC:
            case DEC:       printf("  %s := %s %c #1\n",resultstr,opnstr1,\
                            h->op==INC? '+':'-');
                            break;
            case UMINUS:    printf("  %s := 0 - %s\n",resultstr,opnstr1);
                            break;
            case SELFPLUS:
            case SELFMINUS:
            case SELFSTAR:
            case SELFDIV:   printf("  %s := %s %c %s\n",resultstr,opnstr1, \
                            h->op==SELFPLUS?'+':h->op==SELFMINUS?'-':h->op==SELFSTAR?'*':'/',opnstr2);
                            break;
            case PLUS:
            case MINUS:
            case STAR:
            case DIV:       printf("  %s := %s %c %s\n",resultstr,opnstr1, \
                            h->op==PLUS?'+':h->op==MINUS?'-':h->op==STAR?'*':'/',opnstr2);
                            break;
            case FUNCTION:  printf("\nFUNCTION %s :\n",h->result.id);
                            break;
            case PARAM:     printf("  PARAM %s\n",h->result.id);
                            break;
            case LABEL:     printf("LABEL %s :\n",h->result.id);
                            break;
            case GOTO:      printf("  GOTO %s\n",h->result.id);
                            break;
            case JLE:       printf("  IF %s <= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                            break;
            case JLT:       printf("  IF %s < %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                            break;
            case JGE:       printf("  IF %s >= %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                            break;
            case JGT:       printf("  IF %s > %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                            break;
            case EQ:        printf("  IF %s == %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                            break;
            case NEQ:       printf("  IF %s != %s GOTO %s\n",opnstr1,opnstr2,resultstr);
                            break;
            case ARG:       printf("  ARG %s\n",h->result.id);
                            break;
            case CALL:      if (!strcmp(opnstr1,"write"))
                                printf("  CALL  %s\n", opnstr1);
                            else
                                printf("  %s := CALL %s\n",resultstr, opnstr1);
                            break;
            case RETURN:    if (h->result.kind)
                                printf("  RETURN %s\n",resultstr);
                            else
                                printf("  RETURN\n");
                            break;
        }
    h=h->next;
    } while (h!=head);
}

void objectCode(struct codenode *head){
    FILE *fp;
    char opnstr1[32],opnstr2[32],resultstr[32];
    struct codenode *h=head;
    int rtn, i;
    int ismain = 0;
    if((fp = fopen("test.asm", "w")) == NULL){
        printf("File open failure\n");
        return;
    }
    do{
        fprintf(fp,".text\n");
        fprintf(fp,"jal main\n\n");
        fprintf(fp,"write:\naddi $t1, $0, 4#栈顶上升长度\n");
        fprintf(fp,"add $sp, $sp,$t1#栈上升\nsw $ra,($sp)#返回地址入栈\n");
        fprintf(fp,"add $sp, $sp,$t1#栈上升\naddi $t2, $0, 20#函数总offset\n");
        fprintf(fp,"sw $t2,($sp)#返回地址入栈\nadd $sp, $sp,$t1#栈上升\n");
        fprintf(fp,"sw $fp, ($sp)#调用函数基址保存\nmove $fp, $sp#$fp移动\n");
        fprintf(fp,"add $sp, $sp,$t2#$sp上移\nlw $t1, -12($fp)#取形参\n");
        fprintf(fp,"move $t3, $t1\nsw $t3, 12($fp)#传入对应位置\n");
        fprintf(fp,"lw $t3, 12($fp)\nmove $a0, $t3\n");
        fprintf(fp,"addi $v0, $0,1\nsyscall\n");
        fprintf(fp,"addi $t3,$0,0\nsw $t3, 16($fp)\n");
        fprintf(fp,"lw $v0, 16($fp)#存返回值\nmove $sp, $fp #$sp回退到$fp\n");
        fprintf(fp,"lw $t2, -8($fp)#取返回地址\nmove $ra, $t2#送入$ra\n");
        fprintf(fp,"lw $t1, ($fp)#读取调用函数的栈底\nmove $fp, $t1#回到栈底\n");
        fprintf(fp,"lw $t1, -4($fp)\nadd $t1, $fp,$t1\n");
        fprintf(fp,"move $sp, $t1\njr $ra\n\n");
    }while(0);
    do {
        if (h->opn1.kind==INT)
            sprintf(opnstr1,"#%d",h->opn1.const_int);
        if (h->opn1.kind==FLOAT)
            sprintf(opnstr1,"#%f",h->opn1.const_float);
        if (h->opn1.kind==CHAR)
            sprintf(opnstr1,"#%c",h->opn1.const_float);
        if (h->opn1.kind==ID)
            sprintf(opnstr1,"%s",h->opn1.id);
        if (h->opn2.kind==INT)
            sprintf(opnstr2,"#%d",h->opn2.const_int);
        if (h->opn2.kind==FLOAT)
            sprintf(opnstr2,"#%f",h->opn2.const_float);
        if (h->opn2.kind==CHAR)
            sprintf(opnstr2,"#%c",h->opn2.const_float);
        if (h->opn2.kind==ID)
            sprintf(opnstr2,"%s",h->opn2.id);
        sprintf(resultstr,"%s",h->result.id);
        // fprintf(fp,"\n");
        i = 0;
        switch (h->op) {
            case ASSIGNOP:  if(h->opn1.kind == INT){//x := #k
                                fprintf(fp,"addi $t3,$0,%d\n",h->opn1.const_int);
                                fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);
                            }else if(h->opn1.kind == FLOAT){
                                fprintf(fp,"addi $t3,$0,%d\n",h->opn1.const_float);
                                fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);
                            }else if(h->opn1.kind == CHAR){
                                fprintf(fp,"addi $t3,$0,%d\n",h->opn1.const_char);
                                fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);
                            }else if(h->opn1.kind == ID){//x := y
                                fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                                fprintf(fp,"move $t3, $t1\n");
                                fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);
                            }
                            break;
            case INC:       // v := v + #1
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);//load v
                            fprintf(fp,"addi $t2, $0,1\n");//load 1
                            fprintf(fp,"add $t3, $t1,$t2\n");//add
                            fprintf(fp,"sw $t3, %d($fp)\n",h->opn1.offset);//store into v
                            break;
            case DEC:       // temp := v - #1
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);//load v
                            fprintf(fp,"addi $t2, $0,-1\n");//load -1
                            fprintf(fp,"add $t3, $t1,$t2\n");//add
                            fprintf(fp,"sw $t3, %d($fp)\n",h->opn1.offset);//store into v
                            break;
            case UMINUS:    // temp := #0 - v
                            fprintf(fp,"lw $t3, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"sub $t1, $0,$t3\n");
                            fprintf(fp,"sw $t1, %d($fp)\n",h->result.offset);
                            break;
            case SELFPLUS:  // v := v + #x
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);//load v
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);//load constant
                            fprintf(fp,"add $t3, $t2,$t1\n");//add
                            fprintf(fp,"sw $t3, %d($fp)\n",h->opn1.offset);//store into v
                            fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);//store into temp
                            break;
            case SELFMINUS: // v := v - #x
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);//load v
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);//load constant
                            fprintf(fp,"sub $t3, $t2,$t1\n");//sub
                            fprintf(fp,"sw $t3, %d($fp)\n",h->opn1.offset);//store into v
                            fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);//store into temp
                            break;
            case SELFSTAR:  // v := v * #x
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);//load v
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);//load constant
                            fprintf(fp,"mul $t3, $t2,$t1\n");//multiple
                            fprintf(fp,"sw $t3, %d($fp)\n",h->opn1.offset);//store into v
                            fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);//store into temp
                            break;
            case SELFDIV:   // v := v / #x
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);//load v
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);//load constant
                            fprintf(fp,"div $t1,$t2\n");//div 
                            fprintf(fp,"mflo $t3\n");//div
                            fprintf(fp,"sw $t3, %d($fp)\n",h->opn1.offset);//store into v
                            fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);//store into temp
                            break;
            case PLUS:      // t := vt + vt
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            fprintf(fp,"add $t3, $t2,$t1\n");
                            fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);
                            break;
            case MINUS:     // t := vt - vt
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            fprintf(fp,"sub $t3, $t1,$t2\n");
                            fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);
                            break;
            case STAR:      // t := vt * vt
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            fprintf(fp,"mul $t3, $t1,$t2\n");
                            fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);
                            break;
            case DIV:       // t := vt / vt
                            fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            fprintf(fp,"div $t1,$t2\n");
                            fprintf(fp,"mflo $t3\n");//div
                            fprintf(fp,"sw $t3, %d($fp)\n",h->result.offset);
                            break;
            case FUNCTION:  //打标签
                            rtn = searchSymbolTable(h->result.id);
                            fprintf(fp,"\n%s:\n",h->result.id);
                            if(!strcmp(h->result.id,"main")){
                                fprintf(fp,"addi $t2, $0, 4#栈顶上升高度\n");
                                fprintf(fp,"addi $t1, $0, %d#函数整体长度\n",symbolTable.symbols[rtn].offset + 4);
                                fprintf(fp,"add $sp, $fp,$t1#$sp初始化位置\n");//$sp += 该函数内长度
                                //把$sp在函数中的位置放到$fp里
                                fprintf(fp,"sw $sp, ($fp)#存$sp的正确位置\n");
                                fprintf(fp,"add $fp, $fp, $t2#$fp指向栈底\n");
                                ismain = 1;
                                break;
                            }
                            //把返回地址存入栈
                            fprintf(fp,"addi $t1, $0, 4#栈顶上升长度\n");
                            fprintf(fp,"add $sp, $sp,$t1#栈上升\n");//$sp += 4
                            fprintf(fp,"sw $ra,($sp)#返回地址入栈\n");
                            //该函数的$sp位置入栈
                            fprintf(fp,"add $sp, $sp,$t1#栈上升\n");//$sp += 4
                            fprintf(fp,"addi $t2, $0, %d#函数总offset\n",symbolTable.symbols[rtn].offset);
                            fprintf(fp,"sw $t2,($sp)#返回地址入栈\n");
                            //保存调用函数的$fp
                            fprintf(fp,"add $sp, $sp,$t1#栈上升\n");//$sp += 4
                            fprintf(fp,"sw $fp, ($sp)#调用函数基址保存\n");//保存基址位置
                            //$fp更新到$sp
                            fprintf(fp,"move $fp, $sp#$fp移动\n");
                            //更新$sp
                            fprintf(fp,"add $sp, $sp,$t2#$sp上移\n");//$sp += 该函数内长度
                            //读取形参
                            int stored_param_offset = -4 * (symbolTable.symbols[rtn].paramnum + 2);
                            int new_param_offset = 12;
                            for (i = 0; i < symbolTable.symbols[rtn].paramnum; i++){
                                fprintf(fp,"lw $t1, %d($fp)#取形参\n",stored_param_offset);
                                fprintf(fp,"move $t3, $t1\n");
                                fprintf(fp,"sw $t3, %d($fp)#传入对应位置\n",new_param_offset);
                                stored_param_offset += 4;
                                new_param_offset += 4;
                            }
                            // if(!strcmp(h->result.id,"print")){
                            //     fprintf(fp,"lw $t3, 12($fp)\n");
                            //     fprintf(fp,"move $a0, $t3\n");
                            //     fprintf(fp,"addi $v0, $0,1\n");
                            //     fprintf(fp,"syscall\n");
                            // }
                            // printf("  %s := CALL %s\n",resultstr, opnstr1);
                            break;
            // case PARAM:     printf("  PARAM %s\n",h->result.id);
            //                 break;
            case LABEL:     fprintf(fp,"%s:\n",h->result.id);
                            break;
            case GOTO:      fprintf(fp,"j %s\n",h->result.id);
                            break;
            case JLE:       fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            fprintf(fp,"ble $t1,$t2, %s\n",resultstr);
                            break;
            case JLT:       fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            fprintf(fp,"blt $t1,$t2, %s\n",resultstr);
                            break;
            case JGE:       fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            fprintf(fp,"bge $t1,$t2, %s\n",resultstr);
                            break;
            case JGT:       fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            fprintf(fp,"bgt $t1,$t2, %s\n",resultstr);
                            break;
            case EQ:        fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            if(h->opn2.kind == ID){
                                fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            }else{
                                fprintf(fp,"add $t2, $0,$0\n");
                            }
                            fprintf(fp,"beq $t1,$t2, %s\n",resultstr);
                            break;
            case NEQ:       fprintf(fp,"lw $t1, %d($fp)\n",h->opn1.offset);
                            fprintf(fp,"lw $t2, %d($fp)\n",h->opn2.offset);
                            fprintf(fp,"bne $t1,$t2, %s\n",resultstr);
                            break;
            case ARG:       fprintf(fp,"addi $t1, $0, 4#栈顶提升高度\n");
                            fprintf(fp,"add $sp, $sp,$t1#升栈顶\n");
                            fprintf(fp,"lw $t1, %d($fp)#取参数\n",h->result.offset);
                            fprintf(fp,"sw $t1, ($sp)#压栈\n");
                            break;
            case CALL:      fprintf(fp,"jal %s #转跳被调用函数\n",opnstr1);//保存地址后跳转
                            //调用结束后
                            fprintf(fp,"sw $v0, %d($fp) #返回值存入\n",h->result.offset);
                            break;
            case RETURN:    if(ismain == 1){
                                // fprintf(fp,"+++++++++++\n");
                                ismain = 0;
                                break;
                            }
                            // fprintf(fp,"-------\n");
                            if (h->result.kind)
                                fprintf(fp,"lw $v0, %d($fp)#存返回值\n",h->result.offset);
                            //$sp 回退到 $fp
                            fprintf(fp,"move $sp, $fp #$sp回退到$fp\n");//$fp -> $sp
                            //取返回地址
                            fprintf(fp,"lw $t2, -8($fp)#取返回地址\n");
                            fprintf(fp,"move $ra, $t2#送入$ra\n");
                            //$fp 回退到 调用函数的 $fp
                            fprintf(fp,"lw $t1, ($fp)#读取调用函数的栈底\n");
                            fprintf(fp,"move $fp, $t1#回到栈底\n");
                            //$sp取($sp) 回退
                            fprintf(fp,"lw $t1, -4($fp)\n");
                            fprintf(fp,"add $t1, $fp,$t1\n");//new added
                            fprintf(fp,"move $sp, $t1\n");
                            //返回
                            fprintf(fp,"jr $ra\n");
                            break;
        }
    h=h->next;
    } while (h!=head);
    fprintf(fp,"addi $v0,$0,10\n");
    fprintf(fp,"syscall\n\n");
}

void semantic_error(int line,char *msg1,char *msg2){
    //这里可以只收集错误信息，最后一次显示
    printf("在%d行,%s %s\n",line,msg1,msg2);
}

void prn_symbol(){ //显示符号表
    int i=0;
    char type[10];
    printf("%6s %8s  %7s  %7s %4s %6s\n","变量名","别 名","层 号","类  型","标记","偏移量");
    for(i=0;i<symbolTable.index;i++){
        if(symbolTable.symbols[i].type==INT){
            strcpy(type, "int");
        }else if (symbolTable.symbols[i].type==FLOAT) {
            strcpy(type, "float");
        }else {
            strcpy(type, "char");
        }
        printf("%6s %6s %6d  %6s %4c %6d\n",symbolTable.symbols[i].name,\
                symbolTable.symbols[i].alias,symbolTable.symbols[i].level,\
                type,symbolTable.symbols[i].flag,\
                symbolTable.symbols[i].offset);
    }
}

int searchSymbolTable(char *name) {
    int i,flag=0;
    for(i=symbolTable.index-1;i>=0;i--){
        if (symbolTable.symbols[i].level==0)
            flag=1;
        if (flag && symbolTable.symbols[i].level==1)
            continue;   //跳过前面函数的形式参数表项
        if (!strcmp(symbolTable.symbols[i].name, name))  return i;
    }
    return -1;
}

int fillSymbolTable(char *name,char *alias,int level,int type,char flag,int offset) {
    //首先根据name查符号表，不能重复定义 重复定义返回-1
    int i;
    /*符号查重，考虑外部变量声明前有函数定义，
    其形参名还在符号表中，这时的外部变量与前函数的形参重名是允许的*/
    for(i=symbolTable.index-1; i>=0 && (symbolTable.symbols[i].level==level||level==0); i--) {
        if (level==0 && symbolTable.symbols[i].level==1) continue;  //外部变量和形参不必比较重名
        if (!strcmp(symbolTable.symbols[i].name, name))  return -1;
    }
    //填写符号表内容
    strcpy(symbolTable.symbols[symbolTable.index].name,name);
    strcpy(symbolTable.symbols[symbolTable.index].alias,alias);
    symbolTable.symbols[symbolTable.index].level=level;
    symbolTable.symbols[symbolTable.index].type=type;
    symbolTable.symbols[symbolTable.index].flag=flag;
    symbolTable.symbols[symbolTable.index].offset=offset;
    return symbolTable.index++; //返回的是符号在符号表中的位置序号，中间代码生成时可用序号取到符号别名
}

//填写临时变量到符号表，返回临时变量在符号表中的位置
int fill_Temp(char *name,int level,int type,char flag,int offset) {
    strcpy(symbolTable.symbols[symbolTable.index].name,"");
    strcpy(symbolTable.symbols[symbolTable.index].alias,name);
    symbolTable.symbols[symbolTable.index].level=level;
    symbolTable.symbols[symbolTable.index].type=type;
    symbolTable.symbols[symbolTable.index].flag=flag;
    symbolTable.symbols[symbolTable.index].offset=offset;
    return symbolTable.index++; //返回的是临时变量在符号表中的位置序号
}

void ext_var_list(struct ASTNode *T){  //处理变量列表
    int rtn,num=1;
    struct ASTNode *T0;
    int totallength = 1;
    int flag = 0;
    switch (T->kind){
        case EXT_DEC_LIST:
                T->ptr[0]->type=T->type;              //将类型属性向下传递变量结点
                T->ptr[0]->offset=T->offset;          //外部变量的偏移量向下传递
                T->ptr[1]->type=T->type;              //将类型属性向下传递变量结点
                totallength = 1;
                if (T->ptr[0]->kind == ARRAY){//解决  int a, b[10], c;  的情况
                    //当类型为ARRAY时，记录ARRAY总width，当出现变长数组，产生报错
                    T0 = T->ptr[0];
                    flag = 0;
                    while (T0->kind == ARRAY){
                        if(T0->ptr[1]->kind == INT){
                            totallength *= T0->ptr[1]->type_int; //仅当INT类型定义数组时，记录数组长度
                        } else{
                            flag = 1;//否则报错
                        }
                        T0 = T0->ptr[0];
                    }
                    if (flag){
                        semantic_error(T0->pos,T0->type_id, "数组不支持变长定义");
                    }
                }
                T->ptr[1]->offset=T->offset+T->width*totallength; //外部变量的偏移量向下传递
                T->ptr[1]->width=T->width;
                if (flag != 1){//当已经对变长数组报错后，就不需要进入函数报第二次错
                    ext_var_list(T->ptr[0]);
                }
                ext_var_list(T->ptr[1]);
                T->num= T->ptr[1]->num + totallength;//加上 totallength 才能确定下一行外部定义变量的正确偏移量
                break;
        case ARRAY:
        // 解决  int y[10];  的情况
            T0 = T;
            totallength = 1;
            while (T0->kind == ARRAY){
                T0->ptr[1]->offset = T0->offset;//偏移量传递给数组的一个维度
                if(T0->ptr[1]->kind == INT){
                    totallength *= T0->ptr[1]->type_int; //仅当INT类型定义数组时，记录数组长度
                } else{
                    flag = 1;//否则报错
                }
                semantic_Analysis(T0->ptr[1]);//将数组的长度作为temp存入表中
                T0->ptr[0]->offset = T0->ptr[1]->width + T0->offset;//把下一层ARRAY的offset计算出来
                T0 = T0->ptr[0];
            }
            if (flag){
                semantic_error(T0->pos,T0->type_id, "数组不支持变长定义");
                return;
            }
            rtn = fillSymbolTable(T0->type_id,newAlias(),LEV,T->type,'A',T0->offset);  //offset使用最内侧ARRAY的偏移量
            if (rtn==-1)
                semantic_error(T->pos,T->type_id, "变量重复定义");
            else T->place = rtn;
            T->num=totallength;//考虑数组长度的情况下，下一行外部定义变量的正确偏移量
            break;
        case ID:
            rtn = fillSymbolTable(T->type_id,newAlias(),LEV,T->type,'V',T->offset);  //最后一个变量名
            if (rtn==-1)
                semantic_error(T->pos,T->type_id, "变量重复定义");
            else T->place=rtn;
            T->num=1;
            break;
    }
}

int  match_param(int i,struct ASTNode *T){
    int j,num=symbolTable.symbols[i].paramnum;
    int type1,type2,pos=T->pos;
    T=T->ptr[0];
    if (num==0 && T==NULL) return 1;
    for (j=1;j<=num;j++) {
        if (!T){
            semantic_error(pos,"", "函数调用参数个数少于函数定义");
            return 0;
            }
        type1=symbolTable.symbols[i+j].type;  //形参类型
        type2=T->ptr[0]->type;
        if (type1!=type2){
            semantic_error(pos,"", "参数类型不匹配");
            return 0;
        }
        T=T->ptr[1];
    }
    if (T){ //num个参数已经匹配完，还有实参表达式
        semantic_error(pos,"", "函数调用参数个数超过函数定义");
        return 0;
        }
    return 1;
}

void boolExp(struct ASTNode *T){  //布尔表达式，参考文献[2]p84的思想
  struct opn opn1,opn2,result;
  struct opn opn3,opn4,result1;
  int op;
  int rtn;
  if (T)
	{
	switch (T->kind) {
        case INT:   if(T->type_int){
                        T->code = genGoto(T->Etrue);
                    }else{
                        T->code = genGoto(T->Efalse);
                    }
                    break;
        case FLOAT: if(T->type_float){
                        T->code = genGoto(T->Etrue);
                    }else{
                        T->code = genGoto(T->Efalse);
                    }
                    break;
        case CHAR:  if(T->type_char){
                        T->code = genGoto(T->Etrue);
                    }else{
                        T->code = genGoto(T->Efalse);
                    }
                    break;
        case ID:    rtn = searchSymbolTable(T->type_id);
                    if (rtn==-1){
                        semantic_error(T->pos,T->type_id, "变量未定义");
                    }
                    if (symbolTable.symbols[rtn].flag=='F'){
                        semantic_error(T->pos,T->type_id, "对函数名采用非函数调用形式");
                    }else {
                        T->place=rtn;       //lab3,结点保存变量在符号表中的位置
                        T->code=NULL;       //lab3,标识符不需要生成TAC
                        T->type=symbolTable.symbols[rtn].type;
                        T->offset=symbolTable.symbols[rtn].offset;
                        T->width=0;   //未再使用新单元
                        opn1.kind=ID; 
                        strcpy(opn1.id,symbolTable.symbols[T->place].alias);
                        opn1.offset=symbolTable.symbols[T->place].offset;
                        opn2.const_int = 0;
                        opn2.kind = INT;
                        result.kind=ID; 
                        strcpy(result.id,T->Etrue);
                        op = NEQ;
                        T->code=genIR(op,opn1,opn2,result);
                        T->code=merge(2,T->code,genGoto(T->Efalse));
                    }
                    break;
        case RELOP: //处理关系运算表达式,2个操作数都按基本表达式处理
                    // printf("%d\n",T->offset);
                    T->ptr[0]->offset = T->offset;
                    Exp(T->ptr[0]);
                    T->ptr[1]->offset = T->ptr[0]->width + T->offset;
                    Exp(T->ptr[1]);
                    T->width = T->ptr[1]->width + T->ptr[0]->width;
                    //opn1
                    opn1.kind=ID; 
                    strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                    //opn2
                    opn2.kind=ID; 
                    strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                    opn2.offset=symbolTable.symbols[T->ptr[1]->place].offset;
                    //result
                    result.kind=ID; 
                    strcpy(result.id,T->Etrue);
                    if (strcmp(T->type_id,"<")==0)
                            op=JLT;
                    else if (strcmp(T->type_id,"<=")==0)
                            op=JLE;
                    else if (strcmp(T->type_id,">")==0)
                            op=JGT;
                    else if (strcmp(T->type_id,">=")==0)
                            op=JGE;
                    else if (strcmp(T->type_id,"==")==0)
                            op=EQ;
                    else if (strcmp(T->type_id,"!=")==0)
                            op=NEQ;
                    T->code=genIR(op,opn1,opn2,result);
                    T->code=merge(4,T->ptr[0]->code,T->ptr[1]->code,T->code,genGoto(T->Efalse));
                    break;
        case AND: 
        case OR:
                    if (T->kind==AND){
                        strcpy(T->ptr[0]->Etrue,newLabel());
                        strcpy(T->ptr[0]->Efalse,T->Efalse);
                    }
                    else{
                        strcpy(T->ptr[0]->Etrue,T->Etrue);
                        strcpy(T->ptr[0]->Efalse,newLabel());
                    }
                    strcpy(T->ptr[1]->Etrue,T->Etrue);
                    strcpy(T->ptr[1]->Efalse,T->Efalse);
                    T->ptr[0]->offset = T->offset;
                    boolExp(T->ptr[0]);
                    T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                    boolExp(T->ptr[1]);
                    T->width = T->ptr[0]->width + T->ptr[1]->width;
                    if (T->kind==AND)
                        T->code=merge(3,T->ptr[0]->code,genLabel(T->ptr[0]->Etrue),T->ptr[1]->code);
                    else
                        T->code=merge(3,T->ptr[0]->code,genLabel(T->ptr[0]->Efalse),T->ptr[1]->code);
                    break;
        case NOT:   strcpy(T->ptr[0]->Etrue,T->Efalse);
                    strcpy(T->ptr[0]->Efalse,T->Etrue);
                    boolExp(T->ptr[0]);
                    T->code=T->ptr[0]->code;
                    break;
        case ASSIGNOP:
                    T->place = T->ptr[1]->place;
                    T->ptr[0]->offset = T->offset;
                    boolExp(T->ptr[0]);
                    T->ptr[1]->offset = T->ptr[0]->width + T->offset;
                    boolExp(T->ptr[1]);
                    T->width = T->ptr[1]->width + T->ptr[0]->width;
                    //opn1
                    opn1.kind=ID; 
                    strcpy(opn1.id,symbolTable.symbols[T->ptr[1]->place].alias);
                    opn1.offset=symbolTable.symbols[T->ptr[1]->place].offset;
                    //result
                    result.kind=ID; 
                    strcpy(result.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    result.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                    T->code = merge(3,T->ptr[0]->code,T->ptr[1]->code, genIR(ASSIGNOP,opn1,opn2,result));
                    //opn1
                    opn3.kind=ID; 
                    strcpy(opn3.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    opn3.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                    //opn2
                    opn4.kind = INT;
                    opn4.const_int = 0;
                    //result
                    result1.kind = ID;
                    strcpy(result1.id, T->Etrue);
                    T->code = merge(3,T->code,genIR(NEQ,opn3,opn4,result1),genGoto(T->Efalse));
                    break;
        case INC:   
                    T->place = fill_Temp(newTemp(),LEV,T->type,'T',T->offset);
                    if(T->type == INT){
                        T->width = 4;
                    }else if(T->type == FLOAT){
                        T->width = 8;
                    }else if(T->type == CHAR){
                        T->width = 1;
                    }
                    T->ptr[0]->offset = T->offset + T->width;
                    Exp(T->ptr[0]);
                    T->width += T->ptr[0]->width;
                    opn1.kind=ID; 
                    strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                    //result
                    result.kind=ID; 
                    strcpy(result.id,symbolTable.symbols[T->place].alias);
                    result.offset=symbolTable.symbols[T->place].offset;
                    T->code = merge(2,T->ptr[0]->code, genIR(T->kind,opn1,opn2,result));
                    //opn1
                    opn1.kind=ID; 
                    strcpy(opn1.id,symbolTable.symbols[T->place].alias);
                    opn1.offset=symbolTable.symbols[T->place].offset;
                    //opn2
                    opn2.kind = INT;
                    opn2.const_int = 0;
                    //result
                    result.kind = ID;
                    strcpy(result.id,T->Etrue);
                    T->code = merge(3,T->code,genIR(NEQ,opn1,opn2,result),genGoto(T->Efalse));
                    break;
        case SELFPLUS:
        case SELFMINUS:
        case SELFSTAR:
        case SELFDIV:
        case PLUS:
	    case MINUS:
	    case STAR:
	    case DIV:
                    T->place = fill_Temp(newTemp(),LEV,T->type,'T',T->offset);
                    if(T->type == INT){
                        T->width = 4;
                    }else if(T->type == FLOAT){
                        T->width = 8;
                    }else if(T->type == CHAR){
                        T->width = 1;
                    }
                    T->ptr[0]->offset = T->offset + T->width;
                    Exp(T->ptr[0]);
                    T->ptr[1]->offset = T->ptr[0]->width + T->ptr[0]->offset;
                    Exp(T->ptr[1]);
                    T->width += T->ptr[1]->width + T->ptr[0]->width;
                    //opn1
                    opn1.kind=ID; 
                    strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                    //opn2
                    opn2.kind=ID; 
                    strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                    opn2.offset=symbolTable.symbols[T->ptr[1]->place].offset;
                    //result
                    result.kind=ID; 
                    strcpy(result.id,symbolTable.symbols[T->place].alias);
                    result.offset=symbolTable.symbols[T->place].offset;
                    T->code = merge(3,T->ptr[0]->code,T->ptr[1]->code, genIR(T->kind,opn1,opn2,result));
                    //opn1
                    opn1.kind=ID; 
                    strcpy(opn1.id,symbolTable.symbols[T->place].alias);
                    opn1.offset=symbolTable.symbols[T->place].offset;
                    //opn2
                    opn2.kind = INT;
                    opn2.const_int = 0;
                    //result
                    result.kind = ID;
                    strcpy(result.id,T->Etrue);
                    T->code = merge(3,T->code,genIR(NEQ,opn1,opn2,result),genGoto(T->Efalse));
                    break;
        case UMINUS:
                    T->place = fill_Temp(newTemp(),LEV,T->type,'T',T->offset);
                    if(T->type == INT){
                        T->width = 4;
                    }else if(T->type == FLOAT){
                        T->width = 8;
                    }else if(T->type == CHAR){
                        T->width = 1;
                    }
                    T->ptr[0]->offset = T->offset + T->width;
                    Exp(T->ptr[0]);
                    T->width += T->ptr[0]->width;
                    //opn1
                    opn1.kind=ID; 
                    strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    opn1.offset=symbolTable.symbols[T->ptr[0]->place].offset;
                    //result
                    result.kind=ID; 
                    strcpy(result.id,symbolTable.symbols[T->place].alias);
                    result.offset=symbolTable.symbols[T->place].offset;
                    T->code = merge(2,T->ptr[0]->code, genIR(T->kind, opn1,opn2,result));
                    //opn1
                    opn1.kind=ID; 
                    strcpy(opn1.id,symbolTable.symbols[T->place].alias);
                    opn1.offset=symbolTable.symbols[T->place].offset;
                    //opn2
                    opn2.kind = INT;
                    opn2.const_int = 0;
                    //result
                    result.kind = ID;
                    strcpy(result.id, T->Etrue);
                    T->code = merge(3,T->code, genIR(NEQ,opn1,opn2,result),genGoto(T->Efalse));
        }
	}
}

void Exp(struct ASTNode *T){
    //处理基本表达式，参考文献[2]p82的思想
    int rtn,num,width;
    struct ASTNode *T0;
    struct opn opn1,opn2,result;
    struct opn opn3, opn4, result1, opn5, opn6, result2;
    struct opn opn7, opn8, result3;
    char label1[15], label2[15];
    int flag;
    if (T){
	switch (T->kind) {
	case ID:    //查符号表，获得符号表中的位置，类型送type
                rtn=searchSymbolTable(T->type_id);
                if (rtn==-1)
                    semantic_error(T->pos,T->type_id, "变量未定义");
                if (symbolTable.symbols[rtn].flag=='F')
                    semantic_error(T->pos,T->type_id, "对函数名采用非函数调用形式");
                else {
                    T->place=rtn;       //lab3,结点保存变量在符号表中的位置
                    T->code=NULL;       //lab3,标识符不需要生成TAC
                    T->type=symbolTable.symbols[rtn].type;
                    T->offset=symbolTable.symbols[rtn].offset;
                    T->width=0;   //未再使用新单元
                }
                break;
    case ARRAY: T0 = T;
                flag = 0;
                while(T0->kind == ARRAY){
                    T0->ptr[1]->offset = T0->offset;
                    Exp(T0->ptr[1]);
                    if(T0->ptr[1]->type != INT){
                        flag = 1;
                    }
                    T->width += T0->ptr[1]->width;;
                    T0->ptr[0]->offset = T0->offset + T0->ptr[1]->width;
                    T0 = T0->ptr[0];
                }
                if (flag){
                    semantic_error(T->pos,T0->type_id, "数组变量采用非整型表达式");
                }
                // printf("%s\n",T0->type_id);
                rtn = searchSymbolTable(T0->type_id);
                if(rtn == -1){
                    semantic_error(T->pos,T0->type_id, "变量未定义");
                }
                if (symbolTable.symbols[rtn].flag=='F'){
                    semantic_error(T->pos,T0->type_id, "对函数名采用非函数调用形式");
                }else if(symbolTable.symbols[rtn].flag=='V'){
                    semantic_error(T->pos,T0->type_id, "对非数组变量采用下标变量的形式访问");
                }else {
                    T->place = rtn;
                    T->code=NULL;
                    T->type=symbolTable.symbols[rtn].type;
                    T->offset=symbolTable.symbols[rtn].offset;
                    // T->width=0;   //未再使用新单元
                }
                break;
    case INT:   T->place = fill_Temp(newTemp(),LEV,T->type,'T',T->offset); //lab3,为整常量生成一个临时变量
                // printf("line %d, int %d, offset %d\n",T->pos, T->type_int, T->offset);
                T->type = INT;
                //lab3
                opn1.kind = INT;
                opn1.const_int=T->type_int;
                result.kind = ID; 
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = genIR(ASSIGNOP,opn1,opn2,result);
                T->width = 4;
                break;
    case FLOAT: T->place = fill_Temp(newTemp(),LEV,T->type,'T',T->offset);   //为浮点常量生成一个临时变量
                // printf("line %d float %f, offset %d\n",T->pos, T->type_float, T->offset);
                T->type = FLOAT;
                //lab3
                opn1.kind = FLOAT; 
                opn1.const_float = T->type_float;
                result.kind = ID; 
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = genIR(ASSIGNOP,opn1,opn2,result);
                T->width=8;
                break;
    case CHAR:  T->place = fill_Temp(newTemp(),LEV,T->type,'T',T->offset);   //为字符常量生成一个临时变量
                // printf("line %d, char %c, offset %d\n",T->pos, T->type_char, T->offset);
                T->type = CHAR;
                //lab3
                opn1.kind = CHAR; 
                opn1.const_char = T->type_char;
                result.kind = ID; 
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = genIR(ASSIGNOP,opn1,opn2,result);
                T->width = 1;
                break;
	case ASSIGNOP:
                if (T->ptr[0]->kind != ID && T->ptr[0]->kind != ARRAY){
                    semantic_error(T->pos,"", "赋值语句需要左值");
                }else {
                    T->ptr[0]->offset = T->offset;
                    Exp(T->ptr[0]);   //处理左值，例中仅为变量
                    T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                    Exp(T->ptr[1]);
                    T->type = T->ptr[0]->type;
                    T->width = T->ptr[0]->width + T->ptr[1]->width;
                    T->code = merge(2,T->ptr[0]->code,T->ptr[1]->code);
                    opn1.kind = ID;   
                    strcpy(opn1.id,symbolTable.symbols[T->ptr[1]->place].alias);//右值一定是个变量或临时变量
                    opn1.type = T->ptr[1]->type;
                    opn1.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                    result.kind = ID; 
                    strcpy(result.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    result.type = T->ptr[0]->type;
                    result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                    T->code = merge(2,T->code,genIR(ASSIGNOP,opn1,opn2,result));
                }
                break;
	case AND:   
                T->type = INT;
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                Exp(T->ptr[1]);
                T->width = T->ptr[0]->width + T->ptr[1]->width + 4;
                T->place = fill_Temp(newTemp(),LEV,T -> type,'T',T->offset + T->ptr[0]->width + T->ptr[1]->width);
                strcpy(label1,newLabel());
                //opn1
                opn1.kind = ID; 
                strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type = T->ptr[0]->type;
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                //opn2
                opn2.kind = ID; 
                strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.type = T->ptr[1]->type;
                opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                //result
                result.kind = ID; 
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.offset = symbolTable.symbols[T->place].offset;
                //opn3
                opn3.kind = ID; 
                strcpy(opn3.id,symbolTable.symbols[T->place].alias);
                opn3.offset = symbolTable.symbols[T->place].offset;
                //opn4
                opn4.kind = INT;
                opn4.const_int = 0;
                //result1
                result1.kind = ID;
                strcpy(result1.id,label1);
                //opn5
                opn5.kind = INT;
                opn5.const_int = 1;
                //result2
                result2.kind = ID; 
                strcpy(result2.id,symbolTable.symbols[T->place].alias);
                result2.offset = symbolTable.symbols[T->place].offset;
                T->code = merge(6,T->ptr[0]->code, T->ptr[1]->code, genIR(STAR,opn1,opn2,result),\
                        genIR(EQ,opn3,opn4,result1),genIR(ASSIGNOP,opn5,opn6,result2),\
                        genLabel(label1));
                break;
	case OR:    T->type = INT;
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                Exp(T->ptr[1]);
                T->width = T->ptr[0]->width + T->ptr[1]->width + 4;
                T->place = fill_Temp(newTemp(),LEV,T -> type,'T',T->offset + T->ptr[0]->width + T->ptr[1]->width);
                strcpy(label1,newLabel());
                strcpy(label2,newLabel());
                //opn1
                opn1.kind = INT;
                opn1.const_int = 0;
                //result
                result.kind = ID; 
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.offset = symbolTable.symbols[T->place].offset;
                //opn3
                opn3.kind = ID; 
                strcpy(opn3.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn3.type = T->ptr[0]->type;
                opn3.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                //opn4
                opn4.kind = INT;
                opn4.const_int = 0;
                //result1
                result1.kind = ID;
                strcpy(result1.id, label1);
                //opn5
                opn5.kind = INT;
                opn5.const_int = 1;
                //result2
                result2.kind = ID; 
                strcpy(result2.id,symbolTable.symbols[T->place].alias);
                result2.offset = symbolTable.symbols[T->place].offset;
                //opn7
                opn7.kind = ID; 
                strcpy(opn7.id,symbolTable.symbols[T->ptr[1]->place].alias);
                opn7.type = T->ptr[0]->type;
                opn7.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                //opn8
                opn8.kind = INT;
                opn8.const_int = 0;
                //result3
                result3.kind = ID;
                strcpy(result3.id, label2);
                T->code = merge(10,T->ptr[0]->code,T->ptr[1]->code,genIR(ASSIGNOP,opn1,opn2,result),\
                        genIR(EQ,opn3,opn4,result1),genIR(ASSIGNOP,opn5,opn6,result2),genGoto(label2),\
                        genLabel(label1),genIR(EQ,opn7,opn8,result3),genIR(ASSIGNOP,opn5,opn6,result2),\
                        genLabel(label2));
                break;
	case RELOP: 
                T->type = INT;
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                Exp(T->ptr[1]);
                T->width = T->ptr[0]->width + T->ptr[1]->width + 4;
                T->place = fill_Temp(newTemp(),LEV,T -> type,'T',T->offset + T->ptr[0]->width + T->ptr[1]->width);
                strcpy(label1,newLabel());
                strcpy(label2,newLabel());
                //opn1
                opn1.kind = ID; 
                strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type = T->ptr[0]->type;
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                //opn2
                opn2.kind = ID; 
                strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.type = T->ptr[1]->type;
                opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                //result
                result.kind = ID; 
                strcpy(result.id,label1);
                //opn3
                opn3.kind = INT;
                opn3.const_int = 0;
                //result1
                result1.kind = ID; 
                strcpy(result1.id,symbolTable.symbols[T->place].alias);
                result1.type = T->type;
                result1.offset = symbolTable.symbols[T->place].offset;
                //opn5
                opn5.kind = INT;
                opn5.const_int = 1;
                //result2
                result2.kind = ID; 
                strcpy(result2.id,symbolTable.symbols[T->place].alias);
                result2.type = T->type;
                result2.offset = symbolTable.symbols[T->place].offset;
                int op;
                if (strcmp(T->type_id,"<")==0)
                    op=JLT;
                else if (strcmp(T->type_id,"<=")==0)
                    op=JLE;
                else if (strcmp(T->type_id,">")==0)
                    op=JGT;
                else if (strcmp(T->type_id,">=")==0)
                    op=JGE;
                else if (strcmp(T->type_id,"==")==0)
                    op=EQ;
                else if (strcmp(T->type_id,"!=")==0)
                    op=NEQ;
                T->code = merge(8,T->ptr[0]->code,T->ptr[1]->code,genIR(op,opn1,opn2,result),\
                        genIR(ASSIGNOP,opn3,opn4,result1),genGoto(label2),genLabel(label1),genIR(ASSIGNOP,opn5,opn6,result2)\
                        , genLabel(label2));
                break;
    case SELFPLUS:
    case SELFMINUS:
    case SELFSTAR:
    case SELFDIV:
                if (T->ptr[0]->kind != ID && T->ptr[0]->kind != ARRAY){
                    semantic_error(T->pos,"", "自增自减语句需要左值");
                }
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                Exp(T->ptr[1]);
                //ptr[1]的width是常数的width
                T->width = T->ptr[0]->width + T->ptr[1]->width;
                if (T->ptr[0]->type == FLOAT && T->ptr[1]->type == FLOAT){
                    T->type = FLOAT;
                    T->width += 8;//此处的增量是变量自增后暂存所需要的的width
                }else if (T->ptr[0]->type == INT && T->ptr[1]->type == INT){
                    T->type = INT;
                    T->width += 4;
                }else if (T->ptr[0]->type == CHAR && T->ptr[1]->type == CHAR){
                    T->type = CHAR;
                    T->width += 1;
                }else {
                    semantic_error(T->pos,"", "运算符两端类型不匹配");
                    return;
                }
                T->place = fill_Temp(newTemp(),LEV,T -> type,'T',T->offset + T->ptr[0]->width + T->ptr[1]->width);
                //opn1
                opn1.kind = ID; 
                strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type = T->ptr[0]->type;
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                //opn2
                opn2.kind = ID; 
                strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.type = T->ptr[1]->type;
                opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                //result
                result.kind = ID; 
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.type = T->type;
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = merge(3,T->ptr[0]->code,T->ptr[1]->code,genIR(T->kind,opn1,opn2,result));
                break;
	case PLUS:
	case MINUS:
	case STAR:
	case DIV:   T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                Exp(T->ptr[1]);
                //判断T->ptr[0]，T->ptr[1]类型是否正确，可能根据运算符生成不同形式的代码，给T的type赋值
                if (T->ptr[0]->type == FLOAT && T->ptr[1]->type == FLOAT){
                    T->type = FLOAT;
                    T->width = T->ptr[0]->width + T->ptr[1]->width + 8;
                }else if (T->ptr[0]->type == INT && T->ptr[1]->type == INT){
                    T->type = INT;
                    T->width = T->ptr[0]->width + T->ptr[1]->width + 4;
                }else if (T->ptr[0]->type == CHAR && T->ptr[1]->type == CHAR){
                    T->type = CHAR;
                    T->width = T->ptr[0]->width + T->ptr[1]->width + 1;
                }else {
                    semantic_error(T->pos,"", "运算符两端类型不匹配");
                    return;
                }
                T->place = fill_Temp(newTemp(),LEV,T -> type,'T',T->offset + T->ptr[0]->width + T->ptr[1]->width);
                //opn1
                opn1.kind = ID; 
                strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type = T->ptr[0]->type;
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                //opn2
                opn2.kind = ID; 
                strcpy(opn2.id,symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.type = T->ptr[1]->type;
                opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                //result
                result.kind = ID; 
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.type = T->type;
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = merge(3,T->ptr[0]->code,T->ptr[1]->code,genIR(T->kind,opn1,opn2,result));
                // T->width = T->ptr[0]->width + T->ptr[1]->width + (T->type==INT?4:8);
                break;
    case INC:
    case DEC:   T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                if (T->ptr[0]->type == FLOAT){
                    T->type = FLOAT;
                    T->width = T->ptr[0]->width + 8;
                }else if (T->ptr[0]->type == INT){
                    T->type = INT;
                    T->width = T->ptr[0]->width + 4;
                }else if (T->ptr[0]->type == CHAR){
                    T->type = CHAR;
                    T->width = T->ptr[0]->width + 1;
                }
                T->place = fill_Temp(newTemp(),LEV,T->type,'T',T->offset + T->ptr[0]->width);
                opn1.kind = ID; 
                strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type = T->ptr[0]->type;
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                result.kind = ID; 
                strcpy(result.id,symbolTable.symbols[T->ptr[0]->place].alias);
                result.type = T->type;
                result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                result1.kind = ID;
                strcpy(result1.id,symbolTable.symbols[T->place].alias);
                result1.type = T->type;
                result1.offset = symbolTable.symbols[T->place].offset;
                T->code = merge(3,T->ptr[0]->code,genIR(T->kind,opn1,opn2,result),genIR(ASSIGNOP,opn1,opn2,result1));
                break;
	case NOT:   T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                if (T->ptr[0]->type == FLOAT){
                    T->type = FLOAT;
                    T->width = T->ptr[0]->width + 8;
                }else if (T->ptr[0]->type == INT){
                    T->type = INT;
                    T->width = T->ptr[0]->width + 4;
                }else if (T->ptr[0]->type == CHAR){
                    T->type = CHAR;
                    T->width = T->ptr[0]->width + 1;
                }
                T->place = fill_Temp(newTemp(),LEV,T->type,'T',T->offset + T->ptr[0]->width);
                strcpy(label1,newLabel());
                strcpy(T->Snext,newLabel());
                //opn1
                opn1.kind = ID; 
                strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type = T->ptr[0]->type;
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                //opn2
                opn2.kind = INT;
                opn2.const_int = 0;
                //result
                result1.kind = ID;
                strcpy(result.id, label1);
                //opn3
                opn3.kind = INT;
                opn3.const_int = 0;
                //result1
                result1.kind = ID; 
                strcpy(result1.id,symbolTable.symbols[T->place].alias);
                result1.type = T->type;
                result1.offset = symbolTable.symbols[T->place].offset;
                //opn5
                opn5.kind = INT;
                opn5.const_int = 1;
                //opn4 is empty
                T->code = merge(7,T->ptr[0]->code,genIR(EQ,opn1,opn2,result),genIR(ASSIGNOP,opn3,opn4,result1),\
                        genGoto(T->Snext),genLabel(label1),genIR(ASSIGNOP,opn5,opn6,result1),genLabel(T->Snext));
                break;
	case UMINUS:T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                if (T->ptr[0]->type == FLOAT){
                    T->type = FLOAT;
                    T->width = T->ptr[0]->width + 8;
                }else if (T->ptr[0]->type == INT){
                    T->type = INT;
                    T->width = T->ptr[0]->width + 4;
                }else if (T->ptr[0]->type == CHAR){
                    T->type = CHAR;
                    T->width = T->ptr[0]->width + 1;
                }
                T->place = fill_Temp(newTemp(),LEV,T->type,'T',T->offset + T->ptr[0]->width);
                opn1.kind = ID; 
                strcpy(opn1.id,symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type = T->ptr[0]->type;
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                result.kind = ID; 
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.type = T->type;
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = merge(2,T->ptr[0]->code,genIR(T->kind,opn1,opn2,result));
                break;
    case FUNC_CALL: //根据T->type_id查出函数的定义，如果语言中增加了实验教材的read，write需要单独处理一下
                rtn=searchSymbolTable(T->type_id);
                if (rtn==-1){
                    semantic_error(T->pos,T->type_id, "函数未定义");
                    break;
                }
                if (symbolTable.symbols[rtn].flag!='F'){
                    semantic_error(T->pos,T->type_id, "对非函数名采用函数调用");
                     break;
                }
                T->type=symbolTable.symbols[rtn].type;
                if (T->type == INT) {
                    width = 4;
                }else if (T->type == FLOAT) {
                    width = 8;
                }else if (T->type == CHAR) {
                    width = 1;
                }else{}
                // width=T->type==INT?4:8;   //存放函数返回值的单数字节数
                if (T->ptr[0]){
                    T->ptr[0]->offset=T->offset;
                    Exp(T->ptr[0]);       //处理所有实参表达式求值，及类型
                    T->width=T->ptr[0]->width+width; //累加上计算实参使用临时变量的单元数
                    T->code=T->ptr[0]->code;
                }
                else {T->width=width; T->code=NULL;}
                match_param(rtn,T);   //处理所有参数的匹配
                    //处理参数列表的中间代码
                T0=T->ptr[0];
                while (T0) {
                    result.kind=ID;  strcpy(result.id,symbolTable.symbols[T0->ptr[0]->place].alias);
                    result.offset=symbolTable.symbols[T0->ptr[0]->place].offset;
                    T->code=merge(2,T->code,genIR(ARG,opn1,opn2,result));
                    T0=T0->ptr[1];
                }
                T->place=fill_Temp(newTemp(),LEV,T->type,'T',T->offset+T->width-width);
                opn1.kind=ID;     
                strcpy(opn1.id,T->type_id);  //保存函数名
                opn1.offset=rtn;  //这里offset用以保存函数定义入口,在目标代码生成时，能获取相应信息
                result.kind=ID;   
                strcpy(result.id,symbolTable.symbols[T->place].alias);
                result.offset=symbolTable.symbols[T->place].offset;
                T->code=merge(2,T->code,genIR(CALL,opn1,opn2,result)); //生成函数调用中间代码
                break;
    case ARGS:      //此处仅处理各实参表达式的求值的代码序列，不生成ARG的实参系列
                T->ptr[0]->offset=T->offset;
                Exp(T->ptr[0]);
                T->width=T->ptr[0]->width;
                T->code=T->ptr[0]->code;
                if (T->ptr[1]) {
                    T->ptr[1]->offset=T->offset+T->ptr[0]->width;
                    Exp(T->ptr[1]);
                    T->width+=T->ptr[1]->width;
                    T->code=merge(2,T->code,T->ptr[1]->code);
                    }
                break;
         }
      }
}

int isreturnexist = 0;
int findreturn(struct ASTNode *T){
    int i;
    for(i = 0; i < 3; i ++){
        if(T->ptr[i]){
            if(T->ptr[i]->kind == RETURN){
                isreturnexist = 1;
            }else{
                findreturn(T->ptr[i]);
            }
        }
    }
    
}

void semantic_Analysis(struct ASTNode *T){
  //对抽象语法树的先根遍历,按display的控制结构修改完成符号表管理和语义检查和TAC生成（语句部分）
    int rtn,num,width;
    struct ASTNode *T0, *T1;
    struct opn opn1,opn2,result;
    char label1[15];
    if (T){
	switch (T->kind) {
	case EXT_DEF_LIST://外部定义列表
    // [0]: ExtDef, [1]: ExtDefList or [0] == NULL
            if (!T->ptr[0]){
                break;
            }
            T->ptr[0]->offset = T->offset;   //继承偏移量
            semantic_Analysis(T->ptr[0]);    //访问外部定义列表中的第一个
            T->code=T->ptr[0]->code;
            if (T->ptr[1]){//外部定义列表还没结束
                T->ptr[1]->offset = T->ptr[0]->offset + T->ptr[0]->width;
                semantic_Analysis(T->ptr[1]); //访问该外部定义列表中的其它外部定义
                T->code=merge(2,T->code,T->ptr[1]->code);
            }
            break;
	case EXT_VAR_DEF:   //处理外部说明,将第一个孩子(TYPE结点)中的类型送到第二个孩子的类型域
    // [0]: Specifier, [1]: ExtDecList, [2]: SEMI
            if(!strcmp(T->ptr[0]->type_id,"int")){
                T->type = T->ptr[1]->type = INT;
            }else if(!strcmp(T->ptr[0]->type_id,"float")){
                T->type = T->ptr[1]->type = FLOAT;
            }else if (!strcmp(T->ptr[0]->type_id,"char")){
                T->type = T->ptr[1]->type = CHAR;
            }else {
                
            }//以上将Specifier的类型传给之后定义的变量
            // 不考虑CHAR的情况:
            // T->type=T->ptr[1]->type=!strcmp(T->ptr[0]->type_id,"int")?INT:FLOAT;
            T->ptr[1]->offset = T->offset;        //这个外部变量的偏移量向下传递
            if(T->type == INT){
                T->ptr[1]->width = 4;
            }else if(T->type == FLOAT){
                T->ptr[1]->width = 8;
            }else if(T->type == CHAR){
                T->ptr[1]->width = 1;
            }else{
                // T->ptr[1]->width = struct_type_process(T->ptr[0]->ptr[0]);
            }//将一个变量的宽度向后传递
            ext_var_list(T->ptr[1]);            //处理外部变量说明中的标识符序列
            T->width = T->ptr[1]->width * T->ptr[1]->num;
            //计算这个外部变量说明的宽度
            // T->width=(T->type==INT?4:8)* T->ptr[1]->num; 
            T->code=NULL;             //这里假定外部变量不支持初始化
            break;
	case FUNC_DEF:      //填写函数定义信息到符号表,
    // [0]:Specifier, [1]:FuncDec, [2]:CompSt
            if (!strcmp(T->ptr[0]->type_id,"int")) {
                T->ptr[1]->type = INT;
            } else if (!strcmp(T->ptr[0]->type_id,"float")) {
                T->ptr[1]->type = FLOAT;
            } else if (!strcmp(T->ptr[0]->type_id,"char")) {
                T->ptr[1]->type = CHAR;
            } else{
                semantic_error(T->pos,T->ptr[1]->type_id, "函数返回类型错误");
            }
            //获取函数返回类型送到含函数名、参数的结点
            // T->ptr[1]->type=!strcmp(T->ptr[0]->type_id,"int")?INT:FLOAT;
            T->width = 0;     //函数的宽度设置为0，不会对外部变量的地址分配产生影响
            T->offset = DX;   //设置局部变量在活动记录中的偏移量初值
            semantic_Analysis(T->ptr[1]); //处理函数名和参数结点部分，这里不考虑用寄存器传递参数
            T->offset += T->ptr[1]->width;   //用形参单元宽度修改函数局部变量的起始偏移量
            T->ptr[2]->offset = T->offset;
            strcpy(T->ptr[2]->Snext,newLabel());  //函数体语句执行结束后的位置属性
            isreturnexist = 0;
            findreturn(T->ptr[2]);
            if (isreturnexist == 0){
                semantic_error(T->pos,T->ptr[1]->type_id, "函数无返回值");
            }
            semantic_Analysis(T->ptr[2]);         //处理函数体结点
            //计算活动记录大小,这里offset属性存放的是活动记录大小，不是偏移
            symbolTable.symbols[T->ptr[1]->place].offset = T->offset + T->ptr[2]->width;
            T->code=merge(3,T->ptr[1]->code,T->ptr[2]->code,genLabel(T->ptr[2]->Snext));          //函数体的代码作为函数的代码
            break;
	case FUNC_DEC:      //根据返回类型，函数名填写符号表
    // [0]: VarList or NULL
            rtn = fillSymbolTable(T->type_id,newAlias(),LEV,T->type,'F',0);
            //函数不在数据区中分配单元，偏移量为0
            if (rtn == -1){
                semantic_error(T->pos,T->type_id, "函数重复定义");
                break;
            }
            else T->place = rtn;//rtn 是符号表的index
            result.kind=ID;   
            strcpy(result.id,T->type_id);
            result.offset=rtn;
            T->code=genIR(FUNCTION,opn1,opn2,result);     //生成中间代码：FUNCTION 函数名
            T->offset=DX;   //设置形式参数在活动记录中的偏移量初值
            if (T->ptr[0]) { //判断是否有参数
                T->ptr[0]->offset=T->offset;
                semantic_Analysis(T->ptr[0]);  //处理函数参数列表
                T->width=T->ptr[0]->width;
                symbolTable.symbols[rtn].paramnum=T->ptr[0]->num;
                T->code=merge(2,T->code,T->ptr[0]->code);  //连接函数名和参数代码序列
                }
            else symbolTable.symbols[rtn].paramnum=0,T->width=0;
            break;
	case PARAM_LIST:    //处理函数形式参数列表
    // [0]: ParamDec, [1]: VarList or NULL
            T->ptr[0]->offset = T->offset;
            semantic_Analysis(T->ptr[0]);
            if (T->ptr[1]){
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                semantic_Analysis(T->ptr[1]);
                T->num = T->ptr[0]->num + T->ptr[1]->num;        //统计参数个数
                T->width = T->ptr[0]->width + T->ptr[1]->width;  //累加参数单元宽度
                T->code = merge(2,T->ptr[0]->code,T->ptr[1]->code);  //连接参数代码
            }
            else {
                T->num = T->ptr[0]->num;
                T->width = T->ptr[0]->width;
                T->code = T->ptr[0]->code;
            }
            break;
	case  PARAM_DEC:
    // [0]: Specifier, [1]: VarDec
            //PARAM_DEC是函数形式参数，不用考虑ARRAY的情况。
            rtn = fillSymbolTable(T->ptr[1]->type_id,newAlias(),1,T->ptr[0]->type,'P',T->offset);
            if (rtn == -1){
                semantic_error(T->ptr[1]->pos,T->ptr[1]->type_id, "参数名重复定义");
            } else {
                T->ptr[1]->place=rtn;
            }
            T->num=1;       //参数个数计算的初始值
            if (T->ptr[0]->type == INT) {
                T->width = 4;
            }else if (T->ptr[0]->type == FLOAT) {
                T->width = 8;
            }else {
                T->width = 1;
            }
            // T->width = T->ptr[0]->type==INT? 4:8;  //参数宽度
            result.kind=ID;   
            strcpy(result.id, symbolTable.symbols[rtn].alias);
            result.offset=T->offset;
            T->code=genIR(PARAM,opn1,opn2,result);     //生成：FUNCTION 函数名
            break;
	case COMP_STM:
    // [0]: DefList, [1]: StmList
            LEV++;
            //设置层号加1，并且保存该层局部变量在符号表中的起始位置在symbol_scope_TX
            symbol_scope_TX.TX[symbol_scope_TX.top++] = symbolTable.index;
            T->width = 0;
            T->code = NULL;
            if (T->ptr[0]) {
                T->ptr[0]->offset = T->offset;
                semantic_Analysis(T->ptr[0]);  //处理该层的局部变量DEF_LIST
                T->width += T->ptr[0]->width;
                T->code = T->ptr[0]->code;
            }
            if (T->ptr[1]){
                strcpy(T->ptr[1]->Ebreak,T->Ebreak);
                strcpy(T->ptr[1]->Econtinue,T->Econtinue);
                T->ptr[1]->offset = T->offset+T->width;
                strcpy(T->ptr[1]->Snext, T->Snext);  //S.next属性向下传递
                semantic_Analysis(T->ptr[1]);       //处理复合语句的语句序列
                T->width += T->ptr[1]->width;
                T->code = merge(2,T->code,T->ptr[1]->code);
            }
             #if (DEBUG)
                prn_symbol();       //c在退出一个符合语句前显示的符号表
			//   system("pause");
             #endif
             LEV--;    //出复合语句，层号减1
             symbolTable.index = symbol_scope_TX.TX[--symbol_scope_TX.top]; //删除该作用域中的符号
             break;
    case DEF_LIST:
    // NULL or [0]:Def, [1]:DefList
            T->code = NULL;
            if (T->ptr[0]){
                T->ptr[0]->offset = T->offset;
                semantic_Analysis(T->ptr[0]);   //处理一个局部变量定义
                T->code = T->ptr[0]->code;
                T->width += T->ptr[0]->width;
            }
            if (T->ptr[1]) {
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                semantic_Analysis(T->ptr[1]);   //处理剩下的局部变量定义
                T->code = merge(2,T->code,T->ptr[1]->code);
                T->width += T->ptr[1]->width;
            }
            break;
    case VAR_DEF://处理一个局部变量定义,将第一个孩子(TYPE结点)中的类型送到第二个孩子的类型域
    // [0]:Specifier, [1]:DecList
                T->code = NULL;
                //根据类型确定变量列表内变量类型，用于传递
                if (!strcmp(T->ptr[0]->type_id,"int")) {
                    T->ptr[1]->type = INT;
                } else if (!strcmp(T->ptr[0]->type_id,"float")) {
                    T->ptr[1]->type = FLOAT;
                } else {
                    T->ptr[1]->type = CHAR;
                }
                //确定变量序列各变量类型
                T0 = T->ptr[1]; //T0为变量名列表子树根指针，对ID、ASSIGNOP类结点在登记到符号表，作为局部变量
                //T0's kind is DecList
                num = 0;
                T0->offset = T->offset;
                T->width=0;
                if (T->ptr[1]->type==INT) {
                    width = 4;
                }else if (T->ptr[1]->type==FLOAT){
                    width = 8;
                }else{
                    width = 1;
                }//一个变量宽度
                while (T0) {  //处理所有DEC_LIST结点
                // [0]:Dec, [1]:DecList or NULL
                    num++;
                    T0->offset = T->offset + T->width;
                    T0->ptr[0]->type = T0->type;  //类型属性向下传递
                    if (T0->ptr[1]) {
                        T0->ptr[1]->type = T0->type;
                    }
                    //首先判断T0->ptr[0]作为VarDec的kind是ID或ARRAY，不考虑变长数组
                    T1 = T0->ptr[0];
                    T1->offset = T0->offset;//偏移量传递
                    int totallength = 1;//记录多维数组总的长度
                    int flag = 0;
                    while (T1->kind == ARRAY){
                        T1->ptr[1]->offset = T1->offset;
                        if(T1->ptr[1]->kind == INT){
                            totallength *= T1->ptr[1]->type_int; //仅当INT类型定义数组时，记录数组长度
                        } else{
                            flag = 1;//否则报错
                        }
                        semantic_Analysis(T1->ptr[1]);//将数组的长度作为temp存入表中
                        T->width += T1->ptr[1]->width;//数组各维度长度作为temp也算作VAR_DEF节点长度
                        T1->ptr[0]->offset = T1->ptr[1]->width + T1->offset;
                        T1 = T1->ptr[0];
                    }
                    //若Dec是ARRAY，则T1指向最内侧ID，同时offset中包含了各维度常量的width
                    //若Dec是ID，则也指向ID
                    if (flag){
                        semantic_error(T1->pos,T1->type_id, "数组不支持变长定义");
                    }
                    if (T0->ptr[0]->kind == ID){//不能用T1因为T1可能经过while == ARRAY 而都为ID
                        //加入符号表
                        rtn = fillSymbolTable(T1->type_id,newAlias(),LEV,T1->type,'V',T1->offset);
                        //此处偏移量未计算，暂时为0
                        if (rtn==-1){
                            semantic_error(T1->pos,T1->type_id, "变量重复定义");
                        }else {
                            T1->place=rtn;
                        }
                        T->width += width;
                    }else if(T0->ptr[0]->kind == ARRAY){
                        //考虑ARRAY, 禁止定义变长数组
                        rtn = fillSymbolTable(T1->type_id,newAlias(),LEV,T1->type,'A',T1->offset);
                        //此处偏移量未计算，暂时为0
                        if (rtn==-1){
                            semantic_error(T1->pos,T1->type_id, "变量重复定义");
                        }else {
                            T1->place=rtn;
                        }
                        T->width += width * totallength;
                    }else if (T0->ptr[0]->kind == ASSIGNOP){//变量声明的同时赋值
                        if(T1->ptr[0]->kind == ARRAY){//here
                            semantic_error(T1->ptr[0]->pos," ", "禁止对数组赋值");
                        }
                        rtn = fillSymbolTable(T1->ptr[0]->type_id,newAlias(),LEV,T1->type,'V',T->offset+T->width);
                        //此处偏移量未计算，暂时为0
                        // semantic_Analysis
                        if (rtn==-1)
                            semantic_error(T1->ptr[0]->pos,T1->ptr[0]->type_id, "变量重复定义");
                        else {
                            T1->place = rtn;
                            T1->ptr[1]->offset = T->offset+T->width+width;
                            Exp(T1->ptr[1]);
                            opn1.kind = ID; 
                            strcpy(opn1.id,symbolTable.symbols[T1->ptr[1]->place].alias);
                            opn1.offset = symbolTable.symbols[T1->ptr[1]->place].offset;
                            result.kind = ID; 
                            strcpy(result.id,symbolTable.symbols[T1->place].alias);
                            result.offset = symbolTable.symbols[T1->place].offset;
                            T->code = merge(3,T->code,T1->ptr[1]->code,genIR(ASSIGNOP,opn1,opn2,result));
                        }
                        //width是被的变量的长度，T1->ptr[1]->width是所赋值的长度
                        T->width += width + T1->ptr[1]->width;
                    }
                    T0=T0->ptr[1];//回到While句首，处理剩下Dec
                }
                break;
	case STM_LIST:
    // [0]: Stmt, [1]: StmList
                if (!T->ptr[0]) { 
                    T->code=NULL; 
                    T->width=0; 
                    break;
                }   //空语句序列
                if (T->ptr[1]){ //2条以上语句连接，生成新标号作为第一条语句结束后到达的位置
                    strcpy(T->ptr[0]->Snext,newLabel());
                }else {    //语句序列仅有一条语句，S.next属性向下传递
                    strcpy(T->ptr[0]->Snext,T->Snext);
                }
                strcpy(T->ptr[0]->Ebreak,T->Ebreak);
                strcpy(T->ptr[0]->Econtinue,T->Econtinue);
                T->ptr[0]->offset = T->offset;
                semantic_Analysis(T->ptr[0]);
                T->code = T->ptr[0]->code;
                T->width += T->ptr[0]->width;
                if (T->ptr[1]){     //2条以上语句连接,S.next属性向下传递
                    strcpy(T->ptr[1]->Snext,T->Snext);
                    // T->ptr[1]->offset = T->offset;  //顺序结构共享单元方式
                    T->ptr[1]->offset=T->offset+T->ptr[0]->width; //顺序结构顺序分配单元方式
                    strcpy(T->ptr[1]->Ebreak,T->Ebreak);
                    strcpy(T->ptr[1]->Econtinue,T->Econtinue);
                    semantic_Analysis(T->ptr[1]);
                    //序列中第1条为表达式语句，返回语句，复合语句时，第2条前不需要标号
                    if (T->ptr[0]->kind==RETURN ||T->ptr[0]->kind==EXP_STMT ||T->ptr[0]->kind==COMP_STM){
                        T->code = merge(2,T->code,T->ptr[1]->code);
                    } else {
                        T->code=merge(3,T->code,genLabel(T->ptr[0]->Snext),T->ptr[1]->code);
                    }
                    // if (T->ptr[1]->width>T->width) {
                    //     T->width = T->ptr[1]->width; //顺序结构共享单元方式
                    // }
                    T->width+=T->ptr[1]->width;//顺序结构顺序分配单元方式
                }
                break;
	case IF_THEN:
                strcpy(T->ptr[0]->Etrue,newLabel());  //设置条件语句真假转移位置
                strcpy(T->ptr[0]->Efalse,T->Snext);
                T->ptr[0]->offset = T->offset;
                boolExp(T->ptr[0]);
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                strcpy(T->ptr[1]->Snext,T->Snext);
                strcpy(T->ptr[1]->Ebreak,T->Ebreak);
                strcpy(T->ptr[1]->Econtinue,T->Econtinue);
                semantic_Analysis(T->ptr[1]);      //if子句
                T->width = T->ptr[0]->width + T->ptr[1]->width;
                T->code = merge(3,T->ptr[0]->code, genLabel(T->ptr[0]->Etrue),T->ptr[1]->code);
                break;  //控制语句都还没有处理offset和width属性
	case IF_THEN_ELSE:
                strcpy(T->ptr[0]->Etrue,newLabel());  //设置条件语句真假转移位置
                strcpy(T->ptr[0]->Efalse,newLabel());
                strcpy(T->Snext,newLabel());
                T->ptr[0]->offset = T->offset;
                boolExp(T->ptr[0]);      //条件，要单独按短路代码处理
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                strcpy(T->ptr[1]->Snext,T->Snext);
                strcpy(T->ptr[1]->Ebreak,T->Ebreak);
                strcpy(T->ptr[1]->Econtinue,T->Econtinue);
                semantic_Analysis(T->ptr[1]);      //if子句
                T->ptr[2]->offset = T->ptr[1]->offset + T->ptr[1]->width;
                strcpy(T->ptr[2]->Snext,T->Snext);
                strcpy(T->ptr[2]->Ebreak,T->Ebreak);
                strcpy(T->ptr[2]->Econtinue,T->Econtinue);
                semantic_Analysis(T->ptr[2]);      //else子句
                T->width = T->ptr[0]->width + T->ptr[1]->width + T->ptr[2]->width;
                T->code=merge(6,T->ptr[0]->code,genLabel(T->ptr[0]->Etrue),T->ptr[1]->code,\
                              genGoto(T->Snext),genLabel(T->ptr[0]->Efalse),T->ptr[2]->code);
                break;
	case WHILE: strcpy(T->ptr[0]->Etrue,newLabel());  //子结点继承属性的计算
                strcpy(T->ptr[0]->Efalse,T->Snext);
                T->ptr[0]->offset = T->offset;
                boolExp(T->ptr[0]);      //循环条件，要单独按短路代码处理
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                strcpy(T->ptr[1]->Snext,newLabel());
                INLOOP ++;
                strcpy(T->ptr[1]->Ebreak,T->Snext);
                strcpy(T->ptr[1]->Econtinue,T->ptr[1]->Snext);
                semantic_Analysis(T->ptr[1]);      //循环体
                INLOOP --;
                T->width = T->ptr[0]->width + T->ptr[1]->width;
                T->code = merge(5,genLabel(T->ptr[1]->Snext),T->ptr[0]->code, \
                genLabel(T->ptr[0]->Etrue),T->ptr[1]->code,genGoto(T->ptr[1]->Snext));
                break;
    case FOR:   strcpy(T->ptr[1]->Etrue,newLabel());
                strcpy(T->ptr[1]->Efalse,T->Snext);
                strcpy(T->ptr[3]->Ebreak,T->Snext);
                strcpy(label1,newLabel());
                // strcpy(T->ptr[3]->Snext,newLabel());
                strcpy(T->ptr[2]->Snext,newLabel());
                strcpy(T->ptr[3]->Econtinue,label1);
                strcpy(T->ptr[3]->Snext,label1);
                T->ptr[0]->offset = T->offset;
                semantic_Analysis(T->ptr[0]);
                T->ptr[1]->offset = T->ptr[0]->offset + T->ptr[0]->width;
                boolExp(T->ptr[1]);
                T->ptr[2]->offset = T->ptr[1]->offset + T->ptr[1]->width;
                semantic_Analysis(T->ptr[2]);
                T->ptr[3]->offset = T->ptr[3]->offset + T->ptr[3]->width;
                INLOOP ++;
                semantic_Analysis(T->ptr[3]);//循环体
                INLOOP --;
                T->width = T->ptr[0]->width + T->ptr[1]->width + T->ptr[2]->width + T->ptr[3]->width;
                T->code = merge(8,T->ptr[0]->code,genLabel(T->ptr[2]->Snext),T->ptr[1]->code,\
                        genLabel(T->ptr[1]->Etrue),T->ptr[3]->code,genLabel(label1) ,T->ptr[2]->code,\
                        genGoto(T->ptr[2]->Snext));
                break;
    case EXP_STMT:
                T->ptr[0]->offset = T->offset;
                strcpy(T->ptr[0]->Ebreak,T->Ebreak);
                strcpy(T->ptr[0]->Econtinue,T->Econtinue);
                semantic_Analysis(T->ptr[0]);
                T->code = T->ptr[0]->code;
                T->width = T->ptr[0]->width;
                break;
	case RETURN:if (T->ptr[0]){
                    // printf("%d\n",T->offset);
                    T->ptr[0]->offset = T->offset;
                    Exp(T->ptr[0]);
				 /*需要判断返回值类型是否匹配*/
                    int idx = symbolTable.index;
                    while(symbolTable.symbols[idx].flag != 'F'){
                        idx --;
                    }
                    if (T->ptr[0]->type != symbolTable.symbols[idx].type){
                        semantic_error(T->pos,"RETURN", "返回值类型与函数定义不符");
                    }
                    T->width = T->ptr[0]->width;
                    result.kind=ID; 
                    strcpy(result.id,symbolTable.symbols[T->ptr[0]->place].alias);
                    result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                    T->code = merge(2,T->ptr[0]->code,genIR(RETURN,opn1,opn2,result));
                }else{
                    T->width = 0;
                    result.kind = 0;
                    T->code = genIR(RETURN,opn1,opn2,result);
                }
                break;
    case BREAK: 
                if(INLOOP == 0){
                    semantic_error(T->pos, "BREAK", "在循环语句外使用break");
                }
                T->code = genGoto(T->Ebreak);
                break;
    case CONTINUE:
                if(INLOOP == 0){
                    semantic_error(T->pos, "CONTINUE", "在循环语句外使用continue");
                }
                T->code = genGoto(T->Econtinue);
                break;
	case ID:
    case ARRAY:
    case INT:
    case FLOAT:
    case CHAR:
	case ASSIGNOP:
	case AND:
	case OR:
	case RELOP:
	case PLUS:
	case MINUS:
	case STAR:
	case DIV:
    case INC:
    case DEC:
    case SELFPLUS:
    case SELFMINUS:
    case SELFSTAR:
    case SELFDIV:
	case NOT:
	case UMINUS:
    case FUNC_CALL:
                    Exp(T);          //处理基本表达式
                    break;
    }
    }
}

void semantic_Analysis0(struct ASTNode *T) {
    symbolTable.index=0;
    fillSymbolTable("read","",0,INT,'F',4);
    symbolTable.symbols[0].paramnum=0;//read的形参个数
    fillSymbolTable("write","",0,INT,'F',4);
    symbolTable.symbols[1].paramnum=1;
    fillSymbolTable("x","",1,INT,'P',12);
    symbol_scope_TX.TX[0]=0;  //外部变量在符号表中的起始序号为0
    symbol_scope_TX.top=1;
    T->offset=0;              //外部变量在数据区的偏移量
    semantic_Analysis(T);
    prnIR(T->code);
    objectCode(T->code);
} 
