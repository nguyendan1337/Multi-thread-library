/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int hello(void* arg)
{
	printf("Hello world!\n");
	printf("Leaving hello!\n");
	return 0;
}

int main(void)
{
	uthread_t tid;

	tid = uthread_create(hello, NULL);
	uthread_join(tid, NULL);
	printf("Back from hello.\n");

	return 0;
}
