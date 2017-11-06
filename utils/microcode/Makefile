gen: generator.c
	$(CC) -o mic_gen generator.c

code: generator.c src_mc.txt
	$(CC) -o mic_gen generator.c
	./mic_gen src_mc.txt final_mc.mem

memleak: generator.c src_mc.txt
	$(CC) -o mic_gen generator.c
	valgrind --tool=memcheck --leak-check=full --track-origins=yes ./mic_gen src_mc.txt tmp
	rm -f tmp

