.data
hello_world: .asciiz "Hello world\n"
other_string: .asciiz "abcdef"
.text


firstmatch:
	move $s1, $a0
	move $s2, $a1	#move char *s1 and char *s2 to saved registers
	add $s3, $zero, $s1	#s3 is equal to char *temp = s1
fm_loop:
	la $a0, $s2
	lw $a1, ($s3)	#load s2 and *temp into args
	## do stack stuff??
	jal strchr
	## do stack stuff?? to get s3 back?
	beq $v0, $zero, fm_continue
	move $v0, $s3
	jr $ra
	
rm_continue:
	addi $s3, $s3, 4	*temp++
	lw $t1, ($s3)		#t1 = *temp
	beq $t1, $zero, rm_ret_0  #return 0 if *temp == 0
	j fm_loop
	
rm_ret_0:
	li $v0, 0
	jr $ra
	
strchr:
sc_loop:
	lw $t1, ($a0)	#t1 == *s
	beq $t1, $a1, sc_ret #return if *s == c
	addi $a0, $a0, 4	#increment pointer
	lw $t2, ($a0)	#t2 = *s new value
	beq $t2, $zero, sc_ret_0  #return zero if value is 0 (null char?)
	j sc_loop

sc_ret_char:
	move $v0, $a0	#move s to return
	jr $ra	#return from func
sc_ret_0:
	li $v0, 0
	jr $ra
	
	
##helo world
la $a0,  hello_world
li $v0, 4 
syscall
li $v0, 10
syscall