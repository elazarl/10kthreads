CFLAGS=-Wall #-Wextra
bin/10kthreads: obj/10kthreads.o obj/rusage.o
	gcc $(CFLAGS) -pthread  $^ -o $@
obj/10kthreads.o: 10kthreads.c rusage.h atomic.h atomic_gcc.h atomic_pthread.h
	gcc -c $(CFLAGS) $< -o $@
obj/rusage.o: rusage.c
	gcc -c $(CFLAGS)  $< -o $@
.PHONY: clean
clean:
	find obj/ bin/ -type f ! -name README|xargs rm
