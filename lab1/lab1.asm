# Jacob Sword & Daniel Richter
# Computer Architecture and Design (IEECE 4820)
# Lab Assignment 1

.data
result: .asciiz "Result: "
nomatch: .asciiz "No Matches Found"
str1: .asciiz "asdf"
str2: .asciiz "sad"
.text

main:
	la	$a0, str1
	la	$a1, str2
	jal firstmatch
	move	$t0, $v0  #store fm return in temp0
	#show result string
	la	$a0, result 
	li	$v0, 4
	syscall
	#show stored result
	beq	$t0, $zero, no_matches
	lb	$a0, ($t0)	#get char from returned addr
	li	$v0, 11		#syscall for print char
	syscall
exit:
	li	$v0, 10
	syscall 
	
no_matches:
	la	$a0, nomatch
	li	$v0, 4
	syscall
	j	exit

firstmatch:
	addi	$sp, $sp, -12	#make room for args and ra
	sw	$ra, -8($sp)	#store return addr
	sw	$a0, -4($sp)	#store first arg
	sw	$a1, ($sp)	#store second arg
	move	$s0, $a0	#s0 is first arg (temp)
fm_loop:
	lw	$a0, ($sp)	#get back s2 and pass as first arg
	lb	$a1, ($s0)	#pass *temp as secon arg
	jal	strchr
	beq	$v0, $zero, fm_continue	#continue with loop if zero returned
	move	$v0, $s0	#move temp to return
	lw	$ra, -8($sp)	#get back ret addr
	addi	$sp, $sp, 12	#pop used stack space
	jr 	$ra

fm_continue:
	addi	$s0, $s0, 1	#increment temp ptr
	lb	$t0, ($s0)	#to = *temp
	beq	$t0, $zero, fm_ret_0
	j	fm_loop

fm_ret_0:
	li	$v0, 0
	lw	$ra, -8($sp)	#get back ret addr
	addi	$sp, $sp, 12	#pop stack
	jr	$ra
	
	
strchr:
sc_loop:
	lb 	$t1, ($a0)		#t1 == *s
	beq 	$t1, $a1, sc_ret_char #return if *s == c
	addi 	$a0, $a0, 1	#increment pointer
	lb 	$t2, ($a0)		#t2 = *s new value
	beq 	$t2, $zero, sc_ret_0  #return zero if value is 0 (null char?)
	j 	sc_loop

sc_ret_char:
	move 	$v0, $a0	#move s to return
	jr 	$ra	#return from func
sc_ret_0:
	li 	$v0, 0
	jr 	$ra
