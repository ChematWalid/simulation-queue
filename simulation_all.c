#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "sorted_list.h"
#include <math.h>
#include <string.h>
#include <time.h>

double expRandom(double mean) {
    double u = (double)rand() / ((double)RAND_MAX + 1.0);
    return -mean * log(1.0 - u);
}

typedef enum DistributionType {
    DETERMINISTIC,
    EXPONENTIAL
} DistributionType;

typedef struct Kendall{
    DistributionType arrivalDistribution;
    DistributionType serviceDistribution;
} Kendall;

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

void Arrival(double arrivalTime, double serviceTime, Kendall kendall) {
    Client c;
    c.id = nextId++;
    c.ArrivalTime = Tnow;
    c.StartServiceTime = 0.0;
    c.ServiceTime = (kendall.serviceDistribution == EXPONENTIAL) ? expRandom(serviceTime) : serviceTime;
    pushQueue(&waitingQueue, c);

    if (server == IDLE) {
        server = BUSY;
        createEvent(START_SERVICE, Tnow);
    }
    double nextArrivalTime = (kendall.arrivalDistribution == EXPONENTIAL) ? expRandom(arrivalTime) : arrivalTime;
    createEvent(ARRIVAL, Tnow + nextArrivalTime);
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

void simulate(double arrivalTime, double serviceTime, long maxClients , Kendall kendall) {
    srand(time(NULL));
    initialisation();

    while (clientServed < maxClients) {
        Event e = getEvent();
        Tnow = e.time;

        switch (e.type) {
            case ARRIVAL:
                Arrival(arrivalTime, serviceTime ,kendall);
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
    Kendall kendall = {DETERMINISTIC, DETERMINISTIC};
    if (argc > 1) {
        arrivalTime = atof(argv[1]);
    }
    if (argc > 2) {
        serviceTime = atof(argv[2]);
    }
    if (argc > 3) {
        maxClients = atol(argv[3]);
    }
    if (argc > 4) {
        if(strcmp(argv[4], "D") == 0) {
            kendall.arrivalDistribution = DETERMINISTIC;
        } else if(strcmp(argv[4], "M") == 0) {
            kendall.arrivalDistribution = EXPONENTIAL;
        } else {
            printf("Invalid arrival distribution type. Use 'D' for Deterministic or 'M' for Exponential.\n");
            return 1;
        }
    }
    if (argc > 5) {
        if(strcmp(argv[5], "D") == 0) {
            kendall.serviceDistribution = DETERMINISTIC;
        } else if(strcmp(argv[5], "M") == 0) {
            kendall.serviceDistribution = EXPONENTIAL;
        } else {
            printf("Invalid service distribution type. Use 'D' for Deterministic or 'M' for Exponential.\n");
            return 1;
        }
    }

    simulate(arrivalTime, serviceTime, maxClients, kendall);

    return 0;
}
