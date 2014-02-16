# You may call these functions
.globl print_array_withz
.globl print_array_int
.globl print_int
.globl endl

#========================================================================
# Some test arrays, in the format
#   length, A { ... }, 0 0 0 0
#
# The four 0-words at the end are buffers to detect overflows.
# The test code prints them out.  They better stay as 4 zeros!

	.data
# degenerate
T0:	.word 0				, 0 0 0 0

# trivial: An array of length 1 is always sorted
T1:	.word 1, 1			, 0 0 0 0

# descending
Td:	.word 8, 5 4 3 2 1 6 8 9	, 0 0 0 0

# Wikipedia
Tw:	.word 9, 3 7 8 5 2 1 9 5 4	, 0 0 0 0

#========================================================================
# brutetest(void)
# Does a "brute-force" sequence of 4 calls to quicksort().
#
# This does prove the following correct:
#  +1.  your `.globl` declaration
#  +2.  your quicksort(A[], len) signature
#  +3.  degenerate-case handling
#
# It does not prove the following:
#  -4.  that your quicksort() is fully working -- that's your job!
#  -5.  This is not a test harness!!
#------------------------------------------------------------------------
.globl brutetest
	.text
brutetest:
  # non-leaf
  addi	$sp, $sp, -4
  sw	$ra, 0($sp)

  la	$a0, T0
  jal	brutetest_ettu

  la	$a0, T1
  jal	brutetest_ettu

  la	$a0, Td
  jal	brutetest_ettu

  la	$a0, Tw
  jal	brutetest_ettu

  # pop
  lw	$ra, 0($sp)
  addi	$sp, $sp, 4
  jr	$ra

#========================================================================
# brutetest_ettu(T)
#   $a0		TestCase * T	for struct TestCase { int len; int A[]; };
# 1.  prints T->A[] before
# 2.  Calls quicksort(T->A[], T->len)
# 3.  prints T->A[] after
# 4.  prints a newline
# Clobbers $a0-$a1.
# Returns nothing.
#------------------------------------------------------------------------

brutetest_ettu:
  # non-leaf
  addi	$sp, $sp, -12
  sw	$ra, 0($sp)

  #	T --->	[_] T.len
  #		[_] T.A		\
  #		...	 	 | == len words
  #		[_]		/

  lw	$a1, 0($a0)	# T->len  ==      * (T + 0)
  addi	$a0, $a0, 4	# T->A	  == <int *>(T + 4)

  # save args while we have them
  sw	$a0, 4($sp)	# [4] = T->A[]
  sw	$a1, 8($sp)	# [8] = T->len
  # 1.
  li	$a2, 4
  jal	print_array_withz

  # 2.
  lw	$a0, 4($sp)	# [4] = T->A[]
  lw	$a1, 8($sp)	# [8] = T->len
  jal	quicksort

  # 3.
  lw	$a0, 4($sp)	# [4] = T->A[]
  lw	$a1, 8($sp)	# [8] = T->len
  li	$a2, 4
  jal	print_array_withz

  # 4.
  jal	endl

  # pop
  lw	$ra, 0($sp)
  addi	$sp, $sp, 12
  jr	$ra

#========================================================================
# print_array_withz(A[], len, lenz)
#   $a0		int A[]
#   $a1		int len
#   $a2		int lenz
# Prints "a0 a1 ... ae | z0 ... ze \n", where
#   "a0 ... ae"	are the len  words of A[]
#   "z0 ... ze" are the lenz words immediately after the end of A[].
# This helps detect overflows.
# Clobbers $a0.
# Returns nothing.
#------------------------------------------------------------------------

print_array_withz:
  # non-leaf
  addi	$sp, $sp, -12
  sw	$ra, 0($sp)

  # Compute end address Z = &A[len + lenz]
  add	$t2, $a1, $a2	# int totz = len + lenz;
  sll	$t2, $t2, 2	# int * Z = ... 4 * totz
  add	$t2, $t2 $a0	#	  + A;
  sw	$t2, 8($sp)	# [8] = Z

  # Compute end address E = &A[len]
  sll	$a1, $a1, 2	# int * E =  ...  4 * len
  add	$a1, $a1, $a0	#	  + A
  sw	$a1, 4($sp)	# [4] = E

  # "a0 ... ae "
  jal	print_array_int	# (A, E) already in $a0, $a1

  # "| "
  la	$a0, S_BAR
  li	$v0, 4
  syscall

  # "z0 ... ze "
  lw	$a0, 4($sp)	# [4] = E
  lw	$a1, 8($sp)	# [4] = Z
  jal	print_array_int	# (E, Z)

  # "\n"
  jal	endl

  # pop
  lw	$ra, 0($sp)
  addi	$sp, $sp, 12
  jr	$ra

	.data
S_BAR:	.asciiz "| "
	.text
#========================================================================
# print_array_int(A, E)
#   $a0		int   A[]
#   $a1		int * E
# Prints "a0 a1 ... ae " (with trailing space), for all words in the
# half-open interval [A, E).
# Clobbers $a0.
# Returns nothing.
#------------------------------------------------------------------------

print_array_int:
  # non-leaf
  addi	$sp, $sp, -4
  sw	$ra, 0($sp)

  move	$t0, $a0	# int * I = A
 print_array_loop:
  bge	$t0, $a1, print_array_wend
			# while (I < E)
  lw	$a0, 0($t0)	#   	       *I
  jal	print_int	#   print_int(...);

  addi	$t0, $t0, 4	#   ++I;
  j	print_array_loop

 print_array_wend:
  # pop
  lw	$ra, 0($sp)
  addi	$sp, $sp, 4
  jr	$ra

#========================================================================
# print_int(a)
#   $a0		int
# Prints "a " (with a trailing space).
# Clobbers $a0.
# Returns nothing.
#------------------------------------------------------------------------

print_int:
  li	$v0, 1		# print_int( a );
  syscall

  li	$a0, ' '
  li	$v0, 11		# print_char(' ');
  syscall

  # frameless leaf
  jr	$ra

#========================================================================
# endl(void)
# Prints "\n".
# Returns nothing.
#------------------------------------------------------------------------
endl:
  li	$a0, '\n'
  li	$v0, 11		# print_char('\n');
  syscall

  # frameless leaf
  jr	$ra
