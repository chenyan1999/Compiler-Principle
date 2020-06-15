.text
jal main

write:
addi $t1, $0, 4#栈顶上升长度
add $sp, $sp,$t1#栈上升
sw $ra,($sp)#返回地址入栈
add $sp, $sp,$t1#栈上升
addi $t2, $0, 20#函数总offset
sw $t2,($sp)#返回地址入栈
add $sp, $sp,$t1#栈上升
sw $fp, ($sp)#调用函数基址保存
move $fp, $sp#$fp移动
add $sp, $sp,$t2#$sp上移
lw $t1, -12($fp)#取形参
move $t3, $t1
sw $t3, 12($fp)#传入对应位置
lw $t3, 12($fp)
move $a0, $t3
addi $v0, $0,1
syscall
addi $t3,$0,0
sw $t3, 16($fp)
lw $v0, 16($fp)#存返回值
move $sp, $fp #$sp回退到$fp
lw $t2, -8($fp)#取返回地址
move $ra, $t2#送入$ra
lw $t1, ($fp)#读取调用函数的栈底
move $fp, $t1#回到栈底
lw $t1, -4($fp)
add $t1, $fp,$t1
move $sp, $t1
jr $ra


fibo:
addi $t1, $0, 4#栈顶上升长度
add $sp, $sp,$t1#栈上升
sw $ra,($sp)#返回地址入栈
add $sp, $sp,$t1#栈上升
addi $t2, $0, 64#函数总offset
sw $t2,($sp)#返回地址入栈
add $sp, $sp,$t1#栈上升
sw $fp, ($sp)#调用函数基址保存
move $fp, $sp#$fp移动
add $sp, $sp,$t2#$sp上移
lw $t1, -12($fp)#取形参
move $t3, $t1
sw $t3, 12($fp)#传入对应位置
addi $t3,$0,1
sw $t3, 24($fp)
lw $t1, 12($fp)
lw $t2, 24($fp)
beq $t1,$t2, label3
j label6
label6:
addi $t3,$0,2
sw $t3, 28($fp)
lw $t1, 12($fp)
lw $t2, 28($fp)
beq $t1,$t2, label3
j label4
label3:
addi $t3,$0,1
sw $t3, 32($fp)
lw $v0, 32($fp)#存返回值
move $sp, $fp #$sp回退到$fp
lw $t2, -8($fp)#取返回地址
move $ra, $t2#送入$ra
lw $t1, ($fp)#读取调用函数的栈底
move $fp, $t1#回到栈底
lw $t1, -4($fp)
add $t1, $fp,$t1
move $sp, $t1
jr $ra
j label5
label4:
addi $t3,$0,1
sw $t3, 36($fp)
lw $t1, 12($fp)
lw $t2, 36($fp)
sub $t3, $t1,$t2
sw $t3, 40($fp)
addi $t1, $0, 4#栈顶提升高度
add $sp, $sp,$t1#升栈顶
lw $t1, 40($fp)#取参数
sw $t1, ($sp)#压栈
jal fibo #转跳被调用函数
sw $v0, 44($fp) #返回值存入
addi $t3,$0,2
sw $t3, 48($fp)
lw $t1, 12($fp)
lw $t2, 48($fp)
sub $t3, $t1,$t2
sw $t3, 52($fp)
addi $t1, $0, 4#栈顶提升高度
add $sp, $sp,$t1#升栈顶
lw $t1, 52($fp)#取参数
sw $t1, ($sp)#压栈
jal fibo #转跳被调用函数
sw $v0, 56($fp) #返回值存入
lw $t1, 44($fp)
lw $t2, 56($fp)
add $t3, $t2,$t1
sw $t3, 60($fp)
lw $t1, 60($fp)
move $t3, $t1
sw $t3, 16($fp)
lw $v0, 16($fp)#存返回值
move $sp, $fp #$sp回退到$fp
lw $t2, -8($fp)#取返回地址
move $ra, $t2#送入$ra
lw $t1, ($fp)#读取调用函数的栈底
move $fp, $t1#回到栈底
lw $t1, -4($fp)
add $t1, $fp,$t1
move $sp, $t1
jr $ra
label5:
lw $v0, 12($fp)#存返回值
move $sp, $fp #$sp回退到$fp
lw $t2, -8($fp)#取返回地址
move $ra, $t2#送入$ra
lw $t1, ($fp)#读取调用函数的栈底
move $fp, $t1#回到栈底
lw $t1, -4($fp)
add $t1, $fp,$t1
move $sp, $t1
jr $ra
label1:

main:
addi $t2, $0, 4#栈顶上升高度
addi $t1, $0, 52#函数整体长度
add $sp, $fp,$t1#$sp初始化位置
sw $sp, ($fp)#存$sp的正确位置
add $fp, $fp, $t2#$fp指向栈底
addi $t3,$0,1
sw $t3, 24($fp)
lw $t1, 24($fp)
move $t3, $t1
sw $t3, 20($fp)
label12:
addi $t3,$0,8
sw $t3, 28($fp)
lw $t1, 20($fp)
lw $t2, 28($fp)
blt $t1,$t2, label10
j label9
label10:
addi $t1, $0, 4#栈顶提升高度
add $sp, $sp,$t1#升栈顶
lw $t1, 20($fp)#取参数
sw $t1, ($sp)#压栈
jal fibo #转跳被调用函数
sw $v0, 0($fp) #返回值存入
lw $t1, 0($fp)
move $t3, $t1
sw $t3, 12($fp)
addi $t1, $0, 4#栈顶提升高度
add $sp, $sp,$t1#升栈顶
lw $t1, 12($fp)#取参数
sw $t1, ($sp)#压栈
jal write #转跳被调用函数
sw $v0, 4($fp) #返回值存入
label11:
lw $t1, 20($fp)
addi $t2, $0,1
add $t3, $t1,$t2
sw $t3, 20($fp)
lw $t1, 20($fp)
move $t3, $t1
sw $t3, 32($fp)
j label12
label9:
addi $t3,$0,1
sw $t3, 44($fp)
label8:
addi $v0,$0,10
syscall

