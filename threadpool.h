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
};

ThreadPool::ThreadPool() {
    ThreadPool(10);
}
ThreadPool::ThreadPool(size_t tc){
    threadCount = tc;
    pool.resize((int)tc);
    for(int i=0;i<(int)tc;i++) {
        pool[i] = (pthread_t *) malloc(sizeof(pthread_t));
    }
    //TODO - Maybe there is more to do?
}

ThreadPool::~ThreadPool( )
{
    pool.clear();
}

int ThreadPool::dispatch_thread(void dispatch_function(void*), void *arg){
    //TODO - dispatch    
    return 0;
}

bool ThreadPool::thread_avail(){
    //TODO - return available
    return true;
}
