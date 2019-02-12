.data
result: .asciiz "Result: "
str1: .asciiz "asdf"
str2: .asciiz "sad"
.text

main:
	la	$a0, str1
	la	$a1, str2
	#call func
	jal firstmatch
	move	$t0, $v0  #store fm return in temp0
	#show result string
	la	$a0, result 
	li	$v0, 4
	syscall
	#show stored result
	move	$a0, $t0
	li	$v0, 4
	syscall
	#exit
	li	$v0, 10
	syscall 

firstmatch:
	addi	$sp, $sp, -12	#make room for args and ra
	sw	$ra, -8($sp)	#store return addr
	sw	$a0, -4($sp)	#store first arg
	sw	$a1, ($sp)	#store second arg
	move	$s0, $a0	#s0 is first arg (temp)
fm_loop:
	move	$a0, $a1	#pass s2 as first arg
	lb	$a1, ($s0)	#pass *temp as secon arg
	jal	strchr
	### todo
	beq	$v0, $zero, fm_continue	#continue with loop if zero returned
	
	li	$v0, 10
	syscall
	
strchr:
sc_loop:
	lb $t1, ($a0)		#t1 == *s
	beq $t1, $a1, sc_ret_char #return if *s == c
	addi $a0, $a0, 1	#increment pointer
	lb $t2, ($a0)		#t2 = *s new value
	beq $t2, $zero, sc_ret_0  #return zero if value is 0 (null char?)
	j sc_loop

sc_ret_char:
	move $v0, $a0	#move s to return
	jr $ra	#return from func
sc_ret_0:
	li $v0, 0
	jr $ra