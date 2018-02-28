#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <uthread.h>
#include <preempt.h>




int fatty () {

		while(1) {
			printf("fatty\n");
		}
		return 0;
}

int skinny () {
	printf("im skinny\n");
	return 0;
}

int main ()
{
int i;
int k;

i = uthread_create(fatty, NULL);
k = uthread_create(skinny, NULL);
uthread_join(i, &k);
return 0;
}