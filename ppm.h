#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/errno.h>
#include <iostream>
#include <semaphore.h>
#include "threadpool.h"
#include "message.h"
#include "structs.h"
#include "constants.h"

using namespace std;

void ftp_deliver(void *arg);
void ftp_send(void* ftp_inp);
void eth_send(int hlp, Message *msg, info f);
void eth_deliver(void*);

class ppm {
    public:
    ppm(int send, int recv, size_t poolsize, int message_count,sockaddr_in server,sockaddr_in other);
    void send(Message *msg, int proto_id);
    
    private:
    int tot_mes,send_sock,recv_sock;
    ThreadPool *pool;
    sem_t *sendsem;
    struct sockaddr_in server,other;
};

ppm::ppm(int send, int recv, size_t poolsize, int message_count,sockaddr_in svr,sockaddr_in oth) {
    send_sock = send;
    recv_sock = recv;
    server = svr;
    other = oth;
    sendsem = new sem_t();
    sem_init(sendsem, 0, 1);
    tot_mes = message_count;
    pool = new ThreadPool(poolsize);
    info *inf = new info();
    inf->ssocket = send_sock;
    inf->rsocket = recv_sock;
    inf->sendsem = sendsem;
    inf->other = other;
    inf->server = server;
    inf->tot = tot_mes;
    inf->pool=pool;
    if(pool->thread_avail()) {
        pool->dispatch_thread(eth_deliver, (void*)inf);
    }
}
void ppm::send(Message *msg, int proto_id) {
    send_input *sen= new send_input();
    sen->protocol_id = TCP_ID;
    sen->msg = msg;
    info inf;
    inf.ssocket = send_sock;
    inf.rsocket = recv_sock;
    inf.sendsem = sendsem;
    inf.other = other;
    inf.server = server;
    inf.tot=0;
    inf.pool=pool;
    sen->f = inf;
    //printf("Making thread for message\n");
    if(pool->thread_avail()) {
        /*
        char buf [sen->msg->msgLen()];
        sen->msg->msgFlat(buf);
        printf("App Sending-%s\n",buf);
        */
        pool->dispatch_thread(ftp_send,(void *)sen);
    }
}
void ftp_deliver(void *arg) {
    Message *msg = (Message *) arg; 
    char buf [msg->msgLen()+1];
    msg->msgFlat(buf);
    buf [msg->msgLen()] = '\0';
    printf("Recieved: %s\n",buf);
}
void eth_deliver(void *arg) {
    //printf("Eth ready to deliver\n");
    info *f = (struct info*) arg;
    int cur = 0,n=-1;
    unsigned int len = sizeof(struct sockaddr_in);
    char *buf;
    while (cur < f->tot) {
        buf = new char [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        if ((n = recvfrom(f->rsocket, buf, BUF_SIZE, 0, (struct sockaddr *) &(f->other), &len)) != -1) {
            //printf("%s\n",buf);
            Message *msg = new Message(buf,n);
            if(f->pool->thread_avail()) {
                f->pool->dispatch_thread(ftp_deliver,(void *)msg);
            }
        }
        else {
            printf("Failed to recv message");
            exit(0);
        }
        cur++;
    }
}
void ftp_send(void* ftp_inp) {
    send_input *sen = (send_input*) ftp_inp;
    /*
    char buf [sen->msg->msgLen()];
    sen->msg->msgFlat(buf);
    printf("IP Sending-%s\n",buf); */
    /*
    ftp_hdr *hdr;
    hdr->hlp = sen->protocol_id;
    strcpy(hdr->other_info,"");
    hdr->datalength = sen->msg->msgLen();
    sen->msg->msgAddHdr((char*)hdr,20);
    */
    eth_send(FTP_ID, sen->msg,sen->f);
}

void eth_send(int hlp, Message *msg, info f) {
    /*eth_hdr *et;
    et->hlp = hlp;
    strcpy(et->other_info,"");
    et->datalength = msg->msgLen();
    msg->msgAddHdr((char*)hdr,16);*/
    char buf [msg->msgLen()];
    msg->msgFlat(buf);
    //printf("Eth Sending-%s\n",buf);
    sem_wait(f.sendsem);
    int n = sendto(f.ssocket, buf, msg->msgLen(), 0, (struct sockaddr *) &(f.server), sizeof(struct sockaddr_in));
    if (n == -1) {
        //errexit("recv sock handshake failed from second\n");
        printf("Error in send\n");
    }
    sem_post(f.sendsem);
}

/*------------------------------------------------------------------------
 * errexit - print an error message and exit
 * IMPORTANT - This method has not been implemented by me.
 * This method has been taken from echoClient.c that was given as a sample
 * to students studying "Network Systems" course taught by Prof. S. Mishra
 * at University of Colorado Boulder Fall 2014
 *------------------------------------------------------------------------
 */
int errexit(const char *format, ...)
{
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        exit(1);
}
