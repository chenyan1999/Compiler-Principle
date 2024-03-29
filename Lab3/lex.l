%{
#include "parser.tab.h"
#include "string.h"
#include "def.h"
int yycolumn=1;
#define YY_USER_ACTION    	yylloc.first_line=yylloc.last_line=yylineno; \
	yylloc.first_column=yycolumn;	yylloc.last_column=yycolumn+yyleng-1; \
	yycolumn+=yyleng;
typedef union {
	int type_int; //存储变量具体的值
	int type_float;
	int type_char;
	char type_id[32];//变量的名字
	struct node *ptr;
} YYLVAL;
#define YYSTYPE YYLVAL

%}
%option yylineno
id   [A-Za-z][A-Za-z0-9]*  
int    [0-9]+
float  ([0-9]*\.[0-9]+)|([0-9]+\.)
char  '.'

%%
{int}        {yylval.type_int=atoi(yytext); return INT;}
{float}      {yylval.type_float=atof(yytext); return FLOAT;}
{char}       {yylval.type_char=yytext[1];return CHAR;}
"int"        {strcpy(yylval.type_id,  yytext);return TYPE;}
"float"      {strcpy(yylval.type_id,  yytext);return TYPE;}
"char"		 {strcpy(yylval.type_id,  yytext);return TYPE;}

"return"     {return RETURN;}
"if"         {return IF;}
"else"       {return ELSE;}
"while"      {return WHILE;}
"for"		 {return FOR;}
"break"		 {return BREAK;}
"continue"	 {return CONTINUE;}
"switch"	 {return SWITCH;}
"case"		 {return CASE;}
"default"	 {return DEFAULT;}
"struct"	{return STRUCT;}

{id}       {strcpy(yylval.type_id,  yytext); return ID;/*由于关键字的形式也符合标识符的规则，所以把关键字的处理全部放在标识符的前面，优先识别*/}
";"			{return SEMI;}
","			{return COMMA;}
">"|"<"|">="|"<="|"=="|"!=" {strcpy(yylval.type_id, yytext);;return RELOP;}
"="			{return ASSIGNOP;}
"+"			{return PLUS;}
"-"			{return MINUS;}
"*"			{return STAR;}
"/"			{return DIV;}
"++"        {return INC;}
"--"        {return DEC;}
"+="		{return SELFPLUS;}
"-="		{return SELFMINUS;}
"*="		{return SELFSTAR;}
"/="		{return SELFDIV;}
"&&"        {return AND;}
"||"        {return OR;}
"!"			{return NOT;}
"("			{return LP;/*parenthesis*/}
")"			{return RP;}
"["			{return LB;/*bracket*/}
"]"			{return RB;}
"{"			{return LC;}
"}"			{return RC;}
":"			{return COLON;}
"."			{return DOT;}
[\n]        {yycolumn=1;}   
[ \r\t]     {}   
.			{printf("Error type A :Mysterious character \"%s\"\n\t at Line %d\n",yytext,yylineno);}

"//"[^\n]*  {}
"/*"(.*|[\n])*"*/" {}
%%

/* 和bison联用时，不需要这部分
void main()
{
yylex();
return 0;
}
*/

int yywrap()
{
return 1;
}
