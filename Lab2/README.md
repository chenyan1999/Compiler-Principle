# README about Lab 2

## SymbolTable.c的新增内容

1. 增加了对**任意位置 Char 类型**的符号表添加
2. 在函数 *ext_var_list()* 中增加了对 **外部定义数组** 的符号表添加
3. 在函数 *semantic_Analysis()* 的 *case VAR_DEF* 中增加了对 **内部定义数组** 的符号表添加
4. 在函数 *Exp()* 中增加了 *case ARRAY* ，以支持数组在基本表达式中的使用，但不支持对溢出、维数错误等问题的检测
5. 在函数 *Exp()* 中添加了 *case INC, case DEC, case SELFPLUS, case SELFMINUS, case SELFMNUS, case SLEFSTAR, case SELFDIV* ，以支持新增的运算在基本表达式中的使用，其中对除了 *INC, DEC* 外的运算增加了两侧类型相同的检测。
6. 在函数 *Exp()* 中补充了 *case NOT, case UMINUS*
7. 在函数 *semantic_Analysis()* 中增加了 *case FOR*
8. 支持对临时数偏移量的计算
9. 未能实现结构类型的管理

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
    * +、-、*、/
    * 比较运算
    * 逻辑运算
    * **++、--**
    * **+=、-=、*=、/=**
    
* 控制语句：
    * if-else
    * while
    * **break、continue**
    * **for**
    
* 其他：
    * 单行注释//
    * 多行注释/**/