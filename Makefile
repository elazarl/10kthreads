CFLAGS=-Wall #-Wextra
bin/10kthreads: 10kthreads.c obj/rusage.o
	gcc $(CFLAGS) -pthread  $^ -o $@
obj/rusage.o: rusage.c
	gcc -c $(CFLAGS)  $^ -o $@
