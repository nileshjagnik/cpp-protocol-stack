#include<vector>
#include<pthread.h>
#include<semaphore.h>
#include<cstdlib>

using namespace std;

struct wrap_args {
    bool *flag;
    void (*start)(void *);
    void *argument;
    sem_t *sem;
};

void* wrapper_function(void* arg) {   
    wrap_args *wa = (struct wrap_args*) arg;
    wa->start(wa->argument);
    sem_wait(wa->sem);
    *(wa->flag) = true;
    sem_post(wa->sem);
}


struct slave_args {
    bool *flag;
    pthread_t *p;
};


void* slave_function(void* arg) {
    slave_args *sa = (struct slave_args*) arg;
    //cout<<"\n Waiting to join with "<<*(sa->p)<<endl;
    (void) pthread_join(*(sa->p), NULL);
    *(sa->flag) = true;
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
    vector<sem_t*> semaphores;
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
    semaphores.resize((int)tc);
    for(int i=0;i<(int)tc;i++) {
        pool[i] = (pthread_t *) malloc(sizeof(pthread_t));
        status_flags[i]=new bool(true);
        semaphores[i] = new sem_t();
        sem_init(semaphores[i], 0, 1);
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
        bool iflag;
        sem_wait(semaphores[i]);
        iflag = *status_flags[i];
        sem_post(semaphores[i]);
        if (iflag==true) {
            
            sem_wait(semaphores[i]);
            *status_flags[i] = false;
            sem_post(semaphores[i]);
        
            wrap_args *wa = new wrap_args();
            wa->flag = status_flags[i];
            wa->start = dispatch_function;
            wa->argument = arg;
            wa->sem = semaphores[i];
            int perror = pthread_create(pool[i], NULL, wrapper_function, (void *) wa);
            //int perror = pthread_create(pool[i], NULL, (void* (*) (void*)) dispatch_function, arg);
            if (perror != 0) {
                cout<<"Pthread creation failed with error:"<<perror<<endl;
                sem_wait(semaphores[i]);
                *status_flags[i] = true;
                sem_post(semaphores[i]);
            }
            /*
            *status_flags[i] = false;
            slave_args *sa = new slave_args();
            sa->flag = status_flags[i];
            sa-> p = pool[i];
            
            
            perror = pthread_create((pthread_t *) malloc(sizeof(pthread_t)), NULL, slave_function, (void*) sa);
            if (perror != 0) {
                cout<<"Slave pthread creation failed with error:"<<perror<<endl;
            }*/
            break;
        }
    }
    return 0;
}

bool ThreadPool::thread_avail(){
    for(int i=0;i<(int)threadCount;i++) {
        bool iflag;
        sem_wait(semaphores[i]);
        iflag = *status_flags[i];
        sem_post(semaphores[i]);
        if (iflag==true) {
            return true;
        }
    }
    return false;
}
