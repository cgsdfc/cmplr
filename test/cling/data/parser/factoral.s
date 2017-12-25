.data
__asciiz_2: .asciiz "factoral 1 "
.text
.globl main
	jal main
	li $v0, 10
	syscall

factoral:
	addi $sp, $sp, -36
	sw $ra, 4($sp)
	sw $a0, 36($sp)
	addi $a0, $sp, 36
	lw $a0, 0($a0)
	li $t0, 2
	sub $t1, $a0, $t0
	bgez $t1, __label_0
	li $t2, 1
	move $v0, $t2
	j __label_1

__label_0:
	addi $a0, $sp, 36
	lw $a0, 0($a0)
	addi $a0, $sp, 36
	lw $a0, 0($a0)
	li $t3, 1
	sub $t4, $a0, $t3
	sw $t0, 8($sp)
	sw $t1, 12($sp)
	sw $t2, 16($sp)
	sw $t3, 20($sp)
	sw $t4, 24($sp)
	sw $a0, 36($sp)
	move $a0, $t4
	jal factoral
	lw $t0, 8($sp)
	lw $t1, 12($sp)
	lw $t2, 16($sp)
	lw $t3, 20($sp)
	lw $t4, 24($sp)
	move $t5, $v0
	lw $a0, 36($sp)
	mult $a0, $t5
	mflo $t6
	move $v0, $t6
	j __label_1

__label_1:
	lw $ra, 4($sp)
	addi $sp, $sp, 36
	jr $ra

main:
	addi $sp, $sp, -20
	sw $ra, 4($sp)
	la $t0, __asciiz_2
	move $a0, $t0
	li $v0, 4
	syscall
	li $t1, 5
	sw $t0, 8($sp)
	sw $t1, 12($sp)
	move $a0, $t1
	jal factoral
	lw $t0, 8($sp)
	lw $t1, 12($sp)
	move $t2, $v0
	move $a0, $t2
	li $v0, 1
	syscall
	lw $ra, 4($sp)
	addi $sp, $sp, 20
	jr $ra
factoral 1 120