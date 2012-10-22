#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include "rusage.h"

void print_rusage(void) {
	struct rusage u;

	if (getrusage(RUSAGE_SELF,&u)==0) {
		printf("resident %luK\n", u.ru_maxrss);
	} else {
		perror("getrusage");
	}
}
