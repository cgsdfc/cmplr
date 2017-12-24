.data
__asciiz_4: .asciiz "gcd 160, 40 "
__asciiz_5: .asciiz "gcd 180 20 "
.text
.globl main
	jal main
	li $v0, 10
	syscall

mod:
	addi $sp, $sp, -20
	sw $s0, 0($sp)
	sw $a0, 20($sp)
	sw $a1, 24($sp)
	addi $a0, $sp, 20
	lw $a0, 0($a0)
	addi $a0, $sp, 20
	lw $a0, 0($a0)
	addi $a1, $sp, 24
	lw $a1, 0($a1)
	div $a0, $a1
	mflo $t0
	addi $a1, $sp, 24
	lw $a1, 0($a1)
	mult $t0, $a1
	mflo $t1
	sub $t2, $a0, $t1
	addi $s0, $sp, 4
	sw $t2, 0($s0)
	addi $s0, $sp, 4
	lw $s0, 0($s0)
	move $v0, $s0
	j __label_0

__label_0:
	lw $s0, 0($sp)
	addi $sp, $sp, 20
	jr $ra

gcd:
	addi $sp, $sp, -44
	sw $ra, 8($sp)
	sw $a0, 44($sp)
	sw $a1, 48($sp)
	addi $a1, $sp, 48
	lw $a1, 0($a1)
	li $t0, 0
	bne $a1, $t0, __label_1
	li $t1, 0
	move $v0, $t1
	j __label_2
	j __label_2

__label_1:
	addi $a1, $sp, 48
	lw $a1, 0($a1)
	addi $a0, $sp, 44
	lw $a0, 0($a0)
	sw $t0, 12($sp)
	sw $t1, 20($sp)
	sw $a0, 44($sp)
	sw $a1, 48($sp)
	move $a0, $a0
	move $a1, $a1
	jal mod
	lw $t0, 12($sp)
	lw $t1, 20($sp)
	move $t2, $v0
	lw $a0, 44($sp)
	lw $a1, 48($sp)
	li $t3, 0
	bne $t2, $t3, __label_3
	addi $a1, $sp, 48
	lw $a1, 0($a1)
	move $v0, $a1
	j __label_2
	j __label_2

__label_3:
	addi $a1, $sp, 48
	lw $a1, 0($a1)
	addi $a0, $sp, 44
	lw $a0, 0($a0)
	sw $t0, 12($sp)
	sw $t1, 20($sp)
	sw $t2, 24($sp)
	sw $t3, 28($sp)
	sw $a0, 44($sp)
	sw $a1, 48($sp)
	move $a0, $a0
	move $a1, $a1
	jal mod
	lw $t0, 12($sp)
	lw $t1, 20($sp)
	lw $t2, 24($sp)
	lw $t3, 28($sp)
	move $t4, $v0
	lw $a0, 44($sp)
	lw $a1, 48($sp)
	addi $a1, $sp, 48
	lw $a1, 0($a1)
	sw $t0, 12($sp)
	sw $t1, 20($sp)
	sw $t2, 24($sp)
	sw $t3, 28($sp)
	sw $t4, 36($sp)
	sw $a0, 44($sp)
	sw $a1, 48($sp)
	move $a0, $a1
	move $a1, $t4
	jal gcd
	lw $t0, 12($sp)
	lw $t1, 20($sp)
	lw $t2, 24($sp)
	lw $t3, 28($sp)
	lw $t4, 36($sp)
	move $t5, $v0
	lw $a0, 44($sp)
	lw $a1, 48($sp)
	move $v0, $t5
	j __label_2

__label_2:
	lw $ra, 8($sp)
	addi $sp, $sp, 44
	jr $ra

main:
	addi $sp, $sp, -44
	sw $ra, 8($sp)
	la $t0, __asciiz_4
	move $a0, $t0
	li $v0, 4
	syscall
	li $t1, 40
	li $t2, 160
	sw $t0, 12($sp)
	sw $t1, 16($sp)
	sw $t2, 20($sp)
	move $a0, $t2
	move $a1, $t1
	jal gcd
	lw $t0, 12($sp)
	lw $t1, 16($sp)
	lw $t2, 20($sp)
	move $t3, $v0
	move $a0, $t3
	li $v0, 1
	syscall
	la $t4, __asciiz_5
	move $a0, $t4
	li $v0, 4
	syscall
	li $t5, 20
	li $t6, 180
	sw $t0, 12($sp)
	sw $t1, 16($sp)
	sw $t2, 20($sp)
	sw $t3, 24($sp)
	sw $t4, 28($sp)
	sw $t5, 32($sp)
	sw $t6, 36($sp)
	move $a0, $t6
	move $a1, $t5
	jal gcd
	lw $t0, 12($sp)
	lw $t1, 16($sp)
	lw $t2, 20($sp)
	lw $t3, 24($sp)
	lw $t4, 28($sp)
	lw $t5, 32($sp)
	lw $t6, 36($sp)
	move $t7, $v0
	move $a0, $t7
	li $v0, 1
	syscall
	lw $ra, 8($sp)
	addi $sp, $sp, 44
	jr $ra
