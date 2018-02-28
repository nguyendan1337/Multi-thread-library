#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

typedef struct node{
	void *data;
	struct node *next;
} NODE;

struct queue {
	int size;
	NODE *head;
	NODE *tail;
};


queue_t queue_create(void)
{
	struct queue newQueue;
	newQueue.size = 0;
	newQueue.head = NULL;
	newQueue.tail = NULL;

	queue_t ptr = malloc(sizeof(struct queue));
	*ptr = newQueue;

	return ptr;
}

int queue_destroy(queue_t queue)
{
	if(queue == NULL || queue->size != 0) {
		return -1;
	} else {
		free(queue->head);
		free(queue);
		return 0;
	}
}

int queue_enqueue(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL) {
		return -1;
	}

	NODE *new;

	new = malloc(sizeof(NODE));

	NODE newNode;
	newNode.data = data;
	newNode.next = NULL;
	*new = newNode;

	if(queue->size == 0) {
		queue->head = new;
		queue->tail = new;
	} else {
		queue->tail->next = new;
		queue->tail = new;
	}
	
	queue->size++;

	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if(queue == NULL || data == NULL || queue->size <= 0) {
		return -1;
	} else {
		*data = queue->head->data;
		NODE *toDelete = queue->head;
		queue->head = queue->head->next;
		free(toDelete);
		--queue->size;
		return 0;
	}
}

int queue_delete(queue_t queue, void *data)
{

	if(queue == NULL || data == NULL) {
		return -1;
	}

	NODE *current = queue->head;

	while(current != NULL) {

		if(current->next != NULL && current->next->data == data) {
			NODE *toDelete = current->next;

		if(current->next->next == NULL) {
			current->next = NULL;
		} else {
			current->next = current->next->next;
		}

			free(toDelete);
			queue->size--;
			return 0;
		}
		
    current = current->next;
    
	}

	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{

	if(queue == NULL || func == NULL) {
		return -1;
	}

	NODE *current = queue->head;

	while(current != NULL) {

    	if(func(queue, current->data, arg) == 1){
			*data = current->data;
		    return 0;
		}
		
      
      current = current->next;
      
    }
    
	return 0;
}

int queue_length(queue_t queue)
{
	if(queue == NULL){
		return -1;
	} else {
		return queue->size;
	}
}
