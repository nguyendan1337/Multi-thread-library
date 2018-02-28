#include <stdlib.h>
#include <stdio.h>
#include "queue.h"


typedef struct node{
    void * data;
    struct node * next;
} NODE;

struct queue {
	int size;
  NODE *head;
};



void printQueue(queue_t queue);

int main() {

  int x = 5;
  int y = 6;
  int z = 7;
  int a = 8;
  int b = 9;
  int c = 10;
  void *ptr;

  queue_t queue = queue_create();


  ptr = &x;
  queue_enqueue(queue, ptr);

  printQueue(queue);

  ptr = &y;
  queue_enqueue(queue, ptr);
  printQueue(queue);

  ptr = &z;
  queue_enqueue(queue, ptr);
  printQueue(queue);

  ptr = &y;
  queue_delete(queue, ptr);
  printQueue(queue);

  ptr = &a;
  queue_enqueue(queue, ptr);
  printQueue(queue);

  ptr = &b;
  printf("Trying to delete value that's not in queue: 9\n");
  queue_delete(queue, ptr);
  printQueue(queue);

  ptr = &c;
  queue_enqueue(queue, ptr);
  printQueue(queue);



  void** obj = (void**) malloc(sizeof(void*));
  queue_dequeue(queue, obj);
  printf("Value from dequeue: %d\n", *((int*)(*obj)));

  queue_dequeue(queue, obj);
  printf("Value from dequeue: %d\n", *((int*)(*obj)));

  queue_dequeue(queue, obj);
  printf("Value from dequeue: %d\n", *((int*)(*obj)));

  queue_dequeue(queue, obj);
  printf("Value from dequeue: %d\n", *((int*)(*obj)));

  ptr = &x;


  queue_enqueue(queue, ptr);
  printQueue(queue);

  queue_dequeue(queue, obj);
  printf("Value from dequeue: %d\n", *((int*)(*obj)));


  printQueue(queue);
  if(queue_destroy(queue) == 0)
    printf("Queue destroyed.\n");

  return 0;
}

void printQueue(queue_t queue) {


  printf("Queue size: %d\n", queue->size);

  NODE* current = queue->head;

  while(current != NULL) {
    printf("Current data: %d\n", *((int*)(current->data)));
    current = current->next;
  }
}
