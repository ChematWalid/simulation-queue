#include <iostream>
#include <queue>
#include <set>

enum EventType   {
    ARRIVAL,
    START_SERVICE,
    DEPARTURE
};
enum ServerState {
    IDLE, 
    BUSY
};

struct Client {
    int id;
    double tArrival;
    double tStartService;
    double tServiceTime;
};

struct Event {
    EventType type;
    double time;

    //pour utiliser multiset (ordonner les événements par temps donc on redefenit <)
    bool operator<(const Event& o) const {
        return time < o.time; 
    }
};

double Tnow;
ServerState server;
std::queue<Client> waitingQueue;
Client clientInService;
std::multiset<Event> eventList;

const double ARRIVAL_TIME = 6;
const double SERVICE_TIME = 5;
int nextId = 1;

double sumResponse = 0, sumWaiting = 0;
long clientServed = 0;

const long MAX_CLIENTS = 10000000; 

void createEvent(EventType type, double time) {
    eventList.insert({type, time});
}

Event getEvent() {
    Event e = *eventList.begin();
    eventList.erase(eventList.begin());
    return e;
}

void initialisation() {
    server = IDLE;
    waitingQueue = std::queue<Client>();
    Tnow = 0;
    eventList.clear();

    sumResponse = 0;
    sumWaiting  = 0;
    clientServed = 0;
    nextId = 1;

    createEvent(ARRIVAL, Tnow);
}

void Arrival() {
    Client c{};
    c.id = nextId++;
    c.tArrival = Tnow;
    c.tServiceTime = SERVICE_TIME;
    waitingQueue.push(c);

    if (server == IDLE)
        createEvent(START_SERVICE, Tnow);

    createEvent(ARRIVAL, Tnow + ARRIVAL_TIME); //simulate next clinet
}

void StartService() {
    server = BUSY;

    clientInService = waitingQueue.front();
    waitingQueue.pop();
    clientInService.tStartService = Tnow;

    createEvent(DEPARTURE, Tnow + clientInService.tServiceTime);
}

void Departure() {
    server = IDLE;

    sumResponse += Tnow - clientInService.tArrival; // response time = departure time - arrival time
    sumWaiting  += clientInService.tStartService - clientInService.tArrival; // waiting time = start service time - arrival time
    clientServed++;

    if (!waitingQueue.empty())
        createEvent(START_SERVICE, Tnow); //start next client in queue
}

void statistics() {
    std::cout << "Clients served " << clientServed << "\n";
    std::cout << "Mean response time " << sumResponse / clientServed << "\n";
    std::cout << "Mean waiting time " << sumWaiting  / clientServed << "\n";
    std::cout << "Throughput " << clientServed / Tnow << "\n";
}

int main() {
    initialisation();
    while (clientServed < MAX_CLIENTS) {
        Event e = getEvent();
        Tnow = e.time;//avance le clock      

        switch (e.type) {
            case ARRIVAL:
                Arrival();      
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
    return 0;
}