.data
prompt: .asciiz "Input int\n"
result: .asciiz "You input "

.text
	#show prompt
	la 	$a0, prompt
	li	$v0, 4
	syscall
	#read input
	li	$v0, 5	#syscall code for read int
	syscall
	move 	$t0, $v0  #move input to temp
	# output result str
	la	$a0, result
	li	$v0, 4
	syscall
	move	$a0, $t0
	#output num
	li	$v0, 1	#syscall code for output int
	syscall
	