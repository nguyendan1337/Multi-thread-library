#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

// THREAD STATES
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define ZOMBIE 3

typedef struct node {
	void *data;
	struct node *next;
} NODE;

struct queue {
	int size;
	NODE *head;
	NODE *tail;
};

typedef struct TCB {
	uthread_t tid;
	uthread_t parentTID;
	int state;
	uthread_ctx_t *context;
	void *stack;
	int retval;
	int joined;
} TCB;

queue_t queue = NULL;
queue_t zombies = NULL;
uthread_t TID = 0;
TCB *curr_TCB = NULL;

int uthread_init(uthread_func_t func, TCB *thread, void *arg);

int uthread_create(uthread_func_t func, void *arg)
{

	// if first new thread, initialize queue and main thread
	if (queue == NULL) {
		preempt_start();
		zombies = queue_create();
		queue = queue_create();
		curr_TCB = NULL;
		uthread_init(NULL, curr_TCB, arg);
	}

	preempt_disable();

	// create the other thread
	TCB *newThread = malloc(sizeof(TCB));

	if (uthread_init(func, newThread, arg) == -1)
		return -1;


	preempt_enable();

	return newThread->tid;
}

int uthread_init(uthread_func_t func, TCB *tcb, void *arg) {


	TCB thread;
	thread.tid = TID;


	if (func == NULL) {
		curr_TCB = malloc(sizeof(TCB));
		curr_TCB->state = RUNNING;
		curr_TCB->context = malloc(sizeof(uthread_ctx_t));
		curr_TCB->tid = TID;
		curr_TCB->stack = uthread_ctx_alloc_stack();
		uthread_ctx_init(curr_TCB->context, curr_TCB->stack, func, arg);
		TID++;
		return 0;
	}


	thread.stack = uthread_ctx_alloc_stack();
	thread.context = malloc(sizeof(uthread_ctx_t));



	uthread_ctx_init(thread.context, thread.stack, func, arg);
	if (tcb == NULL || thread.stack == NULL || TID == 65535)
		return -1;

	*tcb = thread;
	tcb->state = READY;

	queue_enqueue(queue, (void*)tcb);
	TID++;

	return 0;
}

// Return TID of current thread
uthread_t uthread_self(void)
{
	return curr_TCB->tid;
}


// Yields to the next available and ready thread
void uthread_yield(void)
{
	// Temporary TCB pointer to receive the dequeue
	TCB *new = NULL;
	// Temporary TCB pointer for outgoing thread
	TCB *temp = curr_TCB;
	TCB* ptr;

	int i;
	for (i = 0; i < queue->size; i++) {

		if (((TCB*)(queue->head->data))->state == READY) {
			break;
		}
		queue_dequeue(queue, (void**)&ptr);
		queue_enqueue(queue, ptr);
	}


	if (curr_TCB->state != ZOMBIE) {
		queue_enqueue(queue, temp);
	}

	// Get next thread TCB
	queue_dequeue(queue, (void**) &new);

	// Set to running, have curr_TCB point to the new thread TCB
	new->state = RUNNING;
	curr_TCB = new;

	uthread_ctx_switch(temp->context, new->context);
}


void uthread_exit(int retval)
{

	preempt_disable();
	curr_TCB->state = ZOMBIE;
	curr_TCB->retval = retval;

	TCB* temp = curr_TCB;
	TCB* ptr;
	int i;

	queue_enqueue(zombies, temp);

	for (i = 0; i < queue->size; i++) {

		if (((TCB*)(queue->head->data))->tid == curr_TCB->parentTID) {
			((TCB*)(queue->head->data))->state = READY;
			uthread_yield();
			return;
		}
		queue_dequeue(queue, (void**)&ptr);
		queue_enqueue(queue, ptr);
	}


	preempt_enable();
}

int uthread_join(uthread_t tid, int *retval)
{
	// Check for garbage value or try to join yourself
	if (tid < 0 || curr_TCB->tid == tid)
		return -1;

	NODE *ptr = queue->head;
	TCB * zptr;
	int i;

	for (i = 0; i < queue->size; i++) {
		if (((TCB*)(ptr->data))->tid == tid) {
			break;
		}
		ptr = ptr->next;
	}
	if (((TCB*)(ptr->data))->joined == 1)
		return -1;

	while (1) {
		for (i = 0; i < zombies->size; i++) {
			if (((TCB*)(zombies->head->data))->tid == tid) {
				TCB* temp;
				queue_dequeue(zombies, (void**)&temp);
				retval = &temp->retval;
				curr_TCB->retval = *retval;
				curr_TCB->state = READY;

				free(temp->context);
				free(temp->stack);
				free(temp);
				return 0;
			}
			queue_dequeue(queue, (void**)&zptr);
			queue_enqueue(queue, zptr);
		}

		if (((TCB*)(ptr->data))->tid == tid) {
			curr_TCB->state = BLOCKED;
			((TCB*)(ptr->data))->joined = 1;
			((TCB*)(ptr->data))->parentTID = curr_TCB->tid;
		}

		uthread_yield();
	}

	return 0;
}