.data
__asciiz_0: .asciiz " z = "
__asciiz_5: .asciiz " "
.text
.globl main

	jal main
	li $v0, 10
	syscall

mod:
	addi $sp, $sp, -52
	sw $s0, 0($sp)
	sw $a0, 52($sp)
	sw $a1, 56($sp)
	addi $t0, $sp, 52
	lw $t0, 0($t0)
	addi $t1, $sp, 52
	lw $t1, 0($t1)
	addi $t2, $sp, 56
	lw $t2, 0($t2)
	div $t1, $t2
	mflo $t3
	addi $t4, $sp, 56
	lw $t4, 0($t4)
	mult $t3, $t4
	mflo $t5
	sub $t6, $t0, $t5
	addi $t7, $sp, 48
	sw $t6, 0($t7)
	la $t8, __asciiz_0
	sw $a0, 52($sp)
	move $a0, $t8
	li $v0, 4
	syscall
	lw $a0, 52($sp)
	addi $t9, $sp, 48
	lw $t9, 0($t9)
	sw $a0, 52($sp)
	move $a0, $t9
	li $v0, 1
	syscall
	lw $a0, 52($sp)
	sw $t0, 4($sp)
	addi $t0, $sp, 48
	lw $t0, 0($t0)
	move $v0, $t0
	j __label_1
__label_1:
	lw $s0, 0($sp)
	addi $sp, $sp, 52
	jr $ra

gcd:
	addi $sp, $sp, -72
	sw $ra, 8($sp)
	sw $a0, 72($sp)
	sw $a1, 76($sp)
	addi $t0, $sp, 76
	lw $t0, 0($t0)
	li $t1, 0
	bne $t0, $t1, __label_2
	li $t2, 0
	move $v0, $t2
	j __label_3
	j __label_3
__label_2:
	addi $t3, $sp, 76
	lw $t3, 0($t3)
	addi $t4, $sp, 72
	lw $t4, 0($t4)
	sw $t0, 12($sp)
	sw $t1, 16($sp)
	sw $t2, 24($sp)
	sw $t3, 28($sp)
	sw $t4, 32($sp)
	sw $a0, 72($sp)
	move $a0, $t4
	sw $a1, 76($sp)
	move $a1, $t3
	jal mod
	lw $t0, 12($sp)
	lw $t1, 16($sp)
	lw $t2, 24($sp)
	lw $t3, 28($sp)
	lw $t4, 32($sp)
	move $t5, $v0
	lw $a1, 76($sp)
	lw $a0, 72($sp)
	li $t6, 0
	bne $t5, $t6, __label_4
	addi $t7, $sp, 76
	lw $t7, 0($t7)
	move $v0, $t7
	j __label_3
	j __label_3
__label_4:
	addi $t8, $sp, 76
	lw $t8, 0($t8)
	addi $t9, $sp, 72
	lw $t9, 0($t9)
	sw $t0, 12($sp)
	sw $t1, 16($sp)
	sw $t2, 24($sp)
	sw $t3, 28($sp)
	sw $t4, 32($sp)
	sw $t5, 36($sp)
	sw $t6, 40($sp)
	sw $t7, 48($sp)
	sw $t8, 52($sp)
	sw $t9, 56($sp)
	sw $a0, 72($sp)
	move $a0, $t9
	sw $a1, 76($sp)
	move $a1, $t8
	jal mod
	lw $t0, 12($sp)
	lw $t1, 16($sp)
	lw $t2, 24($sp)
	lw $t3, 28($sp)
	lw $t4, 32($sp)
	lw $t5, 36($sp)
	lw $t6, 40($sp)
	lw $t7, 48($sp)
	lw $t8, 52($sp)
	lw $t9, 56($sp)
	sw $t0, 12($sp)
	move $t0, $v0
	lw $a1, 76($sp)
	lw $a0, 72($sp)
	sw $t1, 16($sp)
	addi $t1, $sp, 76
	lw $t1, 0($t1)
	sw $t2, 24($sp)
	sw $t3, 28($sp)
	sw $t4, 32($sp)
	sw $t5, 36($sp)
	sw $t6, 40($sp)
	sw $t7, 48($sp)
	sw $t8, 52($sp)
	sw $t9, 56($sp)
	sw $t0, 60($sp)
	sw $t1, 64($sp)
	sw $a0, 72($sp)
	move $a0, $t1
	sw $a1, 76($sp)
	move $a1, $t0
	jal gcd
	lw $t2, 24($sp)
	lw $t3, 28($sp)
	lw $t4, 32($sp)
	lw $t5, 36($sp)
	lw $t6, 40($sp)
	lw $t7, 48($sp)
	lw $t8, 52($sp)
	lw $t9, 56($sp)
	lw $t0, 60($sp)
	lw $t1, 64($sp)
	sw $t2, 24($sp)
	move $t2, $v0
	lw $a1, 76($sp)
	lw $a0, 72($sp)
	move $v0, $t2
	j __label_3
__label_3:
	lw $ra, 8($sp)
	addi $sp, $sp, 72
	jr $ra

main:
	addi $sp, $sp, -76
	sw $s0, 8($sp)
	sw $s1, 12($sp)
	sw $s2, 16($sp)
	sw $ra, 20($sp)
	li $t0, 15
	addi $t1, $sp, 64
	sw $t0, 0($t1)
	li $t2, 24
	addi $t3, $sp, 68
	sw $t2, 0($t3)
	addi $t4, $sp, 68
	lw $t4, 0($t4)
	addi $t5, $sp, 64
	lw $t5, 0($t5)
	sw $t0, 24($sp)
	sw $t1, 28($sp)
	sw $t2, 32($sp)
	sw $t3, 36($sp)
	sw $t4, 40($sp)
	sw $t5, 44($sp)
	sw $a0, 76($sp)
	move $a0, $t5
	sw $a1, 80($sp)
	move $a1, $t4
	jal gcd
	lw $t0, 24($sp)
	lw $t1, 28($sp)
	lw $t2, 32($sp)
	lw $t3, 36($sp)
	lw $t4, 40($sp)
	lw $t5, 44($sp)
	move $t6, $v0
	lw $a1, 80($sp)
	lw $a0, 76($sp)
	addi $t7, $sp, 72
	sw $t6, 0($t7)
	la $t8, __asciiz_5
	move $a0, $t8
	li $v0, 4
	syscall
	addi $t9, $sp, 72
	lw $t9, 0($t9)
	move $a0, $t9
	li $v0, 1
	syscall
	lw $s0, 8($sp)
	lw $s1, 12($sp)
	lw $s2, 16($sp)
	lw $ra, 20($sp)
	addi $sp, $sp, 76
	jr $ra

