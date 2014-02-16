.globl swap
.text
swap:
addi $sp, $sp,-8	#Allocate space on stack for 2 words
sw $s0, 0($sp)		#Save registers used ($s0, $s1) on stack
sw $s1, 4($sp)
lw $s0, 0($a0)		#Load words @ addresses $a0 / $a1
lw $s1, 0($a1)
sw $s1, 0($a0)		#SWAP: Store words @ each others address
sw $s0, 0($a1)
lw $s0, 0($sp)		#Restore previous s register values and restore stack
lw $s1, 4($sp)
addi $sp, $sp,8

jr $ra			#return to caller
