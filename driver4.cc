#include<unistd.h>
#include "application.h"

int main() {
    char host[10] = "localhost";
    char port1[10] = "5000";
    char port2[10] = "5001";
    application a1(true,400,host,port1, port2,true);
    return 1;
}
