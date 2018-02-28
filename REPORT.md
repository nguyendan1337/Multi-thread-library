### P2 report
- Dan Nguyen
- Dario Avetisov


## Approach

Our general approach of this assignment was making sure we understood
the relatioship of our data structures to each other. We knew that
keeping track of how our threads and their structs were moving and loading
had to be precise.


## Relevant Files

uthread_yield.c
---------------

This can be seen in uthread_yield.c, where we have two copies of the
test stored, where one is commented out. We would switch back and forth
between the two for testing, trying to create an implementation of
uthread_join() that would pass both. 

uthread_hello.c
---------------

This was a simpler tester, but it still kept us in check to see if the
core idea was still intact.

queue_test.c
------------

This tester tested multiple enqueues and dequeues, valid and invalid
deletes, and checked that destroy worked. The final version of the file
does not respresent the extent of testing that was done with that file
in previous versions.

test_preempt.c
--------------

This test ensures that a hogging thread is interrupted by the 
preempt mechanism to let another thread execute. Did not interrupt
to our expectations but it did interrupt eventually.

# Program Specifics

### Queue and its Implementation

Intially, we tried (and failed) to create an infitite array where
dequeues would move the base pointer of the array up by one, and 
enqueues would extend the tail by one. Ultimately, we could not
make the data structure work because moving the pointer always 
ended up overwriting other malloced space and segfaulting us.

As a result, we switched over to a Linked List, where we enqueue at 
the back of the list and dequeue from the front. Our iterators and delete 
function use the typical while(current->next != NULL) to loop. We implemented
the iterator as well, but we never used it. The queue_length() function was
also not used, because we stored the queue in a struct with a size field that
we simply incremented and decremented as necessary.


### Yielding

To keep track of the thread information, we created a TCB struct that stored
the TID, the parent TID, pointers to the stack and context, a joined flag, 
a return value, and a state. Our states were defined at the top using #define
for 0 == RUNNING, 1 == READY, 2 == BLOCKED, and 3 == ZOMBIE.

We used a global TCB, curr_TCB, to store the currently executing thread, and
we made two queues that were also global: queue, which stored all ready and 
blocked threads, and zombies, which stored zombie threads. The running thread
was not stored in a queue, but it was enqueued when the context was switched.

Calling uthread_create() for the first time calls our helper function which
checks if the global queues were initialized. If they were not, it meant that
it was the first created thread, and so main was saved and initialized 
globally, and the first thread was created, initialized, and enqueued.

Our uthread_yield() function saved the current context and loaded the new one.
It set the outgoing thread state to READY if it wasn't BLOCKED, and loaded
the next thread, saving the current execution context via a TCB* temp pointer
that dereferenced back to the previous context.

uthread_exit() was implicitly called in the background whenever a function
(thread) returned, and the return value was passed as an arg. uthread_exit()
set the state of that thread to ZOMBIE and instead of putting it in the
queue, it put in the zombies queue. This thread then called uthread_yield()
again to get to the next thread of execution.

### Joining

uthread_join() worked by receiving a TID and a retval (int pointer). This
function would set the current state to BLOCKED and then find the arg TID
in the queue and set it's joined flag to 1, so that other threads could not
try to join a thread that was already being joined.

After doing this, uthread_join() would simply yield again to the next ready 
thread, and would not skip to the thread that just had its joined flag set.
Once a joined thread returned, the joining thread would reap its return value
and store it. If a thread that was being joined was already a ZOMBIE, then
this happened instantly. If the thread to be joined was still running,
threads would continue yielding and the value of the joined thread would only
be reaped when it was done AND was next in line to dequeue.

### Preempt

Preempt sets a timer to go off 100 times a second, where we tell the 
signal handler of the kernel when to ignore and when to acknowledge
the virtual alarm signal.




