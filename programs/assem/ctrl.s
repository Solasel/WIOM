addi sp x0 -1

# Set up values for branch comparison.
addi s0 x0 -1
addi s1 x0 1

# Set up a bad loop that we'll get caught in we
# 	take a bad jump.
# Also set up an intermediary for the jump test.
jal x0 jtest

bad: jal x0 bad

jtest0: jal x0 jtest1 # Jump ahead to jtest1
btest0: beq x0 x0 btest1 # Jump forward
jal x0 bad

# jal and jalr testing.
jtest:

jal x0 jtest0 # Jump back to jtest0
jal x0 bad

jtest1: jal ra jtest2 # Jump ahead to jtest2
jal x0 jtest_hard # Jump to the harder tests.
jal x0 bad

jtest2: jalr x0 0(ra) # Jump back to ra
jal x0 bad

# Complex jalr testing.
jtest_hard:

# Testing jalr with offset.
jal ra jtest3
jal x0 bad
jal x0 jtest4

jtest3: jalr x0 4(ra)
jal x0 bad

# Testing circular jalr usage.
jtest4: jal ra jtest5
jtest5: jalr ra 0(ra)

jal ra jtest6
jal x0 bad
jtest6: jalr ra 4(ra)
jal x0 bad

# Branch tests!

# If you take any of these you're in trouble.
bne x0 x0 bad
beq x0 s0 bad
blt x0 s0 bad
bge x0 s1 bad
bltu x0 x0 bad
bgeu x0 s1 bad

# Load some nasty stuff in the branch delay slot.
beq x0 x0 btest0
jal ra bad
jalr x0 -4(ra)

btest1: add x0 x0 x0

