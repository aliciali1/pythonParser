build:
	rm -f ./a.out
	gcc -std=c11 -g -Wall main.c execute.c parser.c programgraph.o ram.c scanner.o tokenqueue.o -lm -Wno-unused-variable -Wno-unused-function

run:
	./a.out

valgrind:
	rm -f ./a.out
	gcc -std=c11 -g -Wall main.c execute.c parser.c programgraph.o ram.c scanner.o tokenqueue.o -lm -Wno-unused-variable -Wno-unused-function
	valgrind --tool=memcheck --leak-check=no --track-origins=yes ./a.out

submit:
	/home/cs211/s2024/tools/project02  submit  main.c execute.c

objectfiles:
	rm -f *.o
	gcc -std=c11 -g -c -Wall parser.c
	gcc -std=c11 -g -c -Wall programgraph.c
	gcc -std=c11 -g -c -Wall ram.c
	gcc -std=c11 -g -c -Wall scanner.c
	gcc -std=c11 -g -c -Wall tokenqueue.c
