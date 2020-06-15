# README about Lab 3

## 新增的支持生成中间代码的语法

* 基本表达式

    1. **+=, -=, *=, /=** ：在 *prnIR()* 中新增 *case SELFPLUS,case SELFMINUS,case SELFSTAR,case SELFDIV * 的内容

    2. **++, --** ：在 *prnIR()* 中新增 *case INC,case DEC* 的内容

    3. **UMINUS/-** ：在 *prnIR()* 中新增 *case UMINUS*，在*Exp()* 中补充 *case UMINUS* 的内容

    4. **RELOP** ：在*Exp()* 中补充了 *case RELOP* 的内容

    5. **AND, OR, NOT** ：在 *Exp()* 中补充了 *case AND,case OR,case NOT* 的内容

* 布尔表达式
    1. **int, float, char**：在 *boolExp()* 中新增 *case INT,case FLOAT,case CHAR *的内容
    2. **ID（变量）**：在 *boolExp()* 中新增 *case ID*  的内容
    3. **ASSIGNOP** ：在 *boolExp()* 中新增 *case ASSIGNOP* 的内容
    4. **+, -, *,  /**：在 *boolExp()* 中新增 *case PLUS,case MINUS,case STAR,case DIV* 的内容
    5. **+=, -=, *=, /= ** ：在 *boolExp()* 中新增*case SELFPLUS,case SELFMINUS,case SELFSTAR,case SELFDIV* 的内容
    6. **++, --** ：在 *boolExp()* 中新增 *case INC,case DEC* 的内容
    7. **UMINUS/-** ：在*boolExp()* 中补充 *case UMINUS* 的内容
* 语句类节点
    1. **for** ：在*semantic_Analysis()* 中补充 *case FOR* 的内容
    2. **break, continue** ：增加Ebreak和Econtinue。

## mini-c支持的内容（加粗内容为自定义内容）

* 数据类型：
  
    * int
    
    * float
    
    * **char**
    
    * **任意维数的数组**，在变量声明中只支持常量长度的定长数组，只在表达式中支持数组内的表达式计算，如：
    
        ```c
        int x[5][2];
        int i = 2;
        x[1][1] = x[1+2][i --];
        ```
    
    * ~~**struct结构**~~
    
* 基本运算：
    * +, -, *, /
    * 比较运算
    * 逻辑运算
    * **++, --**
    * **+=, -=, *=, /=**
    
* 控制语句：
    * if-else
    * while
    * **break, continue**
    * **for**
    
* 其他：
    * 单行注释//
    * 多行注释/**/