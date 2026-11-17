#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "sorted_list.h"
#include <math.h>

double expRandom(double mean) {
    double u = (double)rand() / ((double)RAND_MAX + 1.0);
    return -mean * log(1.0 - u);
}

typedef enum ServerState {
    IDLE,
    BUSY
} ServerState;

double Tnow;
ServerState server;
Queue waitingQueue;
Client clientInService;
SortedList eventList;

int nextId = 1;
double sumResponse = 0.0;
double sumWaiting = 0.0;
long clientServed = 0;

void createEvent(EventType type, double time) {
    Event e = {type, time};
    insertInPlace(&eventList, e);
}

Event getEvent() {
    return popSortedList(&eventList);
}

void initialisation() {
    server = IDLE;
    initQueue(&waitingQueue);
    initSortedList(&eventList);
    Tnow = 0.0;

    sumResponse = 0.0;
    sumWaiting = 0.0;
    clientServed = 0;
    nextId = 1;

    createEvent(ARRIVAL, Tnow);
}

void Arrival(double arrivalTime, double serviceTime) {
    Client c;
    c.id = nextId++;
    c.ArrivalTime = Tnow;
    c.StartServiceTime = 0.0;
    c.ServiceTime = expRandom(serviceTime);
    pushQueue(&waitingQueue, c);

    if (server == IDLE) {
        server = BUSY;
        createEvent(START_SERVICE, Tnow);
    }

    createEvent(ARRIVAL, Tnow + expRandom(arrivalTime));
}

void StartService() {
    server = BUSY;

    clientInService = popQueue(&waitingQueue);
    clientInService.StartServiceTime = Tnow;

    createEvent(DEPARTURE, Tnow + clientInService.ServiceTime);
}

void Departure() {
    sumResponse += (Tnow - clientInService.ArrivalTime);
    sumWaiting  += (clientInService.StartServiceTime - clientInService.ArrivalTime);
    clientServed++;

    if (isQueueEmpty(&waitingQueue)) {
        server = IDLE;
    } else {
        createEvent(START_SERVICE, Tnow);
    }
}

void statistics() {
    printf("Clients served %ld\n", clientServed);
    printf("Mean response time %f\n", sumResponse / clientServed);
    printf("Mean waiting time %f\n", sumWaiting / clientServed);
    printf("Throughput %f\n", (double)clientServed / Tnow);
}

void simulate(double arrivalTime, double serviceTime, long maxClients) {
    srand(time(NULL));
    initialisation();

    while (clientServed < maxClients) {
        Event e = getEvent();
        Tnow = e.time;

        switch (e.type) {
            case ARRIVAL:
                Arrival(arrivalTime, serviceTime);
                break;

            case START_SERVICE:
                StartService();
                break;

            case DEPARTURE:
                Departure();
                break;
        }
    }

    statistics();

    clearQueue(&waitingQueue);
    clearSortedList(&eventList);
}

int main(int argc, char* argv[]) {
    double arrivalTime = 6.0;
    double serviceTime = 5.0;
    long maxClients = 10000000;

    if (argc > 1) {
        arrivalTime = atof(argv[1]);
    }
    if (argc > 2) {
        serviceTime = atof(argv[2]);
    }
    if (argc > 3) {
        maxClients = atol(argv[3]);
    }

    simulate(arrivalTime, serviceTime, maxClients);

    return 0;
}
