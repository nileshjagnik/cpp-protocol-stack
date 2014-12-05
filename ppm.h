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

void ftp_send(void* ftp_inp);
void dns_send(void* dns_inp);
void rdp_send(void* rdp_inp);
void telnet_send(void* telnet_inp);
void tcp_send(send_input* sen);
void udp_send(send_input* sen);
void ip_send(send_input* sen);
void eth_send(send_input* sen);
void eth_deliver(void*);
void tcp_deliver(Message*);
void ftp_deliver(Message*);
void telnet_deliver(Message*);
void udp_deliver(Message*);
void rdp_deliver(Message*);
void dns_deliver(Message*);

int FTP_count=0;
int Telnet_count=0;
int RDP_count=0;
int DNS_count=0;

struct timeval start, end;
long mtime, seconds, useconds;
	
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
    gettimeofday(&start,NULL);
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
    sen->protocol_id = proto_id;
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
    if(proto_id == FTP_ID) {
        if(pool->thread_avail()) {
            /*
            char buf [sen->msg->msgLen()];
            sen->msg->msgFlat(buf);
            printf("App Sending-%s\n",buf);
            */
            pool->dispatch_thread(ftp_send,(void *)sen);
        }
    }
    else if(proto_id == TELNET_ID) {
        if(pool->thread_avail()) {
            /*
            char buf [sen->msg->msgLen()];
            sen->msg->msgFlat(buf);
            printf("App Sending-%s\n",buf);
            */
            pool->dispatch_thread(telnet_send,(void *)sen);
        }
    }
    else if(proto_id == RDP_ID) {
        if(pool->thread_avail()) {
            /*
            char buf [sen->msg->msgLen()];
            sen->msg->msgFlat(buf);
            printf("App Sending-%s\n",buf);
            */
            pool->dispatch_thread(rdp_send,(void *)sen);
        }
    }
    else if(proto_id == DNS_ID) {
        if(pool->thread_avail()) {
            /*
            char buf [sen->msg->msgLen()];
            sen->msg->msgFlat(buf);
            printf("App Sending-%s\n",buf);
            */
            pool->dispatch_thread(dns_send,(void *)sen);
        }
    }
    
}
void ftp_deliver(Message *msg) {
    ftp_hdr *hdr = (ftp_hdr*) msg->msgStripHdr(16); 
    /*char buf [msg->msgLen()+1];
    msg->msgFlat(buf);
    buf [msg->msgLen()] = '\0';*/
    FTP_count++;
    //printf("FTP Received: %s\n",buf);
    if((FTP_count==TOT_SEND) && (Telnet_count==TOT_SEND)&&(RDP_count==TOT_SEND) &&(DNS_count==TOT_SEND)) {
        gettimeofday(&end,NULL);
        useconds = end.tv_usec - start.tv_usec;
	    seconds = end.tv_sec - start.tv_sec;
	    if(useconds<0) {
	        seconds--;
	        useconds = 1000000 - useconds;
	    }
	    cout << "elapsed time: "<< seconds << " seconds " << useconds << " microseconds\n" << endl;
    }
}

void telnet_deliver(Message *msg) {
    telnet_hdr *hdr = (telnet_hdr*) msg->msgStripHdr(16); 
    /*char buf [msg->msgLen()+1];
    msg->msgFlat(buf);
    buf [msg->msgLen()] = '\0';*/
    Telnet_count++;
    //printf("TELNET Received: %s\n",buf);
    if((FTP_count==TOT_SEND) && (Telnet_count==TOT_SEND)&&(RDP_count==TOT_SEND) &&(DNS_count==TOT_SEND)) {
        gettimeofday(&end,NULL);
        useconds = end.tv_usec - start.tv_usec;
	    seconds = end.tv_sec - start.tv_sec;
	    if(useconds<0) {
	        seconds--;
	        useconds = 1000000 - useconds;
	    }
	    cout << "elapsed time: "<< seconds << " seconds " << useconds << " microseconds\n" << endl;
    }
}

void tcp_deliver(Message *msg) {
    tcp_hdr *hdr = (tcp_hdr*) msg->msgStripHdr(12);
    //printf("%d\n",hdr->hlp);
    if(hdr->hlp == FTP_ID) {
        ftp_deliver(msg);
    }
    else if(hdr->hlp == TELNET_ID) {
        telnet_deliver(msg);
    }
}

void rdp_deliver(Message *msg) {
    rdp_hdr *hdr = (rdp_hdr*) msg->msgStripHdr(20); 
    /*char buf [msg->msgLen()+1];
    msg->msgFlat(buf);
    buf [msg->msgLen()] = '\0';*/
    RDP_count++;
    //printf("RDP Received: %s\n",buf);
    if((FTP_count==TOT_SEND) && (Telnet_count==TOT_SEND)&&(RDP_count==TOT_SEND) &&(DNS_count==TOT_SEND)) {
        gettimeofday(&end,NULL);
        useconds = end.tv_usec - start.tv_usec;
	    seconds = end.tv_sec - start.tv_sec;
	    if(useconds<0) {
	        seconds--;
	        useconds = 1000000 - useconds;
	    }
	    cout << "elapsed time: "<< seconds << " seconds " << useconds << " microseconds\n" << endl;
    }
}

void dns_deliver(Message *msg) {
    dns_hdr *hdr = (dns_hdr*) msg->msgStripHdr(16); 
    /*char buf [msg->msgLen()+1];
    msg->msgFlat(buf);
    buf [msg->msgLen()] = '\0';*/
    DNS_count++;
    //printf("DNS     Received: %s\n",buf);
    if((FTP_count==TOT_SEND) && (Telnet_count==TOT_SEND)&&(RDP_count==TOT_SEND) &&(DNS_count==TOT_SEND)) {
        gettimeofday(&end,NULL);
        useconds = end.tv_usec - start.tv_usec;
	    seconds = end.tv_sec - start.tv_sec;
	    if(useconds<0) {
	        seconds--;
	        useconds = 1000000 - useconds;
	    }
	    cout << "elapsed time: "<< seconds << " seconds " << useconds << " microseconds\n" << endl;
    }
}

void udp_deliver(Message *msg) {
    udp_hdr *hdr = (udp_hdr*) msg->msgStripHdr(12);
    //printf("%d\n",hdr->hlp);
    if(hdr->hlp == RDP_ID) {
        rdp_deliver(msg);
    }
    else if(hdr->hlp == DNS_ID) {
        dns_deliver(msg);
    }
}

void ip_deliver(void *arg) {
    Message *msg = (Message*) arg;
    ip_hdr *hdr = (ip_hdr*) msg->msgStripHdr(20);
    if(hdr->hlp == TCP_ID) {
        tcp_deliver(msg);
    }
    else if(hdr->hlp == UDP_ID) {
        udp_deliver(msg);
    }
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
            //printf("%d\n",n);
            Message *msg = new Message(buf,n);
            eth_hdr *eth = (eth_hdr*) msg->msgStripHdr(16);
            //printf("%d\n",eth->hlp);
            if(f->pool->thread_avail()) {
                f->pool->dispatch_thread(ip_deliver,(void *)msg);
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
    /*char buf [BUF_SIZE];
    sen->msg->msgFlat(buf);
    //printf("FTP Sending-%s\n",buf);*/
    ftp_hdr *hdr=new ftp_hdr();
    hdr->hlp = sen->protocol_id;
    strcpy(hdr->other_info,"");
    hdr->datalength = sen->msg->msgLen();
    sen->msg->msgAddHdr((char*)hdr,16);
    sen->protocol_id=FTP_ID;
    tcp_send(sen);
}

void telnet_send(void* telnet_inp) {
    send_input *sen = (send_input*) telnet_inp;
    /*char buf [sen->msg->msgLen()];
    sen->msg->msgFlat(buf);
    //printf("Telnet Sending-%s\n",buf); */
    telnet_hdr *hdr=new telnet_hdr();
    hdr->hlp = sen->protocol_id;
    strcpy(hdr->other_info,"");
    hdr->datalength = sen->msg->msgLen();
    sen->msg->msgAddHdr((char*)hdr,16);
    sen->protocol_id=TELNET_ID;
    tcp_send(sen);
}

void rdp_send(void* rdp_inp) {
    send_input *sen = (send_input*) rdp_inp;
    /*char buf [sen->msg->msgLen()];
    sen->msg->msgFlat(buf);
    //printf("RDP Sending-%s\n",buf);*/ 
    rdp_hdr *hdr = new rdp_hdr();
    hdr->hlp = sen->protocol_id;
    strcpy(hdr->other_info,"");
    hdr->datalength = sen->msg->msgLen();
    sen->msg->msgAddHdr((char*)hdr,20);
    sen->protocol_id=RDP_ID;
    udp_send(sen);
}

void dns_send(void* dns_inp) {
    send_input *sen = (send_input*) dns_inp;
    /*char buf [sen->msg->msgLen()];
    sen->msg->msgFlat(buf);
    //printf("DNS Sending-%s\n",buf); */
    dns_hdr *hdr = new dns_hdr();
    hdr->hlp = sen->protocol_id;
    strcpy(hdr->other_info,"");
    hdr->datalength = sen->msg->msgLen();
    sen->msg->msgAddHdr((char*)hdr,16);
    sen->protocol_id=DNS_ID;
    udp_send(sen);
}

void tcp_send(send_input* sen) {
   /* char buf [sen->msg->msgLen()];
    sen->msg->msgFlat(buf);
    //printf("TCP Sending-%s\n",buf); */
    tcp_hdr *hdr= new tcp_hdr();
    hdr->hlp = sen->protocol_id;
    strcpy(hdr->other_info,"");
    hdr->datalength = sen->msg->msgLen();
    sen->msg->msgAddHdr((char*)hdr,12);
    
    sen->protocol_id=TCP_ID;
    ip_send(sen);
}

void udp_send(send_input* sen) {
    
    /*char buf [sen->msg->msgLen()];
    sen->msg->msgFlat(buf);
    //printf("UDP Sending-%s\n",buf); */
    udp_hdr *hdr = new udp_hdr();
    hdr->hlp = sen->protocol_id;
    strcpy(hdr->other_info,"");
    hdr->datalength = sen->msg->msgLen();
    sen->msg->msgAddHdr((char*)hdr,12);
    sen->protocol_id=UDP_ID;
    ip_send(sen);
}

void ip_send(send_input* sen) {
    
    /*char buf [sen->msg->msgLen()];
    sen->msg->msgFlat(buf);
    //printf("IP Sending-%s\n",buf); */
    ip_hdr *hdr=new ip_hdr();
    hdr->hlp = sen->protocol_id;
    strcpy(hdr->other_info,"");
    hdr->datalength = sen->msg->msgLen();
    sen->msg->msgAddHdr((char*)hdr,20);
    sen->protocol_id=IP_ID;
    eth_send(sen);
}
void eth_send(send_input* sen) {
    eth_hdr *et=new eth_hdr();
    et->hlp = sen->protocol_id;
    strcpy(et->other_info,"");
    et->datalength = sen->msg->msgLen();
    sen->msg->msgAddHdr((char*)et,16);
    char buf [sen->msg->msgLen()];
    sen->msg->msgFlat(buf);
    //printf("Eth Sending-%s\n",buf);
    sem_wait(sen->f.sendsem);
    int n = sendto(sen->f.ssocket, buf, sen->msg->msgLen(), 0, (struct sockaddr *) &(sen->f.server), sizeof(struct sockaddr_in));
    if (n == -1) {
        //errexit("recv sock handshake failed from second\n");
        printf("Error in send\n");
    }
    sem_post(sen->f.sendsem);
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
