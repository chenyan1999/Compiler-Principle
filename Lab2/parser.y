%error-verbose
%locations
%{
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "def.h"
extern int yylineno;
extern char *yytext;
extern FILE *yyin;
void yyerror(const char* fmt, ...);
extern void display(struct ASTNode *,int);
extern struct ASTNode * mknode(int num,int kind,int pos,...);
%}

%union {
	int    type_int;
	float  type_float;
    char   type_char;
	char   type_id[32];
	struct ASTNode *ptr;
};

//  %type 定义非终结符的语义值类型
%type  <ptr> program ExtDefList ExtDef Specifier ExtDecList FuncDec CompSt VarList VarDec ParamDec Stmt StmList DefList Def DecList Dec Exp Args 
// %type  <ptr> CaseStmtList0 CaseStmtList
%type  <ptr> StructSpecifier StructName
//% token 定义终结符的语义值类型
%token <type_int> INT              /*指定INT的语义值是type_int，有词法分析得到的数值*/
%token <type_id> ID  RELOP TYPE    /*指定ID,RELOP 的语义值是type_id，有词法分析得到的标识符字符串*/
%token <type_float> FLOAT          /*指定ID的语义值是type_id，有词法分析得到的标识符字符串*/
%token <type_char> CHAR            
%token LP RP LB RB LC RC SEMI COMMA      /*用bison对该文件编译时，带参数-d，生成的.tab.h中给这些单词进行编码，可在lex.l中包含parser.tab.h使用这些单词种类码*/
%token PLUS MINUS STAR DIV ASSIGNOP AND OR NOT IF ELSE WHILE RETURN FOR BREAK CONTINUE DOT
%token SWITCH CASE COLON DEFAULT /*未使用到的类型*/
// %token CaseStmtList0 CaseStmtList 
/*以下为接在上述token后依次编码的枚举常量，作为AST结点类型标记*/
%token EXT_DEF_LIST EXT_VAR_DEF FUNC_DEF FUNC_DEC EXT_DEC_LIST PARAM_LIST PARAM_DEC VAR_DEF DEC_LIST DEF_LIST COMP_STM STM_LIST EXP_STMT IF_THEN IF_THEN_ELSE
%token FUNC_CALL ARGS FUNCTION PARAM ARG CALL LABEL GOTO JLT JLE JGT JGE EQ NEQ
%token ARRAY STRUCT INC DEC SELFPLUS SELFMINUS SELFSTAR SELFDIV VARIABLE STRUCT_DEF STRUCT_TYPE STRUCT_VISIT
// %token CASE_STMT_LIST CASE_STMT_LIST0 /*未使用到的节点类型*/

%left INT FLOAT ID
%left STRUCT
%left ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS SELFPLUS SELFMINUS
%left STAR DIV SELFSTAR SELFDIV
%left LB RB
%right UMINUS NOT INC DEC
%left DOT


%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE

%%
program:    ExtDefList   {display($1,0); semantic_Analysis0($1);}     //显示语法树,语义分析
            ;          
ExtDefList: {$$=NULL;}
            | ExtDef ExtDefList   {$$=mknode(2,EXT_DEF_LIST,yylineno,$1,$2);}   //每一个EXTDEFLIST的结点，其第1棵子树对应一个外部变量声明或函数
            ; 
ExtDef:     Specifier ExtDecList SEMI   {$$=mknode(2,EXT_VAR_DEF,yylineno,$1,$2);}   //该结点对应一个外部变量声明
            | Specifier FuncDec CompSt   {$$=mknode(3,FUNC_DEF,yylineno,$1,$2,$3);}         //该结点对应一个函数定义
            | error SEMI   {$$=NULL;}
            ;
Specifier:  TYPE   {$$=mknode(0,TYPE,yylineno);
                    strcpy($$->type_id,$1); 
                    if(!strcmp($1,"int")) $$->type = INT;
                    if(!strcmp($1,"float")) $$->type = FLOAT;
                    if(!strcmp($1,"char")) $$->type = CHAR;}
            | StructSpecifier {$$=mknode(1,STRUCT_TYPE,yylineno,$1);}
            ;      
ExtDecList: VarDec   {$$=$1;}       /*每一个EXT_DECLIST的结点，其第一棵子树对应一个变量名(ID类型的结点),第二棵子树对应剩下的外部变量名*/
            | VarDec COMMA ExtDecList   {$$=mknode(2,EXT_DEC_LIST,yylineno,$1,$3);}
            ;  
VarDec:     ID   {$$=mknode(0,ID,yylineno);strcpy($$->type_id,$1);}   //ID结点，标识符符号串存放结点的type_id
            | VarDec LB Exp RB {$$=mknode(2,ARRAY,yylineno,$1,$3);strcpy($$->type_id,"ARRAY");} //数组类型的定义
            ;
FuncDec:    ID LP VarList RP   {$$=mknode(1,FUNC_DEC,yylineno,$3);strcpy($$->type_id,$1);}//函数名存放在$$->type_id
	        | ID LP RP   {$$=mknode(0,FUNC_DEC,yylineno);strcpy($$->type_id,$1);$$->ptr[0]=NULL;}//函数名存放在$$->type_id
            ;  
VarList:    ParamDec  {$$=mknode(1,PARAM_LIST,yylineno,$1);}
            | ParamDec COMMA  VarList  {$$=mknode(2,PARAM_LIST,yylineno,$1,$3);}
            ;
ParamDec:   Specifier VarDec   {$$=mknode(2,PARAM_DEC,yylineno,$1,$2);}
            ;
CompSt:     LC DefList StmList RC   {$$=mknode(2,COMP_STM,yylineno,$2,$3);}
            ;
StmList:    {$$=NULL;}  
            | Stmt StmList   {$$=mknode(2,STM_LIST,yylineno,$1,$2);}
            ;
Stmt:       Exp SEMI   {$$=mknode(1,EXP_STMT,yylineno,$1);}
            | CompSt   {$$=$1;}      //复合语句结点直接最为语句结点，不再生成新的结点
            | RETURN Exp SEMI   {$$=mknode(1,RETURN,yylineno,$2);}
            | IF LP Exp RP Stmt %prec LOWER_THEN_ELSE   {$$=mknode(2,IF_THEN,yylineno,$3,$5);}
            | IF LP Exp RP Stmt ELSE Stmt   {$$=mknode(3,IF_THEN_ELSE,yylineno,$3,$5,$7);}
            | WHILE LP Exp RP Stmt   {$$=mknode(2,WHILE,yylineno,$3,$5);}
            | FOR LP Exp SEMI Exp SEMI Exp RP Stmt {$$=mknode(4,FOR,yylineno,$3,$5,$7,$9);}
            ;
DefList:    {$$=NULL; }
            | Def DefList   {$$=mknode(2,DEF_LIST,yylineno,$1,$2);}
            | error SEMI   {$$=NULL;}
            ;
Def:        Specifier DecList SEMI   {$$=mknode(2,VAR_DEF,yylineno,$1,$2);}
            ;
DecList:    Dec   {$$=mknode(1,DEC_LIST,yylineno,$1);}
            | Dec COMMA DecList   {$$=mknode(2,DEC_LIST,yylineno,$1,$3);}
	        ;
Dec:        VarDec   {$$=$1;}
            | VarDec ASSIGNOP Exp   {$$=mknode(2,ASSIGNOP,yylineno,$1,$3);strcpy($$->type_id,"ASSIGNOP");}
            ;
Exp:        Exp ASSIGNOP Exp {$$=mknode(2,ASSIGNOP,yylineno,$1,$3);strcpy($$->type_id,"ASSIGNOP");}//$$结点type_id空置未用，正好存放运算符
            | Exp AND Exp   {$$=mknode(2,AND,yylineno,$1,$3);strcpy($$->type_id,"AND");}
            | Exp OR Exp    {$$=mknode(2,OR,yylineno,$1,$3);strcpy($$->type_id,"OR");}
            | Exp RELOP Exp {$$=mknode(2,RELOP,yylineno,$1,$3);strcpy($$->type_id,$2);}  //词法分析关系运算符号自身值保存在$2中
            | Exp PLUS Exp  {$$=mknode(2,PLUS,yylineno,$1,$3);strcpy($$->type_id,"PLUS");}
            | Exp MINUS Exp {$$=mknode(2,MINUS,yylineno,$1,$3);strcpy($$->type_id,"MINUS");}
            | Exp STAR Exp  {$$=mknode(2,STAR,yylineno,$1,$3);strcpy($$->type_id,"STAR");}
            | Exp DIV Exp   {$$=mknode(2,DIV,yylineno,$1,$3);strcpy($$->type_id,"DIV");}           
            | LP Exp RP     {$$=$2;}
            | MINUS Exp %prec UMINUS   {$$=mknode(1,UMINUS,yylineno,$2);strcpy($$->type_id,"UMINUS");}
            | NOT Exp       {$$=mknode(1,NOT,yylineno,$2);strcpy($$->type_id,"NOT");}
            | ID LP Args RP {$$=mknode(1,FUNC_CALL,yylineno,$3);strcpy($$->type_id,$1);}
            | ID LP RP      {$$=mknode(0,FUNC_CALL,yylineno);strcpy($$->type_id,$1);}
            // | Exp LB Exp RB {$$=mknode(2, ARRAY, yylineno, $1, $3); strcpy($$->type_id,"ARRAY");} //数组
            // | ID            {$$=mknode(0,ID,yylineno);strcpy($$->type_id,$1);}
            | VarDec        {$$=mknode(1,VARIABLE,yylineno, $1);}//数组或变量
            | INT           {$$=mknode(0,INT,yylineno);$$->type_int=$1;$$->type=INT;}
            | FLOAT         {$$=mknode(0,FLOAT,yylineno);$$->type_float=$1;$$->type=FLOAT;}
            | CHAR          {$$=mknode(0,CHAR,yylineno);$$->type_char=$1;$$->type=CHAR;}
            | Exp INC       {$$=mknode(1,INC,yylineno,$1);strcpy($$->type_id,"INC");}
            | Exp DEC       {$$=mknode(1,DEC,yylineno,$1);strcpy($$->type_id,"DEC");}
            | INC Exp       {$$=mknode(1,INC,yylineno,$2);strcpy($$->type_id,"INC");}
            | DEC Exp       {$$=mknode(1,DEC,yylineno,$2);strcpy($$->type_id,"DEC");}
            | VarDec SELFPLUS Exp  {$$=mknode(2,SELFPLUS,yylineno,$1,$3);strcpy($$->type_id,"SELFPLUS");}
            | VarDec SELFMINUS Exp {$$=mknode(2,SELFMINUS,yylineno,$1,$3);strcpy($$->type_id,"SELFMINUS");}
            | VarDec SELFSTAR Exp {$$=mknode(2,SELFSTAR,yylineno,$1,$3);strcpy($$->type_id,"SELFSTAR");}
            | VarDec SELFDIV Exp {$$=mknode(2,SELFDIV,yylineno,$1,$3);strcpy($$->type_id,"SELFDIV");}
            | BREAK         {$$=mknode(0,BREAK,yylineno);strcpy($$->type_id,"BREAK");}
            | CONTINUE      {$$=mknode(0,CONTINUE,yylineno);strcpy($$->type_id,"CONTINUE");}
            | Exp DOT ID    {$$=mknode(2,STRUCT_VISIT,yylineno,$1,$3);$$->ptr[1]=mknode(0,ID,yylineno);strcpy($$->ptr[1]->type_id,$3);}
            // | SWITCH LP Exp RP LC CaseStmtList RC {$$=mknode(2,SWITCH,yylineno,$3,$6);strcpy($$->type_id,"SWITCH");}
            ;
// CaseStmtList0: LC CaseStmtList RC {$$=mknode(1,CASE_STMT_LIST0,yylineno,$2);}
//             ;
// CaseStmtList: {$$=NULL;}  
//             | CASE INT COLON StmList CaseStmtList {$$=mknode(3,CASE_STMT_LIST,yylineno,$2,$4,$5);}
//             | DEFAULT COLON StmList               {$$=mknode(1,DEFAULT,yylineno,$3);strcpy($$->type_id,"DEFAULT");}  
//             ;
Args:       Exp COMMA Args    {$$=mknode(2,ARGS,yylineno,$1,$3);}
            | Exp               {$$=mknode(1,ARGS,yylineno,$1);}
            ;
StructSpecifier:
            STRUCT StructName LC ExtDefList RC {$$=mknode(2,STRUCT_DEF,yylineno,$2,$4);}
            | STRUCT ID {$$=mknode(1,ID,yylineno,$2);strcpy($$->type_id,$2);}
            ;
StructName: ID {$$=mknode(1,ID,yylineno,$1);strcpy($$->type_id,$1);}
            | {$$ = NULL;}
            ;
%%

int main(int argc, char *argv[]){
	yyin=fopen(argv[1],"r");
	if (!yyin) return;
	yylineno=1;
	yyparse();
	return 0;
	}

#include<stdarg.h>
void yyerror(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Grammar Error at Line %d Column %d: ", yylloc.first_line,yylloc.first_column);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}
