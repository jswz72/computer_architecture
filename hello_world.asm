.data
hello_world: .asciiz "Hello World\n"

.text
la $a0, hello_world #load addr of str stored in hello_world
li $v0, 4	#load syscall to print to screen
syscall