.data
__asciiz_5: .asciiz "result = "
__asciiz_6: .asciiz "expect = "
__asciiz_7: .asciiz "Error"
.text
.globl main
	jal main
	li $v0, 10
	syscall

fibo:
	addi $sp, $sp, -84
	sw $ra, 4($sp)
	sw $a0, 84($sp)
	addi $a0, $sp, 84
	lw $a0, 0($a0)
	li $t0, 0
	sub $t1, $a0, $t0
	bgez $t1, __label_0
	li $t2, 1
	move $v0, $t2
	j __label_1
	j __label_1

__label_0:
	addi $a0, $sp, 84
	lw $a0, 0($a0)
	li $t3, 0
	bne $a0, $t3, __label_2
	li $t4, 0
	move $v0, $t4
	j __label_1
	j __label_1

__label_2:
	addi $a0, $sp, 84
	lw $a0, 0($a0)
	li $t5, 1
	bne $a0, $t5, __label_3
	li $t6, 1
	move $v0, $t6
	j __label_1
	j __label_1

__label_3:
	addi $a0, $sp, 84
	lw $a0, 0($a0)
	li $t7, 1000
	sub $t8, $a0, $t7
	blez $t8, __label_4
	li $t9, 2
	move $v0, $t9
	j __label_1
	j __label_1

__label_4:
	addi $a0, $sp, 84
	lw $a0, 0($a0)
	sw $t0, 8($sp)
	li $t0, 1
	sw $t1, 12($sp)
	sub $t1, $a0, $t0
	sw $t2, 16($sp)
	sw $t3, 20($sp)
	sw $t4, 28($sp)
	sw $t5, 32($sp)
	sw $t6, 40($sp)
	sw $t7, 44($sp)
	sw $t8, 48($sp)
	sw $t9, 52($sp)
	sw $t0, 56($sp)
	sw $t1, 60($sp)
	sw $a0, 84($sp)
	move $a0, $t1
	jal fibo
	lw $t2, 16($sp)
	lw $t3, 20($sp)
	lw $t4, 28($sp)
	lw $t5, 32($sp)
	lw $t6, 40($sp)
	lw $t7, 44($sp)
	lw $t8, 48($sp)
	lw $t9, 52($sp)
	lw $t0, 56($sp)
	lw $t1, 60($sp)
	sw $t2, 16($sp)
	move $t2, $v0
	lw $a0, 84($sp)
	addi $a0, $sp, 84
	lw $a0, 0($a0)
	sw $t3, 20($sp)
	li $t3, 2
	sw $t4, 28($sp)
	sub $t4, $a0, $t3
	sw $t5, 32($sp)
	sw $t6, 40($sp)
	sw $t7, 44($sp)
	sw $t8, 48($sp)
	sw $t9, 52($sp)
	sw $t0, 56($sp)
	sw $t1, 60($sp)
	sw $t2, 64($sp)
	sw $t3, 68($sp)
	sw $t4, 72($sp)
	sw $a0, 84($sp)
	move $a0, $t4
	jal fibo
	lw $t5, 32($sp)
	lw $t6, 40($sp)
	lw $t7, 44($sp)
	lw $t8, 48($sp)
	lw $t9, 52($sp)
	lw $t0, 56($sp)
	lw $t1, 60($sp)
	lw $t2, 64($sp)
	lw $t3, 68($sp)
	lw $t4, 72($sp)
	sw $t5, 32($sp)
	move $t5, $v0
	lw $a0, 84($sp)
	sw $t6, 40($sp)
	add $t6, $t2, $t5
	move $v0, $t6
	j __label_1

__label_1:
	lw $ra, 4($sp)
	addi $sp, $sp, 84
	jr $ra

test_fibo:
	addi $sp, $sp, -36
	sw $s0, 4($sp)
	sw $ra, 8($sp)
	sw $a0, 36($sp)
	sw $a1, 40($sp)
	addi $a0, $sp, 36
	lw $a0, 0($a0)
	sw $a0, 36($sp)
	sw $a1, 40($sp)
	move $a0, $a0
	jal fibo
	move $t0, $v0
	lw $a0, 36($sp)
	lw $a1, 40($sp)
	addi $s0, $sp, 12
	sw $t0, 0($s0)
	addi $s0, $sp, 12
	lw $s0, 0($s0)
	addi $a1, $sp, 40
	lw $a1, 0($a1)
	beq $s0, $a1, __label_8
	la $t1, __asciiz_5
	sw $a0, 36($sp)
	move $a0, $t1
	li $v0, 4
	syscall
	lw $a0, 36($sp)
	addi $s0, $sp, 12
	lw $s0, 0($s0)
	sw $a0, 36($sp)
	move $a0, $s0
	li $v0, 1
	syscall
	lw $a0, 36($sp)
	la $t2, __asciiz_6
	sw $a0, 36($sp)
	move $a0, $t2
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
	la $t3, __asciiz_7
	sw $a0, 36($sp)
	move $a0, $t3
	li $v0, 4
	syscall
	lw $a0, 36($sp)

__label_8:
	lw $s0, 4($sp)
	lw $ra, 8($sp)
	addi $sp, $sp, 36
	jr $ra

main:
	addi $sp, $sp, -80
	sw $ra, 4($sp)
	li $t0, 0
	sw $t0, 8($sp)
	move $a0, $t0
	jal fibo
	lw $t0, 8($sp)
	move $t1, $v0
	move $a0, $t1
	li $v0, 1
	syscall
	li $t2, 1
	sw $t0, 8($sp)
	sw $t1, 12($sp)
	sw $t2, 16($sp)
	move $a0, $t2
	jal fibo
	lw $t0, 8($sp)
	lw $t1, 12($sp)
	lw $t2, 16($sp)
	move $t3, $v0
	move $a0, $t3
	li $v0, 1
	syscall
	li $t4, 2
	sw $t0, 8($sp)
	sw $t1, 12($sp)
	sw $t2, 16($sp)
	sw $t3, 20($sp)
	sw $t4, 24($sp)
	move $a0, $t4
	jal fibo
	lw $t0, 8($sp)
	lw $t1, 12($sp)
	lw $t2, 16($sp)
	lw $t3, 20($sp)
	lw $t4, 24($sp)
	move $t5, $v0
	move $a0, $t5
	li $v0, 1
	syscall
	li $t6, 3
	sw $t0, 8($sp)
	sw $t1, 12($sp)
	sw $t2, 16($sp)
	sw $t3, 20($sp)
	sw $t4, 24($sp)
	sw $t5, 28($sp)
	sw $t6, 32($sp)
	move $a0, $t6
	jal fibo
	lw $t0, 8($sp)
	lw $t1, 12($sp)
	lw $t2, 16($sp)
	lw $t3, 20($sp)
	lw $t4, 24($sp)
	lw $t5, 28($sp)
	lw $t6, 32($sp)
	move $t7, $v0
	move $a0, $t7
	li $v0, 1
	syscall
	li $t8, 4
	sw $t0, 8($sp)
	sw $t1, 12($sp)
	sw $t2, 16($sp)
	sw $t3, 20($sp)
	sw $t4, 24($sp)
	sw $t5, 28($sp)
	sw $t6, 32($sp)
	sw $t7, 36($sp)
	sw $t8, 40($sp)
	move $a0, $t8
	jal fibo
	lw $t0, 8($sp)
	lw $t1, 12($sp)
	lw $t2, 16($sp)
	lw $t3, 20($sp)
	lw $t4, 24($sp)
	lw $t5, 28($sp)
	lw $t6, 32($sp)
	lw $t7, 36($sp)
	lw $t8, 40($sp)
	move $t9, $v0
	move $a0, $t9
	li $v0, 1
	syscall
	sw $t1, 12($sp)
	li $t1, 5
	sw $t0, 8($sp)
	sw $t2, 16($sp)
	sw $t3, 20($sp)
	sw $t4, 24($sp)
	sw $t5, 28($sp)
	sw $t6, 32($sp)
	sw $t7, 36($sp)
	sw $t8, 40($sp)
	sw $t9, 44($sp)
	sw $t1, 48($sp)
	move $a0, $t1
	jal fibo
	lw $t0, 8($sp)
	lw $t2, 16($sp)
	lw $t3, 20($sp)
	lw $t4, 24($sp)
	lw $t5, 28($sp)
	lw $t6, 32($sp)
	lw $t7, 36($sp)
	lw $t8, 40($sp)
	lw $t9, 44($sp)
	lw $t1, 48($sp)
	sw $t2, 16($sp)
	move $t2, $v0
	move $a0, $t2
	li $v0, 1
	syscall
	sw $t3, 20($sp)
	li $t3, 6
	sw $t0, 8($sp)
	sw $t4, 24($sp)
	sw $t5, 28($sp)
	sw $t6, 32($sp)
	sw $t7, 36($sp)
	sw $t8, 40($sp)
	sw $t9, 44($sp)
	sw $t1, 48($sp)
	sw $t2, 52($sp)
	sw $t3, 56($sp)
	move $a0, $t3
	jal fibo
	lw $t0, 8($sp)
	lw $t4, 24($sp)
	lw $t5, 28($sp)
	lw $t6, 32($sp)
	lw $t7, 36($sp)
	lw $t8, 40($sp)
	lw $t9, 44($sp)
	lw $t1, 48($sp)
	lw $t2, 52($sp)
	lw $t3, 56($sp)
	sw $t4, 24($sp)
	move $t4, $v0
	move $a0, $t4
	li $v0, 1
	syscall
	sw $t5, 28($sp)
	li $t5, 7
	sw $t0, 8($sp)
	sw $t6, 32($sp)
	sw $t7, 36($sp)
	sw $t8, 40($sp)
	sw $t9, 44($sp)
	sw $t1, 48($sp)
	sw $t2, 52($sp)
	sw $t3, 56($sp)
	sw $t4, 60($sp)
	sw $t5, 64($sp)
	move $a0, $t5
	jal fibo
	lw $t0, 8($sp)
	lw $t6, 32($sp)
	lw $t7, 36($sp)
	lw $t8, 40($sp)
	lw $t9, 44($sp)
	lw $t1, 48($sp)
	lw $t2, 52($sp)
	lw $t3, 56($sp)
	lw $t4, 60($sp)
	lw $t5, 64($sp)
	sw $t6, 32($sp)
	move $t6, $v0
	move $a0, $t6
	li $v0, 1
	syscall
	sw $t7, 36($sp)
	li $t7, 8
	sw $t0, 8($sp)
	sw $t8, 40($sp)
	sw $t9, 44($sp)
	sw $t1, 48($sp)
	sw $t2, 52($sp)
	sw $t3, 56($sp)
	sw $t4, 60($sp)
	sw $t5, 64($sp)
	sw $t6, 68($sp)
	sw $t7, 72($sp)
	move $a0, $t7
	jal fibo
	lw $t0, 8($sp)
	lw $t8, 40($sp)
	lw $t9, 44($sp)
	lw $t1, 48($sp)
	lw $t2, 52($sp)
	lw $t3, 56($sp)
	lw $t4, 60($sp)
	lw $t5, 64($sp)
	lw $t6, 68($sp)
	lw $t7, 72($sp)
	sw $t8, 40($sp)
	move $t8, $v0
	move $a0, $t8
	li $v0, 1
	syscall
	lw $ra, 4($sp)
	addi $sp, $sp, 80
	jr $ra
