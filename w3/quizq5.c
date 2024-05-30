#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef struct list *List;
struct list {
	int value;
	List next;
};

typedef struct queue *Queue;
struct queue {
	List head;
	List tail;
	int  size;
};

List newNode(int v) {
	List n = malloc(sizeof(*n));
	n->value = v;
	n->next = NULL;
	return n;
}

Queue QueueNew(void) {
	Queue q = malloc(sizeof(*q));
	if (q == NULL) {
		fprintf(stderr, "couldn't allocate Queue\n");
		exit(EXIT_FAILURE);
	}

	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
	return q;
}

Queue fn(List list) {
    Queue List2 = QueueNew(0);

    if (list == NULL) {
        return NULL;
    }
    if (list->next == NULL) {
        return list;
    }
    
    list->next->next = list;
    list->next = NULL;
    return List2;
}

void QueueEnqueue(Queue q, int it) {
	List n = newNode(it);
	if (q->size == 0) {
		q->head = n;
		q->tail = n;
	}
	else {
		q->tail->next = n;
		q->tail = n;
	}
	q->size++;
	return;
}

int main() {

    Function main():
    listA = linked list containing the integers 3, 7, 5, 3, 2 (in this order)
    listB = fn(listA)

    Queue q = QueueNew();
    QueueEnqueue(q, 1);
    QueueEnqueue(q, 3);
    QueueEnqueue(q, 1);
    QueueEnqueue(q, 2);
    QueueEnqueue(q, 3);
    QueueEnqueue(q, 6);
    QueueEnqueue(q, 7);
    QueueEnqueue(q, 4);
    QueueEnqueue(q, 9);
    QueueEnqueue(q, 5);
    //printf("%d", q->size);
    //printf("%d", q->head->value);
    //printf("%d", q->tail->value);
    fn(q->head, 0);
    return 0;
}