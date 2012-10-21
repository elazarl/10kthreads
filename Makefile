CFLAGS=-Wall #-Wextra
10kthreads: 10kthreads.c rusage.o
	gcc $(CFLAGS) -pthread  $^ -o $@
rusage.o: rusage.c
	gcc -c $(CFLAGS)  $^ -o $@
