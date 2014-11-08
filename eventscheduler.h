#include "threadpool.h"

#include<vector>
#include<cstdlib>

using namespace std;

struct event_args {
    void (*start)(void *);
    void *argument;
    int timeout;
    vector<int> * q;
    int id;
};

void eventWrapper(void *arg) {
    event_args *ea = (struct event_args*) arg;
    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = ea->timeout;
    int error;
    if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
        cout<<"Error in select, returned :"<<error<<endl;
    }
    bool cancel = false;
    int len = (int)ea->q->size();
    for (int i=0;i<len;i++) {
        if (ea->q->at(i) == (ea->id)) {
            cancel = true;
            ea->q->erase(ea->q->begin()+i);
            break;    
        }
    }
    if (cancel == false) {
        ea->start(ea->argument);
    }
}
 
class EventScheduler {
    public:
    EventScheduler();
    EventScheduler(size_t maxEvents);
    int eventSchedule(void evFunction(void*), void *arg, int timeout);
    ~EventScheduler();
    void eventCancel(int eventId);
    
    private:
    size_t max;
    ThreadPool *threadpool=NULL;
    int eventcounter;
    vector<int> *queue;
};

EventScheduler::EventScheduler() {
    //default constructor
    EventScheduler(10);
}

EventScheduler::EventScheduler(size_t maxEvents) {
    // contructor
    max = maxEvents;
    threadpool = new ThreadPool(maxEvents+1);
    eventcounter = 0;
    queue = new vector<int>();
}

EventScheduler::~EventScheduler() {
    // destructor
    delete threadpool;
    delete queue;
}

int EventScheduler::eventSchedule(void evFunction(void*), void *arg, int timeout) {
    if(threadpool->thread_avail()) {
        eventcounter++;
        event_args *ea = new event_args();
        ea->start = evFunction;
        ea->argument = arg;
        ea->timeout = timeout;
        ea->q = queue;
        ea->id = eventcounter;
        threadpool->dispatch_thread(eventWrapper,(void*) ea);
        return eventcounter;
    }
    return -1;
}
void EventScheduler::eventCancel(int eventId) {
    if (eventId <= 0) {
        cout<<"Invalid eventId :"<<eventId<<endl;
    }
    else {
        queue->push_back(eventId);
    }
}
