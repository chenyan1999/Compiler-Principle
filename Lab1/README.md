# README about Lab 1

## lex.l的新增内容

1. 增加了对char类型的识别

2. 增加了对for类型的识别

3. 增加了对break和continue的识别

4. 增加了对struct的识别

5. 增加了对++、--、+=、-=、*=、/=的识别

6. 增加了对//、/* */两种注释的识别

7. 增加了对 : (冒号)、. (点)、[ (左中括号)、] (右中括号)的识别

    

## parser.y的新增内容

1. 增加了非终结符的语义值类型：StructSpecifier和StructName
2. 增加了终结符的语义类型：CHAR、FOR、COLON 、BREAK、CONTINUE、DOT
3. 增加了AST的节点类型：ARRAY、STRUCT、INC、DEC、SELFPLUS、SLEFMINUS、SELFSTAR、SELFDIV、VARIABLE、STRUCT_TYPE、STRUCT_DEF 和 STRUCT_VISIT

4. 新增了文法如下：

    1. Specifier $\to \space \cdots$  StructSpecifier

    2. VarDec $\to \space \cdots |$ VarDec LB Exp RB

    3. Stmt $\to \space \cdots |$ FOR LP Exp SEMI Exp SEMI Exp RP Stmt

    4. Exp $\to \space \cdots$ | VarDec | CHAR | Exp INC | Exp DEC | INC Exp | DEC Exp | VarDec SELFPLUS Exp | VarDec SELFMINUS Exp |  VarDec SELFSTAR Exp |  VarDec SELFDIV Exp | BREAK | CONTINUE | Exp DOT ID 
       * 将Exp $\to$ ID 和新增的Exp $\to$ Exp LB Exp RB合并为Exp $\to$ VarDec

    1. StructSpecifier $\to$ STRUCT StructName LC ExtDefList RC | STRUCT ID

    2. StructName $\to$ ID| $\epsilon$

        

## ast.c的新增内容

1. 新增函数semantic_Analysis0、semantic_Analysis用于展示二元单词序列

2. 在函数display的switch-case语句中新增：STRUCT_TYPE、FOR、CHAR、ARRAY、VARIABLE、BREAK、CONTINUE、STRUCT_DEF、STRUCT_VISIT、SELFPLUS、SELFMINUS的识别

   
## mini-c支持的内容

* 数据类型：
    * int
    
    * float
    
    * char
    
    * 任意维数的数组，支持数组内的表达式计算，如：
    
        ```c
        int x = 3;
        int array[1+2][x];
        ```
    
    * struct结构
* 基本运算：
    * +、-、*、/
    * 比较运算
    * 逻辑运算
    * ++、--
    * +=、-=、*=、/=
* 控制语句：
    * if-else
    * while
    * break、continue
    * for
* 其他：
    * 单行注释//
    * 多行注释/**/