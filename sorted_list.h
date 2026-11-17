#ifndef SORTED_LIST_H
#define SORTED_LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef enum EventType {
    ARRIVAL,
    START_SERVICE,
    DEPARTURE
} EventType;

typedef struct Event {
    EventType type;
    double time;
} Event;

typedef struct EventNode {
    Event data;
    struct EventNode* next;
} EventNode;

typedef struct SortedList {
    EventNode* head;
    int size;
} SortedList;

void initSortedList(SortedList* list) {
    list->head = NULL;
    list->size = 0;
}

int isSortedListEmpty(SortedList* list) {
    return list->head == NULL;
}

void insertInPlace(SortedList* list, Event event) {
    EventNode* newNode = (EventNode*)malloc(sizeof(EventNode));
    if (!newNode) {
        printf("Error: Memory allocation failed in insertInPlace\n");
        exit(1);
    }
    newNode->data = event;
    newNode->next = NULL;

    if (list->head == NULL || event.time < list->head->data.time) {
        newNode->next = list->head;
        list->head = newNode;
    } else {
        EventNode* curr = list->head;
        while (curr->next != NULL && curr->next->data.time <= event.time) {
            curr = curr->next;
        }
        newNode->next = curr->next;
        curr->next = newNode;
    }
    list->size++;
}

Event popSortedList(SortedList* list) {
    if (isSortedListEmpty(list)) {
        printf("Error: popSortedList called on empty list\n");
        Event empty = {ARRIVAL, 0.0};
        return empty;
    }
    EventNode* temp = list->head;
    Event data = temp->data;

    list->head = list->head->next;
    free(temp);
    list->size--;
    return data;
}

void clearSortedList(SortedList* list) {
    EventNode* curr = list->head;
    while (curr != NULL) {
        EventNode* temp = curr;
        curr = curr->next;
        free(temp);
    }
    list->head = NULL;
    list->size = 0;
}

#endif
