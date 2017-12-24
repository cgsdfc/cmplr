.data
int_1: .word 0
char_1: .byte 0
int_array_1: .space 40
char_array_1: .space 20
int_2: .word 0
int_array_2: .space 8
char_2: .byte 0
char_array_2: .space 3
__asciiz_5: .asciiz "zero equals to zero"
__asciiz_6: .asciiz "A: prints int_arg_2"
__asciiz_7: .asciiz "B: prints char_arg_3"
__asciiz_8: .asciiz "Nothing to do"
__asciiz_14: .asciiz "Calls int_function_noargs returns "
__asciiz_15: .asciiz "void_function_noargs_2 does loop "
__asciiz_23: .asciiz "Move disk "
__asciiz_24: .asciiz " from "
__asciiz_25: .asciiz " to "
__asciiz_26: .asciiz "Move disk "
__asciiz_27: .asciiz "from "
__asciiz_28: .asciiz "to "
__asciiz_31: .asciiz "End of recursive_function_3"
__asciiz_54: .asciiz "Enter a number to select one function to run:"
__asciiz_55: .asciiz "  `0': int_function: takes one int arg and returns a local int variable"
__asciiz_56: .asciiz "  `1': int_function_noargs: takes no args and returns a somehow sum of something"
__asciiz_57: .asciiz "  `2': void_function: take in a char as command and displays the other args according to that command char"
__asciiz_58: .asciiz "                      if that char is 'a', it displays the second arg. if the char is 'b', it displays the third arg"
__asciiz_59: .asciiz "  `3': void_function_noargs: takes and returns nothing. the most boring function."
__asciiz_60: .asciiz "  `4': void_function_noargs_2: scanfs an int and does different things according to the inputted int."
__asciiz_61: .asciiz "  `5': recursive_function_1: scanfs an int and recursively computes the factual of it"
__asciiz_62: .asciiz "  `6': recursive_function_3: scanfs an int and does different recursions according to the input"
__asciiz_63: .asciiz "  `7': recursive_function_2: scanfs an int and plays HANOI game with you."
__asciiz_64: .asciiz "  `8': int_array_do_sth: swaps the int_array_1 and int_array_2, element-wise"
__asciiz_65: .asciiz "  `9': swap_sth: swaps int_1 and int_2"
__asciiz_66: .asciiz "  `h': shows this menu argain."
__asciiz_67: .asciiz "  `q': quits"
__asciiz_68: .asciiz "Swaps int_array_1 and int_array_2"
__asciiz_72: .asciiz "Swaps int_1 and int_2"
__asciiz_73: .asciiz "const_int_decl="
__asciiz_74: .asciiz "const_int_1="
__asciiz_75: .asciiz "const_char_add="
__asciiz_76: .asciiz "const_char_number="
__asciiz_77: .asciiz "(^_^)"
__asciiz_78: .asciiz "int_function takes one int. please input that:"
__asciiz_79: .asciiz "int_function returns "
__asciiz_80: .asciiz "int_function_noargs returns "
__asciiz_81: .asciiz "void_function takes one char. please input that:"
__asciiz_82: .asciiz "void_function_noargs does nothing."
__asciiz_83: .asciiz "void_function_noargs_2 takes one int. please input that:"
__asciiz_84: .asciiz "recursive_function_1 takes one int. please input that:"
__asciiz_85: .asciiz "recursive_function_3 takes one int. please input that:"
__asciiz_86: .asciiz "recursive_function_2 takes one int. please input that:"
__asciiz_87: .asciiz "Not understood. type `h'."
.text
.globl main
	jal main
	li $v0, 10
	syscall

int_function:
	addi $sp, $sp, -32
	sw $s0, 0($sp)
	sw $s1, 4($sp)
	li $t0, 2
	addi $s0, $sp, 8
	sw $t0, 0($s0)
	li $t1, 98
	addi $s1, $sp, 12
	sb $t1, 0($s1)
	addi $s0, $sp, 8
	lw $s0, 0($s0)
	li $t2, 1
	add $t3, $s0, $t2
	move $v0, $t3
	j __label_0

__label_0:
	lw $s0, 0($sp)
	lw $s1, 4($sp)
	addi $sp, $sp, 32
	jr $ra

int_function_noargs:
	addi $sp, $sp, -48
	sw $s0, 0($sp)
	sw $s1, 4($sp)
	li $t0, 0
	addi $s1, $sp, 12
	sw $t0, 0($s1)
	li $t1, 1
	addi $s0, $sp, 8
	sw $t1, 0($s0)
	j __label_1

__label_3:
	addi $s0, $sp, 8
	lw $s0, 0($s0)
	li $t2, 100
	sub $t3, $s0, $t2
	bgez $t3, __label_2

__label_1:
	addi $s0, $sp, 8
	lw $s0, 0($s0)
	li $t4, 1
	add $t5, $s0, $t4
	addi $s1, $sp, 12
	sw $t5, 0($s1)
	addi $s0, $sp, 8
	lw $s0, 0($s0)
	li $t6, 1
	add $t7, $s0, $t6
	addi $s0, $sp, 8
	sw $t7, 0($s0)
	j __label_3

__label_2:
	addi $s1, $sp, 12
	lw $s1, 0($s1)
	move $v0, $s1
	j __label_4

__label_4:
	lw $s0, 0($sp)
	lw $s1, 4($sp)
	addi $sp, $sp, 48
	jr $ra

void_function:
	addi $sp, $sp, -36
	li $t0, 0
	li $t1, 0
	bne $t0, $t1, __label_9
	la $t2, __asciiz_5
	sw $a0, 36($sp)
	move $a0, $t2
	li $v0, 4
	syscall
	lw $a0, 36($sp)

__label_9:
	addi $a0, $sp, 36
	lb $a0, 0($a0)
	li $t3, 97
	bne $t3, $a0, __label_10
	la $t4, __asciiz_6
	sw $a0, 36($sp)
	move $a0, $t4
	li $v0, 4
	syscall
	lw $a0, 36($sp)
	addi $a1, $sp, 40
	lw $a1, 0($a1)
	sw $a0, 36($sp)
	move $a0, $a1
	li $v0, 1
	syscall
	lw $a0, 36($sp)
	j __label_11

__label_10:
	li $t5, 98
	bne $t5, $a0, __label_12
	la $t6, __asciiz_7
	sw $a0, 36($sp)
	move $a0, $t6
	li $v0, 4
	syscall
	lw $a0, 36($sp)
	addi $a2, $sp, 44
	lb $a2, 0($a2)
	sw $a0, 36($sp)
	move $a0, $a2
	li $v0, 11
	syscall
	lw $a0, 36($sp)
	j __label_11

__label_12:
	la $t7, __asciiz_8
	sw $a0, 36($sp)
	move $a0, $t7
	li $v0, 4
	syscall
	lw $a0, 36($sp)

__label_11:
	addi $sp, $sp, 36
	jr $ra

void_function_noargs:
	j __label_13

__label_13:
	jr $ra

void_function_noargs_2:
	addi $sp, $sp, -88
	sw $s0, 12($sp)
	sw $s1, 16($sp)
	sw $ra, 20($sp)
	addi $s0, $sp, 24
	li $v0, 5
	syscall
	sw $v0, 0($s0)
	addi $s0, $sp, 24
	lw $s0, 0($s0)
	li $t0, 1
	bne $s0, $t0, __label_16
	li $t1, 98
	addi $s0, $sp, 24
	lw $s0, 0($s0)
	li $t2, 97
	move $a0, $t2
	move $a1, $s0
	move $a2, $t1
	jal void_function
	j __label_17

__label_16:
	addi $s0, $sp, 24
	lw $s0, 0($s0)
	li $t3, 2
	bne $s0, $t3, __label_18
	jal int_function_noargs
	move $t4, $v0
	addi $s1, $sp, 28
	sw $t4, 0($s1)
	la $t5, __asciiz_14
	move $a0, $t5
	li $v0, 4
	syscall
	addi $s1, $sp, 28
	lw $s1, 0($s1)
	move $a0, $s1
	li $v0, 1
	syscall
	j __label_17

__label_18:
	li $t6, 2
	addi $s1, $sp, 28
	sw $t6, 0($s1)
	j __label_19

__label_20:
	addi $s1, $sp, 28
	lw $s1, 0($s1)
	li $t7, 3
	sub $t8, $s1, $t7
	bgez $t8, __label_17

__label_19:
	la $t9, __asciiz_15
	move $a0, $t9
	li $v0, 4
	syscall
	addi $s1, $sp, 28
	lw $s1, 0($s1)
	move $a0, $s1
	li $v0, 1
	syscall
	addi $s1, $sp, 28
	lw $s1, 0($s1)
	li $s0, 1
	sw $t0, 32($sp)
	add $t0, $s1, $s0
	addi $s1, $sp, 28
	sw $t0, 0($s1)
	j __label_20

__label_17:
	lw $s0, 12($sp)
	lw $s1, 16($sp)
	lw $ra, 20($sp)
	addi $sp, $sp, 88
	jr $ra

recursive_function_1:
	addi $sp, $sp, -36
	sw $ra, 4($sp)
	addi $a0, $sp, 36
	lw $a0, 0($a0)
	li $t0, 2
	sub $t1, $a0, $t0
	bgez $t1, __label_21
	li $t2, 1
	move $v0, $t2
	j __label_22

__label_21:
	addi $a0, $sp, 36
	lw $a0, 0($a0)
	addi $a0, $sp, 36
	lw $a0, 0($a0)
	li $t3, 1
	sub $t4, $a0, $t3
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 36($sp)
	move $a0, $t4
	jal recursive_function_1
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	move $t5, $v0
	lw $a0, 36($sp)
	mult $a0, $t5
	mflo $t6
	move $v0, $t6
	j __label_22

__label_22:
	lw $ra, 4($sp)
	addi $sp, $sp, 36
	jr $ra

recursive_function_2:
	addi $sp, $sp, -68
	sw $ra, 16($sp)
	addi $a3, $sp, 80
	lw $a3, 0($a3)
	li $t0, 1
	bne $a3, $t0, __label_29
	la $t1, __asciiz_23
	sw $a0, 68($sp)
	move $a0, $t1
	li $v0, 4
	syscall
	lw $a0, 68($sp)
	addi $a3, $sp, 80
	lw $a3, 0($a3)
	sw $a0, 68($sp)
	move $a0, $a3
	li $v0, 1
	syscall
	lw $a0, 68($sp)
	la $t2, __asciiz_24
	sw $a0, 68($sp)
	move $a0, $t2
	li $v0, 4
	syscall
	lw $a0, 68($sp)
	addi $a0, $sp, 68
	lb $a0, 0($a0)
	sw $a0, 68($sp)
	move $a0, $a0
	li $v0, 11
	syscall
	lw $a0, 68($sp)
	la $t3, __asciiz_25
	sw $a0, 68($sp)
	move $a0, $t3
	li $v0, 4
	syscall
	lw $a0, 68($sp)
	addi $a2, $sp, 76
	lb $a2, 0($a2)
	sw $a0, 68($sp)
	move $a0, $a2
	li $v0, 11
	syscall
	lw $a0, 68($sp)
	j __label_30

__label_29:
	addi $a3, $sp, 80
	lw $a3, 0($a3)
	li $t4, 1
	sub $t5, $a3, $t4
	addi $a1, $sp, 72
	lb $a1, 0($a1)
	addi $a2, $sp, 76
	lb $a2, 0($a2)
	addi $a0, $sp, 68
	lb $a0, 0($a0)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 68($sp)
	sw $a1, 72($sp)
	sw $a2, 76($sp)
	sw $a3, 80($sp)
	move $a0, $a0
	move $a1, $a2
	move $a2, $a1
	move $a3, $t5
	jal recursive_function_2
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 68($sp)
	lw $a1, 72($sp)
	lw $a2, 76($sp)
	lw $a3, 80($sp)
	la $t6, __asciiz_26
	sw $a0, 68($sp)
	move $a0, $t6
	li $v0, 4
	syscall
	lw $a0, 68($sp)
	addi $a3, $sp, 80
	lw $a3, 0($a3)
	sw $a0, 68($sp)
	move $a0, $a3
	li $v0, 1
	syscall
	lw $a0, 68($sp)
	la $t7, __asciiz_27
	sw $a0, 68($sp)
	move $a0, $t7
	li $v0, 4
	syscall
	lw $a0, 68($sp)
	addi $a0, $sp, 68
	lb $a0, 0($a0)
	sw $a0, 68($sp)
	move $a0, $a0
	li $v0, 11
	syscall
	lw $a0, 68($sp)
	la $t8, __asciiz_28
	sw $a0, 68($sp)
	move $a0, $t8
	li $v0, 4
	syscall
	lw $a0, 68($sp)
	addi $a2, $sp, 76
	lb $a2, 0($a2)
	sw $a0, 68($sp)
	move $a0, $a2
	li $v0, 11
	syscall
	lw $a0, 68($sp)
	addi $a3, $sp, 80
	lw $a3, 0($a3)
	li $t9, 1
	sub $a3, $a3, $t9
	addi $a2, $sp, 76
	lb $a2, 0($a2)
	addi $a0, $sp, 68
	lb $a0, 0($a0)
	addi $a1, $sp, 72
	lb $a1, 0($a1)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 68($sp)
	sw $a1, 72($sp)
	sw $a2, 76($sp)
	sw $a3, 80($sp)
	move $a0, $a1
	move $a1, $a0
	move $a2, $a2
	move $a3, $a3
	jal recursive_function_2
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 68($sp)
	lw $a1, 72($sp)
	lw $a2, 76($sp)
	lw $a3, 80($sp)

__label_30:
	lw $ra, 16($sp)
	addi $sp, $sp, 68
	jr $ra

recursive_function_3:
	addi $sp, $sp, -188
	sw $s0, 4($sp)
	sw $s1, 8($sp)
	sw $s2, 12($sp)
	sw $ra, 16($sp)
	addi $a0, $sp, 188
	lw $a0, 0($a0)
	li $t0, 0
	bne $a0, $t0, __label_32
	la $t1, __asciiz_31
	sw $a0, 188($sp)
	move $a0, $t1
	li $v0, 4
	syscall
	lw $a0, 188($sp)
	j __label_33

__label_32:
	addi $a0, $sp, 188
	lw $a0, 0($a0)
	li $t2, 1
	bne $a0, $t2, __label_34
	li $t3, 0
	addi $s0, $sp, 20
	sw $t3, 0($s0)
	j __label_35

__label_37:
	addi $s0, $sp, 20
	lw $s0, 0($s0)
	li $t4, 4
	sub $t5, $s0, $t4
	bgez $t5, __label_36

__label_35:
	addi $s0, $sp, 20
	lw $s0, 0($s0)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 188($sp)
	move $a0, $s0
	jal recursive_function_3
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 188($sp)
	addi $s0, $sp, 20
	lw $s0, 0($s0)
	li $t6, 1
	add $t7, $s0, $t6
	addi $s0, $sp, 20
	sw $t7, 0($s0)
	j __label_37

__label_36:
	j __label_33

__label_34:
	addi $a0, $sp, 188
	lw $a0, 0($a0)
	li $t8, 2
	bne $a0, $t8, __label_38
	li $t9, 0
	addi $s0, $sp, 20
	sw $t9, 0($s0)
	j __label_39

__label_44:
	addi $s0, $sp, 20
	lw $s0, 0($s0)
	li $a0, 4
	sw $t0, 32($sp)
	sub $t0, $s0, $a0
	bgez $t0, __label_40

__label_39:
	sw $zero, 36($sp)
	li $zero, 0
	addi $s1, $sp, 24
	sw $zero, 0($s1)
	j __label_41

__label_43:
	addi $s1, $sp, 24
	lw $s1, 0($s1)
	sw $t1, 40($sp)
	li $t1, 4
	sub $a0, $s1, $t1
	bgez $a0, __label_42

__label_41:
	addi $s0, $sp, 20
	lw $s0, 0($s0)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 84($sp)
	sw $a0, 100($sp)
	sw $a0, 0($sp)
	sw $a0, 188($sp)
	move $a0, $s0
	jal recursive_function_3
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 84($sp)
	lw $a0, 100($sp)
	lw $a0, 0($sp)
	lw $a0, 188($sp)
	addi $s1, $sp, 24
	lw $s1, 0($s1)
	sw $t2, 44($sp)
	li $t2, 1
	sw $zero, 48($sp)
	add $zero, $s1, $t2
	addi $s1, $sp, 24
	sw $zero, 0($s1)
	j __label_43

__label_42:
	addi $s0, $sp, 20
	lw $s0, 0($s0)
	sw $t3, 52($sp)
	li $t3, 1
	add $s0, $s0, $t3
	addi $s0, $sp, 20
	sw $s0, 0($s0)
	j __label_44

__label_40:
	j __label_33

__label_38:
	addi $a0, $sp, 188
	lw $a0, 0($a0)
	sw $t4, 56($sp)
	li $t4, 3
	bne $a0, $t4, __label_33
	sw $t5, 60($sp)
	li $t5, 0
	addi $s0, $sp, 20
	sw $t5, 0($s0)
	j __label_45

__label_53:
	addi $s0, $sp, 20
	lw $s0, 0($s0)
	li $s0, 4
	sw $t6, 64($sp)
	sub $t6, $s0, $s0
	bgez $t6, __label_46

__label_45:
	sw $t7, 68($sp)
	li $t7, 0
	addi $s1, $sp, 24
	sw $t7, 0($s1)
	j __label_47

__label_52:
	addi $s1, $sp, 24
	lw $s1, 0($s1)
	li $a0, 4
	sw $t8, 72($sp)
	sub $t8, $s1, $a0
	bgez $t8, __label_48

__label_47:
	sw $zero, 76($sp)
	li $zero, 0
	addi $s2, $sp, 28
	sw $zero, 0($s2)
	j __label_49

__label_51:
	addi $s2, $sp, 28
	lw $s2, 0($s2)
	sw $t9, 80($sp)
	li $t9, 4
	sub $s0, $s2, $t9
	bgez $s0, __label_50

__label_49:
	addi $s0, $sp, 20
	lw $s0, 0($s0)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 0($sp)
	sw $a0, 84($sp)
	sw $a0, 100($sp)
	sw $a0, 0($sp)
	sw $a0, 144($sp)
	sw $a0, 188($sp)
	move $a0, $s0
	jal recursive_function_3
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 0($sp)
	lw $a0, 84($sp)
	lw $a0, 100($sp)
	lw $a0, 0($sp)
	lw $a0, 144($sp)
	lw $a0, 188($sp)
	addi $s2, $sp, 28
	lw $s2, 0($s2)
	sw $a0, 84($sp)
	li $a0, 1
	sw $t0, 88($sp)
	add $t0, $s2, $a0
	addi $s2, $sp, 28
	sw $t0, 0($s2)
	j __label_51

__label_50:
	addi $s1, $sp, 24
	lw $s1, 0($s1)
	sw $zero, 92($sp)
	li $zero, 1
	add $s1, $s1, $zero
	addi $s1, $sp, 24
	sw $s1, 0($s1)
	j __label_52

__label_48:
	addi $s0, $sp, 20
	lw $s0, 0($s0)
	sw $t1, 96($sp)
	li $t1, 1
	sw $a0, 100($sp)
	add $a0, $s0, $t1
	addi $s0, $sp, 20
	sw $a0, 0($s0)
	j __label_53

__label_46:
	j __label_33

__label_33:
	lw $s0, 4($sp)
	lw $s1, 8($sp)
	lw $s2, 12($sp)
	lw $ra, 16($sp)
	addi $sp, $sp, 188
	jr $ra

print_usage:
	addi $sp, $sp, -56
	la $t0, __asciiz_54
	move $a0, $t0
	li $v0, 4
	syscall
	la $t1, __asciiz_55
	move $a0, $t1
	li $v0, 4
	syscall
	la $t2, __asciiz_56
	move $a0, $t2
	li $v0, 4
	syscall
	la $t3, __asciiz_57
	move $a0, $t3
	li $v0, 4
	syscall
	la $t4, __asciiz_58
	move $a0, $t4
	li $v0, 4
	syscall
	la $t5, __asciiz_59
	move $a0, $t5
	li $v0, 4
	syscall
	la $t6, __asciiz_60
	move $a0, $t6
	li $v0, 4
	syscall
	la $t7, __asciiz_61
	move $a0, $t7
	li $v0, 4
	syscall
	la $t8, __asciiz_62
	move $a0, $t8
	li $v0, 4
	syscall
	la $t9, __asciiz_63
	move $a0, $t9
	li $v0, 4
	syscall
	sw $t0, 0($sp)
	la $t0, __asciiz_64
	move $a0, $t0
	li $v0, 4
	syscall
	sw $t1, 4($sp)
	la $t1, __asciiz_65
	move $a0, $t1
	li $v0, 4
	syscall
	sw $t2, 8($sp)
	la $t2, __asciiz_66
	move $a0, $t2
	li $v0, 4
	syscall
	sw $t3, 12($sp)
	la $t3, __asciiz_67
	move $a0, $t3
	li $v0, 4
	syscall
	addi $sp, $sp, 56
	jr $ra

int_array_do_sth:
	addi $sp, $sp, -96
	sw $s0, 0($sp)
	la $t0, __asciiz_68
	move $a0, $t0
	li $v0, 4
	syscall
	li $t1, 0
	la $t2, int_1
	sw $t1, 0($t2)
	j __label_69

__label_71:
	la $t3, int_1
	lw $t3, 0($t3)
	li $t4, 1
	sub $t5, $t3, $t4
	bgez $t5, __label_70

__label_69:
	la $t6, int_1
	lw $t6, 0($t6)
	la $t7, int_array_1
	li $t8, 4
	mult $t6, $t8
	mflo $t8
	add $t8, $t8, $t7
	lw $t8, 0($t8)
	addi $s0, $sp, 4
	sw $t8, 0($s0)
	la $t9, int_1
	lw $t9, 0($t9)
	sw $t0, 8($sp)
	la $t0, int_array_2
	sw $t1, 12($sp)
	li $t1, 4
	mult $t9, $t1
	mflo $t1
	add $t1, $t1, $t0
	lw $t1, 0($t1)
	sw $t2, 16($sp)
	la $t2, int_1
	lw $t2, 0($t2)
	sw $t3, 20($sp)
	la $t3, int_array_1
	sw $t4, 24($sp)
	li $t4, 4
	mult $t2, $t4
	mflo $t4
	add $t4, $t4, $t3
	sw $t1, 0($t4)
	addi $s0, $sp, 4
	lw $s0, 0($s0)
	sw $t5, 28($sp)
	la $t5, int_1
	lw $t5, 0($t5)
	sw $t6, 32($sp)
	la $t6, int_array_2
	sw $t7, 36($sp)
	li $t7, 4
	mult $t5, $t7
	mflo $t7
	add $t7, $t7, $t6
	sw $s0, 0($t7)
	sw $t8, 40($sp)
	la $t8, int_1
	lw $t8, 0($t8)
	sw $t9, 44($sp)
	li $t9, 1
	sw $t0, 48($sp)
	add $t0, $t8, $t9
	sw $t1, 52($sp)
	la $t1, int_1
	sw $t0, 0($t1)
	j __label_71

__label_70:
	lw $s0, 0($sp)
	addi $sp, $sp, 96
	jr $ra

swap_sth:
	addi $sp, $sp, -12
	sw $s0, 0($sp)
	la $t0, __asciiz_72
	move $a0, $t0
	li $v0, 4
	syscall
	lw $s0, 0($sp)
	addi $sp, $sp, 12
	jr $ra

main:
	addi $sp, $sp, -260
	sw $s0, 16($sp)
	sw $ra, 20($sp)
	la $t0, __asciiz_73
	move $a0, $t0
	li $v0, 4
	syscall
	li $t1, 1
	move $a0, $t1
	li $v0, 1
	syscall
	la $t2, __asciiz_74
	move $a0, $t2
	li $v0, 4
	syscall
	li $t3, 1
	move $a0, $t3
	li $v0, 1
	syscall
	la $t4, __asciiz_75
	move $a0, $t4
	li $v0, 4
	syscall
	li $t5, 43
	move $a0, $t5
	li $v0, 11
	syscall
	la $t6, __asciiz_76
	move $a0, $t6
	li $v0, 4
	syscall
	li $t7, 49
	move $a0, $t7
	li $v0, 11
	syscall
	jal print_usage
	li $t8, 0
	la $t9, int_1
	sw $t8, 0($t9)
	j __label_88

__label_103:
	sw $t0, 28($sp)
	la $t0, int_1
	lw $t0, 0($t0)
	sw $t1, 32($sp)
	li $t1, 1
	sw $t2, 36($sp)
	sub $t2, $t0, $t1
	bgez $t2, __label_89

__label_88:
	sw $t3, 40($sp)
	la $t3, __asciiz_77
	move $a0, $t3
	li $v0, 4
	syscall
	addi $s0, $sp, 24
	li $v0, 5
	syscall
	sw $v0, 0($s0)
	addi $s0, $sp, 24
	lw $s0, 0($s0)
	sw $t4, 44($sp)
	li $t4, 0
	bne $t4, $s0, __label_90
	sw $t5, 48($sp)
	la $t5, __asciiz_78
	move $a0, $t5
	li $v0, 4
	syscall
	sw $t6, 52($sp)
	la $t6, int_1
	li $v0, 5
	syscall
	sw $v0, 0($t6)
	sw $t7, 56($sp)
	la $t7, __asciiz_79
	move $a0, $t7
	li $v0, 4
	syscall
	sw $t8, 60($sp)
	la $t8, int_1
	lw $t8, 0($t8)
	move $a0, $t8
	li $v0, 1
	syscall
	j __label_91

__label_90:
	sw $t9, 64($sp)
	li $t9, 1
	bne $t9, $s0, __label_92
	sw $t0, 68($sp)
	la $t0, __asciiz_80
	move $a0, $t0
	li $v0, 4
	syscall
	jal int_function_noargs
	sw $t1, 72($sp)
	move $t1, $v0
	move $a0, $t1
	li $v0, 1
	syscall
	j __label_91

__label_92:
	sw $t2, 76($sp)
	li $t2, 2
	bne $t2, $s0, __label_93
	sw $t3, 80($sp)
	la $t3, __asciiz_81
	move $a0, $t3
	li $v0, 4
	syscall
	sw $t4, 84($sp)
	la $t4, char_1
	li $v0, 12
	syscall
	sb $v0, 0($t4)
	sw $t5, 88($sp)
	li $t5, 43
	sw $t6, 92($sp)
	li $t6, 0
	sw $t7, 96($sp)
	la $t7, char_1
	lb $t7, 0($t7)
	move $a0, $t7
	move $a1, $t6
	move $a2, $t5
	jal void_function
	j __label_91

__label_93:
	sw $t8, 100($sp)
	li $t8, 3
	bne $t8, $s0, __label_94
	sw $t9, 104($sp)
	la $t9, __asciiz_82
	move $a0, $t9
	li $v0, 4
	syscall
	jal void_function_noargs
	j __label_91

__label_94:
	sw $t0, 108($sp)
	li $t0, 4
	bne $t0, $s0, __label_95
	sw $t1, 112($sp)
	la $t1, __asciiz_83
	move $a0, $t1
	li $v0, 4
	syscall
	sw $t2, 116($sp)
	la $t2, int_1
	li $v0, 5
	syscall
	sw $v0, 0($t2)
	jal void_function_noargs_2
	j __label_91

__label_95:
	sw $t3, 120($sp)
	li $t3, 5
	bne $t3, $s0, __label_96
	sw $t4, 124($sp)
	la $t4, __asciiz_84
	move $a0, $t4
	li $v0, 4
	syscall
	sw $t5, 128($sp)
	la $t5, int_1
	li $v0, 5
	syscall
	sw $v0, 0($t5)
	sw $t6, 132($sp)
	la $t6, int_1
	lw $t6, 0($t6)
	move $a0, $t6
	jal recursive_function_1
	sw $t7, 136($sp)
	move $t7, $v0
	j __label_91

__label_96:
	sw $t8, 140($sp)
	li $t8, 6
	bne $t8, $s0, __label_97
	sw $t9, 144($sp)
	la $t9, __asciiz_85
	move $a0, $t9
	li $v0, 4
	syscall
	sw $t0, 148($sp)
	la $t0, int_1
	li $v0, 5
	syscall
	sw $v0, 0($t0)
	sw $t1, 152($sp)
	la $t1, int_1
	lw $t1, 0($t1)
	move $a0, $t1
	jal recursive_function_3
	j __label_91

__label_97:
	sw $t2, 156($sp)
	li $t2, 7
	bne $t2, $s0, __label_98
	sw $t3, 160($sp)
	la $t3, __asciiz_86
	move $a0, $t3
	li $v0, 4
	syscall
	sw $t4, 164($sp)
	la $t4, int_1
	li $v0, 5
	syscall
	sw $v0, 0($t4)
	sw $t5, 168($sp)
	la $t5, int_1
	lw $t5, 0($t5)
	sw $t6, 172($sp)
	li $t6, 42
	sw $t7, 176($sp)
	li $t7, 47
	sw $t8, 180($sp)
	li $t8, 43
	move $a0, $t8
	move $a1, $t7
	move $a2, $t6
	move $a3, $t5
	jal recursive_function_2
	j __label_91

__label_98:
	sw $t9, 184($sp)
	li $t9, 8
	bne $t9, $s0, __label_99
	jal int_array_do_sth
	j __label_91

__label_99:
	sw $t0, 188($sp)
	li $t0, 9
	bne $t0, $s0, __label_100
	jal swap_sth
	j __label_91

__label_100:
	sw $t1, 192($sp)
	li $t1, 113
	bne $t1, $s0, __label_101
	j __label_89
	j __label_91

__label_101:
	sw $t2, 196($sp)
	li $t2, 104
	bne $t2, $s0, __label_102
	jal print_usage
	j __label_91

__label_102:
	sw $t3, 200($sp)
	la $t3, __asciiz_87
	move $a0, $t3
	li $v0, 4
	syscall

__label_91:
	sw $t4, 204($sp)
	la $t4, int_1
	lw $t4, 0($t4)
	sw $t5, 208($sp)
	li $t5, 1
	sw $t6, 212($sp)
	sub $t6, $t4, $t5
	sw $t7, 216($sp)
	la $t7, int_1
	sw $t6, 0($t7)
	j __label_103

__label_89:
	lw $s0, 16($sp)
	lw $ra, 20($sp)
	addi $sp, $sp, 260
	jr $ra
