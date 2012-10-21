#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

#include <pthread.h>

#include "rusage.h"
#include "atomic.h"

void panic(char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

typedef unsigned char byte;

atomic_int currConc = ATOMIC_INT(0);
atomic_int maxConc = ATOMIC_INT(0);
atomic_int total = ATOMIC_INT(0);

void *agent(void *_sock) {
	int sock = (intptr_t)_sock;
	byte buf[100];
	int len = read(sock, buf, sizeof(buf));
	if (len < 0) {
		perror("read");
		return NULL;
	}
	const char *msg = "ECHO: ";
	if (write(sock, msg, strlen(msg)<0)) {
		perror("write");
		return NULL;
	}
	if (write(sock, buf, len)<0) {
		return NULL;
	}
	if (close(sock)!=0) {
		perror("close");
		return NULL;
	}
	atomic_int_add(&currConc, -1);
	return NULL;
}

int close_sock;
atomic_int is_exit = ATOMIC_INT(0);

void set_exit(int sig) {
	puts("EXITING");
	atomic_int_set(&is_exit, 1);
	if (close(close_sock)!=0) {
		perror("exit sock");
	}
}

int main(int argc, char **argv) {
	int sock;
	int one = 1;
	int rv;
	struct addrinfo *addr;
	struct addrinfo *it = NULL;
	struct addrinfo hint = {AF_UNSPEC, PF_INET, AI_PASSIVE};
	
	signal(SIGINT, set_exit);
	if ((rv = getaddrinfo(NULL, "1234", &hint, &addr))!=0) {
		printf("rv=%s\n", gai_strerror(rv));
	}
	for (it = addr; it!=NULL; it = it->ai_next) {
		sock = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
		if (sock == -1) continue;

		setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
		if (bind(sock, it->ai_addr, it->ai_addrlen) == 0) break;

		close(sock);
		perror("bind");
	}
	if (it==NULL) {
		fprintf(stderr, "could not bind");
		return -1;
	}
	listen(sock, 50);
	close_sock = sock;
	while (1) {
		struct addrinfo client_addr;
		socklen_t addr_len = sizeof(client_addr);
		pthread_t t;
		int conc;
		int client_sock = accept(sock, (struct sockaddr *) &client_addr, &addr_len);
		if (atomic_int_get(&is_exit)!=0) break;
		if (client_sock==-1) panic("accept");
		conc = atomic_int_add(&currConc, 1);
		atomic_int_add(&total, 1);
		while (conc>atomic_int_get(&maxConc) && !atomic_int_compare_and_swap(&maxConc, atomic_int_get(&maxConc), conc));
		pthread_create(&t, NULL, agent, (void*)(intptr_t)client_sock);
	}
	// if we got here, we already close(sock);
	freeaddrinfo(addr);
	print_rusage();
	printf("Max conc: %d\n", atomic_int_get(&maxConc));
	printf("Total: %d\n", atomic_int_get(&total));
	printf("Now: %d\n", atomic_int_get(&currConc));
	return 0;
}
