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

typedef struct Statistics {
    double meanArrivalTime;
    double meanServiceTime;
    long maxClients;
    Kendall kendall;
    long clientsServed;
    double meanResponseTime;
    double meanWaitingTime;
    double throughput;
} Statistics;

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


Statistics simulate(double arrivalTime, double serviceTime, long maxClients , Kendall kendall) {
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

    Statistics stats;
    stats.meanArrivalTime = arrivalTime;
    stats.meanServiceTime = serviceTime;
    stats.maxClients = maxClients;
    stats.kendall = kendall;
    stats.clientsServed = clientServed;
    stats.meanResponseTime = sumResponse / clientServed;
    stats.meanWaitingTime = sumWaiting / clientServed;
    stats.throughput = (double)clientServed / Tnow;

    clearQueue(&waitingQueue);
    clearSortedList(&eventList);

    return stats;
}

void writeStatsToCSV(const char *filename, Statistics stats) {
    FILE *check = fopen(filename, "r");
    int writeHeader = 0;
    if (check == NULL) {
        writeHeader = 1;
    } else {
        fseek(check, 0, SEEK_END);
        if (ftell(check) == 0) {
            writeHeader = 1;
        }
        fclose(check);
    }

    FILE *f = fopen(filename, "a");
    if (!f) {
        perror("Error opening output file");
        return;
    }

    if (writeHeader) {
        fprintf(f, "arrival_time,service_time,rho,arrival_dist,service_dist,clients_served,mean_response_time,mean_waiting_time,throughput\n");
    }

    double rho = stats.meanServiceTime / stats.meanArrivalTime;
    char arrDist = (stats.kendall.arrivalDistribution == EXPONENTIAL) ? 'M' : 'D';
    char servDist = (stats.kendall.serviceDistribution == EXPONENTIAL) ? 'M' : 'D';

    fprintf(f, "%.4f,%.4f,%.4f,%c,%c,%ld,%.6f,%.6f,%.6f\n",
            stats.meanArrivalTime,
            stats.meanServiceTime,
            rho,
            arrDist,
            servDist,
            stats.clientsServed,
            stats.meanResponseTime,
            stats.meanWaitingTime,
            stats.throughput);

    fclose(f);
}

int main(int argc, char* argv[]) {
    double arrivalTime = 6.0;
    double serviceTime = 5.0;
    long maxClients = 10000000;
    Kendall kendall = {DETERMINISTIC, DETERMINISTIC};
    const char *filename = "simulation_results.csv";

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
    if (argc > 6) {
        filename = argv[6];
    }

    Statistics stats = simulate(arrivalTime, serviceTime, maxClients, kendall);
    writeStatsToCSV(filename, stats);

    printf("Clients served %ld\n", stats.clientsServed);
    printf("Mean response time %f\n", stats.meanResponseTime);
    printf("Mean waiting time %f\n", stats.meanWaitingTime);
    printf("Throughput %f\n", stats.throughput);
    printf("Saved to %s\n", filename);

    return 0;
}
