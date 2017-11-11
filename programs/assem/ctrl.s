addi sp x0 -1 #initialize values
addi ra x0 1

add t0 x0 x0
add t1 x0 x0

jal x0 start #jump over the bad catcher

badjump: jal x0 badjump #infinite loops if we take a jump we shouldn't have

add x0 x0 x0 #part 1

start: addi a0 x0 5 
jal s0 loop #go to loop, a function that decrements a0 until it reaches 0.
jal x0 cont #go on with the program

loop: beq a0 x0 return
addi a0 a0 -1
jal x0 loop
return: jalr x0 s0 0

cont: blt sp x0 cont2 #should be taken because -1 < 0
jal x0 badjump

cont2: blt x0 ra cont3 #should be taken because 0 < 1
jal x0 badjump

cont3: bltu sp x0 badjump #should NOT be taken because -1 == some huge number unsigned, which is not less than 0
beq x0 x0 cont4 #should be taken since 0 == 0
jal x0 badjump

cont4: addi a0 x0 3
jal s0 loop #call loop again, because why not
addi t0 x0 5

add x0 x0 x0 #part 2

jal ra next #jump to next
next: jalr ra ra 0 #first jump here again, then jump to the next instruction since we updated ra

jal ra next2 #jump to next2
add x0 x0 x0
next2: jalr ra ra 4 #first jump here again, then jump OVER the bad jump since we updated ra
jal x0 badjump

addi t1 x0 5
