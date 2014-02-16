.globl partition
### Previous Test data
#.data
#test: .word 1 3 4 2 5                   , 0 0 0 0

#.text
#la $a0, test
#la $a1, test + 20

#####
#Registers / Purpose:
# $t0 / pivot
# $t1 / temp variable to hold left element
# $t2 / temp variable to hold right element
# $t3 / Left element needs to be swapped check (0 = no swap, 1 = swap)
# $t4 / Right element needs to be swapped check (0= no swap, 1 = swap)
# $a0 / Address of start
# $a1 / Address of End (and pivot / last element being compared)
# $ra / return address


partition:			#Partition Global Function / Setting up variables
addi $sp, $sp, -16		#Allocate Space on Stack
sw $ra, 0($sp)			#save previous $ra, $a0, $a1, $s0 on stack
sw $a0, 4($sp)
sw $a1, 8($sp)
addi $a1, $a1, -4		#decrement last address by 1 word, to retrieve last word
lw $t0, 0($a1)			#t0 = pivot (last element)
addi $a1, $a1, -4		#decrement 1 word from pivot (last element
li $t3, 0			#Set Checks (Left and Right Sides) to 0 (initialize)
li $t4, 0			#t3 = left, t4 = right

partition_loop:			#Partition Loop / Checks if elements need to be swapped
bge $a0,$a1, insert_pivot	#if address of first element is greater or eyal to end, insert pivot
lw $t1, 0($a0)			#Reset Checks
lw $t2, 0($a1)
bgt $t1, $t0, swap_left		#If Left > Pivot, branch to swap left (which sets swap left check to 1)
blt $t2, $t0, swap_right	#If Right < Pivot, branch to swap right (sets right check to 1)
addi $a0, $a0, 4		#If none of previous cases met, increment right, and decrement left
addi $a1, $a1, -4
j partition_loop		#Return to loop

swap_left:			#Simple function that sets left check $t3 to 1, indicating it needs to be swapped
li $t3, 1
j partition_check

swap_right:			#Simple function that sets right check $t4 to 1, indicating it needs to be swapped
li $t4, 1
j partition_check

partition_check:		#Checks if both sides need to be swapped
beq $t3, 0, increment_left	#if left check = 0 (Doesn't Need to be swapped) go to increment left
beq $t4, 0, decrement_right	#if right check = 0 (Doesn't Need to be swapped) go to decrement right
jal swap			#Both Checks = 1, swap the elements
li $t3, 0			#reset checks
li $t4, 0
addi $a0, $a0, 4		#increment right element and decrement right
addi $a1, $a1, -4
j partition_loop		#jump to initial partition loop (to reload and check new elements)

increment_left:			#If right side needs to be swapped, increment left until it needs to be swapped (or not at all)
bgt $t1, $t0, swap_left		#if left > pivot, branch to set check in swap_left
addi $a0, $a0, 4		#increment left side by 1 word
bge $a0,$a1, insert_pivot	#check to see if we need to insert pivot (left address greater or equal to right address)
lw $t1, 0($a0)			#load new left element into $t1	
j increment_left		#loop until left needs to be swapped or pivot needs to be inserted

decrement_right:		#If left side needs to be swapped, decrement right until it needs to be swapped (or not at all)
blt $t2,$t0, swap_right		#if right < pivot branch to set check in swap_right
addi $a1, $a1, -4		#decrement right side by 1 word
bge $a0,$a1, insert_pivot	#check for exit condition (insert pivot)
lw $t2, 0($a1)			#load new right element in $t2
j decrement_right		#loop until right needs to be swapped (or pivot needs to be inserted)

insert_pivot:			#Insert Pivot Function: restores original array start and end locations (from stack)
lw $a0, 4($sp)			#Retrieve $a0 from stack (beginning)	
lw $a1, 8($sp)			#Retrieve $a1 from stack (end)
addi $a1, $a1, -4		#Decrement address of back to get last word in array

insert_pivot_loop:		#Checks for first element greater than pivot in array, then swaps with that found element
lw $t0, 0($a0)			#load first element into $t0
lw $t1, 0($a1)			#load pivot into $a1
bge $a0, $a1, exit		#exit check (if pivot is greatest element in array, $a0 doesn't change)
bgt $t0,$t1,insert_pivot2	#if $t0 is greater than pivot, go to insert_pivot2 (just swaps and exits)
addi $a0,$a0,4			#increment starting address by 1 word
j insert_pivot_loop		#repeat until pivot is swapped or exits

insert_pivot2:			#simple function, justs swaps pivot with a element greater than it and exits
jal swap
j exit

exit:				#exit function
la $v0, -4($a0)			#sets v0 to the address of the element to the left of the pivot
la $v1, 4($a0)			#sets v1 to the address of the element to the right of the pivot
lw $a1, 8($sp)			#restores values $a0, $a1, $ra clobbered in function to original values (stored on stack)
lw $a0, 4($sp)
lw $ra, 0($sp)
addi $sp, $sp, 16		#restore space allocated on stack
jr $ra				#return to caller

### Previous Test data
#move $a1,$a0
#test_part:
#lw $s3, 0($a1)
#beqz $s3, derpy
#move $a0, $s3
#li $v0, 1
#syscall
#addi $a1, $a1, 4
#j test_part
