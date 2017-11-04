gen: generator.c
	$(CC) -o test_gen generator.c

test: generator.c microcode.txt
	$(CC) -o test_gen generator.c
	./test_gen microcode.txt target

memleak: generator.c microcode.txt
	$(CC) -o test_gen generator.c
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./test_gen microcode.txt target

