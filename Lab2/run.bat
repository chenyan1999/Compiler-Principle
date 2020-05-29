flex lex.l
bison -d -v parser.y
gcc -o parser lex.yy.c SymbleTable.c parser.tab.c
parser test.c