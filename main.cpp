#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string.h>
#include <vector>
#include <utility>
#include "threadpool.h"
#include "message.h"

using namespace std;

int main(){
    ThreadPool *t = new ThreadPool(10);
    //if (t->thread_avail()) {
        //int number=t.dispatch_thread(,,);    
    //}
    
    delete t;
    
    int *p = new int(4);
    cout<<*p<<endl;
}
