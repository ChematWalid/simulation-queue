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
long   clientServed = 0;

void createEvent(EventType type, double time) {
    eventList.insert({type, time});
}

Event getEvent() {
    Event e = *eventList.begin();
    eventList.erase(eventList.begin());
    return e;
}


int main() {
    createEvent(DEPARTURE, 5);
    createEvent(ARRIVAL, 0);
    createEvent(ARRIVAL, 6);
    while (!eventList.empty()) {
        Event e = getEvent();
        std::cout << e.type << " at " << e.time << "\n";
    }
}