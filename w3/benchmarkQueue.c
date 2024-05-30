#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Queue.h"

int main(void) {
	Queue q = QueueNew();
	for(int i = 0; i < 50000; i+=2) {
		QueueEnqueue(q, i);
		QueueEnqueue(q, i+1);
		//QueueDequeue(q);
	}
	for(int i = 0; i < 30000; i+=2) {
		QueueDequeue(q);
	}
	QueueFree(q);
}

/*
Paste the results of the time commands here:

ARRAY QUEUE
Real: 0.59
User: 0.17
Sys: 0.07

CIRCULAR ARRAY QUEUE
Real: 0.50
User: 0.09
Sys: 0.06

CIRCULAR ARRAY QUEUE IS FASTER! :)
*/

