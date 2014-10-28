#include <vector>
using namespace std;
struct wrap_args {
    bool* flag;
    void (*start)(void *);
    void* argument;
};

void* wrapper_function(void* arg) {   
    wrap_args *wa = (struct wrap_args*) arg;
    wa->flag = new bool(false);
    wa->start(wa->argument);
    wa->flag = new bool(true);
}
           
class ThreadPool
{
public:
    ThreadPool();
    ThreadPool(size_t tc);
    ~ThreadPool();
    int dispatch_thread(void dispatch_function(void*), void *arg);
    bool thread_avail();
    
private:
    size_t threadCount;
    vector<pthread_t*> pool;
    vector<bool*> status_flags;
};

ThreadPool::ThreadPool() {
    // default constructor
    ThreadPool(10);
}
ThreadPool::ThreadPool(size_t tc) {
    // Initialize threadpool with size tc 
    threadCount = tc;
    pool.resize((int)tc);
    status_flags.resize((int)tc);
    for(int i=0;i<(int)tc;i++) {
        pool[i] = (pthread_t *) malloc(sizeof(pthread_t));
        status_flags[i]=new bool(true);
    }
}

ThreadPool::~ThreadPool( )
{
    // threadpool destructor
    pool.clear();
    status_flags.clear();
}

int ThreadPool::dispatch_thread(void dispatch_function(void*), void *arg){
    for(int i=0;i<(int)threadCount;i++) {
        if (*status_flags[i]==true) {
            wrap_args *wa = new wrap_args();
            wa->flag = status_flags[i];
            wa->start = dispatch_function;
            wa->argument = arg;
            int perror = pthread_create(pool[i], NULL, wrapper_function, (void *) wa);
            if (perror != 0) {
                cout<<"Pthread creation failed with error:"<<perror<<endl;
            }
            break;
        }
    }
    return 0;
}

bool ThreadPool::thread_avail(){
    for(int i=0;i<(int)threadCount;i++) {
        if (*status_flags[i]==true) {
            return true;
        }
    }
    return false;
}
