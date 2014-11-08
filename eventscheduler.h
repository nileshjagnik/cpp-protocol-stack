#include "threadpool.h"

using namespace std;

struct event_args {
    void (*start)(void *);
    void *argument;
    int timeout;
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
    ea->start(ea->argument);
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
};

EventScheduler::EventScheduler() {
    //default constructor
    EventScheduler(10);
}

EventScheduler::EventScheduler(size_t maxEvents) {
    // contructor
    max = maxEvents;
    threadpool = new ThreadPool(maxEvents+1); 
}

EventScheduler::~EventScheduler() {
    // destructor
    delete threadpool;
}

int EventScheduler::eventSchedule(void evFunction(void*), void *arg, int timeout) {
    if(threadpool->thread_avail()) {
        event_args *ea = new event_args();
        ea->start = evFunction;
        ea->argument = arg;
        ea->timeout = timeout;
        threadpool->dispatch_thread(eventWrapper,(void*) ea);
        return 1;
    }
    return 0;
}
void EventScheduler::eventCancel(int eventId) {
}
