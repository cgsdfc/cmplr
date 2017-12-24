.data
global_int: .word 0
global_char: .byte 0
__asciiz_0: .asciiz "test_local "
__asciiz_1: .asciiz "test_global "
.text
.globl main
	jal main
	li $v0, 10
	syscall

test_local:
	addi $sp, $sp, -16
	sw $s0, 0($sp)
	sw $s1, 4($sp)
	addi $s0, $sp, 8
	li $v0, 5
	syscall
	sw $v0, 0($s0)
	addi $s0, $sp, 8
	lw $s0, 0($s0)
	move $a0, $s0
	li $v0, 1
	syscall
	addi $s1, $sp, 12
	li $v0, 12
	syscall
	sb $v0, 0($s1)
	addi $s1, $sp, 12
	lb $s1, 0($s1)
	move $a0, $s1
	li $v0, 11
	syscall
	lw $s0, 0($sp)
	lw $s1, 4($sp)
	addi $sp, $sp, 16
	jr $ra

test_global:
	addi $sp, $sp, -16
	la $t0, global_int
	li $v0, 5
	syscall
	sw $v0, 0($t0)
	la $t1, global_int
	lw $t1, 0($t1)
	move $a0, $t1
	li $v0, 1
	syscall
	la $t2, global_char
	li $v0, 12
	syscall
	sb $v0, 0($t2)
	la $t3, global_char
	lb $t3, 0($t3)
	move $a0, $t3
	li $v0, 11
	syscall
	addi $sp, $sp, 16
	jr $ra

main:
	addi $sp, $sp, -40
	sw $s0, 0($sp)
	sw $ra, 4($sp)
	li $t0, 0
	addi $s0, $sp, 8
	sw $t0, 0($s0)
	j __label_2

__label_4:
	addi $s0, $sp, 8
	lw $s0, 0($s0)
	li $t1, 0
	sub $t2, $s0, $t1
	bgtz $t2, __label_3

__label_2:
	la $t3, __asciiz_0
	move $a0, $t3
	li $v0, 4
	syscall
	sw $t0, 12($sp)
	sw $t1, 16($sp)
	sw $t2, 20($sp)
	sw $t3, 24($sp)
	jal test_local
	lw $t0, 12($sp)
	lw $t1, 16($sp)
	lw $t2, 20($sp)
	lw $t3, 24($sp)
	la $t4, __asciiz_1
	move $a0, $t4
	li $v0, 4
	syscall
	sw $t0, 12($sp)
	sw $t1, 16($sp)
	sw $t2, 20($sp)
	sw $t3, 24($sp)
	sw $t4, 28($sp)
	jal test_global
	lw $t0, 12($sp)
	lw $t1, 16($sp)
	lw $t2, 20($sp)
	lw $t3, 24($sp)
	lw $t4, 28($sp)
	addi $s0, $sp, 8
	lw $s0, 0($s0)
	li $t5, 1
	sub $t6, $s0, $t5
	addi $s0, $sp, 8
	sw $t6, 0($s0)
	j __label_4

__label_3:
	lw $s0, 0($sp)
	lw $ra, 4($sp)
	addi $sp, $sp, 40
	jr $ra
test_local 1
test_global 
$sp=4048
total memory=4096
used memory=64
