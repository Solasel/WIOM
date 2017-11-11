addi sp x0 -1

# Assume all registers other than sp are 0.

# Set s0 to be the useful value 0x12345678.
# Requires: lui, addi, ALU->ALU forwarding on rs1.
lui s0 74565
addi s0 s0 1656

# Changes: s0 = 0x12345678

# Add the previously computed s0 to itself.
# Requires: add, ALU->ALU forwarding on rs1, rs2.
add t0 s0 s0
add t1 s0 s0
add t2 s0 s0

# Changes: t0 = t1 = t2 = 2468acf0

add t0 x0 x0
add t1 x0 x0
add t2 x0 x0

# Store the value 0x12345678 to address 555,
# then immediately try to read it and use it.
# Requires: sw, lw, add, stalling on arithmetic op
# 	dependent on a preceding load, and MEM->ALU 
#	forwarding on rs1 and rs2.
addi s1 x0 555
sw s0 0(s1)
lw t0 0(s1)
add t1 t0 t0
add t2 t0 t0

# Changes: s1 = 0x0000022b, t0 = 0x12345678, t1 = t2 = 0x2468acf0

add t0 x0 x0
add t1 x0 x0
add t2 x0 x0

# Load 0x12345678 to t1, then immediately stores it to address 559.
# Checks for MEM->MEM forwarding on rs2.
# Requires: lw, sw, MEM->MEM forwarding on rs2.
lw t0 0(s1)
sw s0 4(s1)
lw t1 4(s1)

# Changes: t0 = t1 = 0x12345678

add t0 x0 x0
add t1 x0 x0

# Compute 3 values for t0, then check to see that we
# forward the most recent version.
# Requires: addi, add, ALU->ALU forwarding.
addi t0 x0 1
addi t0 x0 2
add t0 x0 s0
add t1 x0 t0
add t2 x0 t0
add t3 x0 t0

# Changes: t0 = t1 = t2 = t3 = 0x12345678

add t0 x0 x0
add t1 x0 x0
add t2 x0 x0
add t3 x0 x0

# Make sure we don't stall when "loading" to x0.
# Requires: lw, add, stalling on arithmetic op
# 	dependent on preceding load.
lw x0 0(s0)
add x0 x0 x0

# Changes: None. (NO STALL!!)

# Make sure you don't forward values for x0.
# First "set" x0 to 666, then add it to itself and store in t0.
# Then, "load" 0x12345678 to x0, and store it in x0. Then manually
# 	check memory address 0x12345678 to see that it wasn't set.
# Requires: addi, lw, sw, ALU->ALU forwarding on
# 	rs1 and rs2, MEM->MEM forwarding on rs2.
addi x0 x0 666
add t0 x0 x0

lw x0 0(s1)
sw x0 0(x0)
lw t1 0(s0)

# Changes: t0 = 0x00000000, t1 = 0x00000000

add t0 x0 x0
add t1 x0 x0

