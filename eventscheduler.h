#include<threadpool.h>

using namespace std;

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
}

EventScheduler::EventScheduler() {
    //default constructor
    EventScheduler(10);
}

EventScheduler::EventScheduler(size_t maxEvents) {
    // contructor
    max = maxEvents;
    threadpool = new ThreadPool(maxEvents); 
}

EventScheduler::~EventScheduler() {
    // destructor
    delete threadpool;
}

int EventScheduler::eventSchedule(void evFunction(void*), void *arg, int timeout) {
}
