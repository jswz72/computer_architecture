.data
prompt: .asciiz "Input int\n"
result: .asciiz "Result "
.text

main:
	#show prompt
	la 	$a0, prompt
	li	$v0, 4
	syscall
	#read input
	li	$v0, 5	#syscall code for read int
	syscall
	move 	$a0, $v0  #move input to arg
	jal fact
	move	$t0, $v0  #move result of function to temp0
	#show result string
	la	$a0, result 
	li	$v0, 4
	syscall
	#show stored result
	move	$a0, $t0
	li	$v0, 1
	syscall
	#terminate
	li	$v0, 10
	syscall
	

fact:
	addi	$sp, $sp, -8	 #stack needs 2 items
	sw	$ra, 4($sp)	#save ra cuz "caller"
	sw	$a0, 0($sp)	#save arg cuz also "caller"
	slti	$t0, $a0, 1	#t0 = a0 < 1 (int)
	beq	$t0, $0, L1	#if greater than 1 continue with func
	#else fact of 1 is 1
	addi	$v0, $zero, 1	#move 1 to return
	addi	$sp, $sp, 8	#restore stack (pop 2 items)
	jr	$ra
L1:
	#recurse and continue part
	addi	$a0, $a0, -1	#decrement arg
	jal	fact		#recurse
	#after return from recursion
	lw	$a0, 0($sp)	#restore orig arg before recursion
	lw	$ra, 4($sp)	#restore orig return addr before recursion
	addi	$sp, $sp, 8	#pop 2 items
	mul	$v0, $a0, $v0	#ans is cur arg * result from recurse
	jr	$ra
	