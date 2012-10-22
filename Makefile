CFLAGS=-Wall #-Wextra
all: bin/10kthreads bin/ref_async_server bin/client
	@echo Success
bin/10kthreads: obj/10kthreads.o obj/rusage.o
	gcc $(CFLAGS) -pthread  $^ -o $@
obj/10kthreads.o: 10kthreads.c rusage.h atomic.h atomic_gcc.h atomic_pthread.h
	gcc -c $(CFLAGS) $< -o $@
obj/rusage.o: rusage.c
	gcc -c $(CFLAGS)  $< -o $@
bin/ref_async_server: ref_async_server/10kthreads.go
	(cd $$(dirname $<);go build -o ../$@)
bin/client: client/conc_tcp_client.go
	(cd $$(dirname $<);go build -o ../$@)
.PHONY: clean all
clean:
	find obj/ bin/ -type f ! -name README|xargs rm
