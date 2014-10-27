using namespace std;

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
    vector<*bool> status_flags;
};

ThreadPool::ThreadPool() {
    // default constructor
    ThreadPool(10);
}
ThreadPool::ThreadPool(size_t tc) {
    // Initialize threadpool with size tc 
    threadCount = tc;
    pool.resize((int)tc);
    status_flags((int)tc);
    for(int i=0;i<(int)tc;i++) {
        pool[i] = (pthread_t *) malloc(sizeof(pthread_t));
        *status_flags[i]=true;
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
        if (*status_flag[i]==true) {
            
        }
    }
    return 0;
}

bool ThreadPool::thread_avail(){
    for(int i=0;i<(int)threadCount;i++) {
        if (*status_flag[i]==true) {
            return true;
        }
    }
    return false;
}
