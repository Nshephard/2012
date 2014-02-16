.globl quicksort

#### Previous Test Data    
#.data
#test: .word 7 1 3 9 6 8 5 12 11 10 4 2        , 0 0 0 0

#.text
# la $a0, test
# li $a1, 12

quicksort:
	ble	$a1, 1, exit		#If size of array is less than or equal to 1 exit
	mul	$a1, $a1, 4		#multiply size by 4 to increment address to length of array
	add	$a1, $a1, $a0		#add number of words (currently $a1) to inital address to become address of end
	addi	$sp, $sp, -20		#Allocate space to save values on stack
	sw	$ra, 0($sp)		#save values $ra,$a0,$a1 on stack
	sw	$a0, 4($sp)		
	sw	$a1, 8($sp)		
	jal	partition		#parition array
	
	#Left Side
	sw	$v0, 12($sp)		#store address of element left of pivot on stack(stored in $v0 from partition)
	sw	$v1, 16($sp)		#store address of element right of pivot on stack (stored in $v1 from partition)
	lw	$a0, 4($sp)		#re-Load $a0, from its saved location on stack (the address from beginning of array)
	sub 	$a1, $v0, $a0		#restoring $a1 to total number of elements on the left (left of pivot address - initial address)
	addi	$a1, $a1, 4		#increment to include 1 last word (pivot)
	div 	$a1, $a1, 4		#divide to convert from words to number of elements (words / 4 = # remaining elements)
	jal	quicksort
	#Right Side
	lw	$a0, 16($sp)		#set $a0 (start of array) to right of original pivot (address stored in 16($sp) @ line 23)
	lw	$a1, 8($sp)		#load $a1 to original end of array (stored @ 8($sp) on stack)
	sub 	$a1, $a1, $a0		#subtract end of array - beginning of array to get size
	div	$a1, $a1, 4		#divide to convert from words to number of elements (words / 4 = # remaining elements)
	jal	quicksort		# recurse for lhs
	
	lw	$a0, 4($sp)		#restore $a0, $a1, $ra to original values before function used (caller saved)
	lw	$a1, 8($sp)		
	lw	$ra, 0($sp)		
	addi	$sp, $sp, 20		#Restore space used on stack
	jr	$ra			#return to caller

exit:
	jr $ra				#return to caller
