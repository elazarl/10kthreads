#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include "rusage.h"

void print_rusage(void) {
	struct rusage u;

	if (getrusage(RUSAGE_SELF,&u)==0) {
		puts("resident ");
		printf("%lu", u.ru_maxrss);
		puts("\nshared ");
		printf("%lu", u.ru_ixrss);
		puts("\nunshared data ");
		printf("%lu", u.ru_idrss);
		puts("\nstack ");
		printf("%lu\n", u.ru_isrss);
	} else {
		perror("getrusage");
	}
}
