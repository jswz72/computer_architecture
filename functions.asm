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
	move 	$a0, $v0  #move input to temp
	jal plus_one
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
	

plus_one:
	addi	$v0, $a0, 1
	jr	$ra