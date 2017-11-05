gen: generator.c
	$(CC) -o mic_gen generator.c

code: generator.c microcode.txt
	$(CC) -o mic_gen generator.c
	./mic_gen microcode.txt final

memleak: generator.c microcode.txt
	$(CC) -o mic_gen generator.c
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./mic_gen microcode.txt target
	rm -f target

