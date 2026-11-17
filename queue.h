#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Client {
    int id;
    double ArrivalTime;
    double StartServiceTime;
    double ServiceTime;
} Client;

typedef struct QueueNode {
    Client data;
    struct QueueNode* next;
} QueueNode;

typedef struct Queue {
    QueueNode* front;
    QueueNode* rear;
    int size;
} Queue;

void initQueue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
    q->size = 0;
}

int isQueueEmpty(Queue* q) {
    return q->front == NULL;
}

void pushQueue(Queue* q, Client c) {
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (!newNode) {
        printf("Error: Memory allocation failed in pushQueue\n");
        exit(1);
    }
    newNode->data = c;
    newNode->next = NULL;

    if (q->rear == NULL) {
        q->front = newNode;
        q->rear = newNode;
    } else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
    q->size++;
}

Client frontQueue(Queue* q) {
    if (isQueueEmpty(q)) {
        printf("Error: frontQueue called on empty queue\n");
        Client empty = {0, 0, 0, 0};
        return empty;
    }
    return q->front->data;
}

Client popQueue(Queue* q) {
    if (isQueueEmpty(q)) {
        printf("Error: popQueue called on empty queue\n");
        Client empty = {0, 0, 0, 0};
        return empty;
    }
    QueueNode* temp = q->front;
    Client data = temp->data;

    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    q->size--;
    return data;
}

void clearQueue(Queue* q) {
    while (!isQueueEmpty(q)) {
        popQueue(q);
    }
}

#endif
