#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

sigset_t block_mask;
struct sigaction sa;
struct itimerval timer;

void timer_handler (int signum)
{
	//printf("hey\n");
	uthread_yield();
}

void preempt_disable(void)
{
	//printf("disable\n");
	sigprocmask(SIG_BLOCK, &block_mask, NULL);
}

void preempt_enable(void)
{
	//printf("enable\n");
	sigprocmask(SIG_UNBLOCK, &block_mask, NULL);
}

void preempt_start(void)
{

 struct sigaction sa;
 struct itimerval timer;

 memset (&sa, 0, sizeof (sa));
 sa.sa_handler = &timer_handler;
 sigemptyset(&block_mask);
 sigaddset(&block_mask, SIGVTALRM);
 sigaction(SIGVTALRM, &sa, NULL);
 

 timer.it_value.tv_sec = 0;
 timer.it_value.tv_usec = HZ;
 
 timer.it_interval.tv_sec = 0;
 timer.it_interval.tv_usec = HZ;

 setitimer (ITIMER_VIRTUAL, &timer, NULL);

}

