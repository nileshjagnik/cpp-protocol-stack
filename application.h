#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/errno.h>
#include<cstdlib>

#define BUF_SIZE 2048
#include "ppm.h"
using namespace std;

extern int	errno;
int		errexit(const char *format, ...);
void ppp_poll_ftp(void *arg);
void ppp_poll_telnet(void *arg);
void ppp_poll_rdp(void *arg);
void ppp_poll_dns(void *arg);


class application{
    public:
    application(bool first, int total, char* host, char* other_portno, char* self_portno, bool flag);
    ~application();
    //void deliver();
    
    private:
    int total_msg;
    int recv_msg;
    int send_sock;
    int recv_sock;
    ppm *p;
};

application::~application() {
    close(recv_sock);
    close(send_sock);
}

application::application(bool first, int total, char* hostname, char* other_portno, char* self_portno, bool flag) {
    total_msg = total;
    /* buffer to store data for communication */
    char buf[BUF_SIZE];
    
    int port,n;
    unsigned int len;
    /* Struct to store address and port of server */
    struct sockaddr_in server;
    struct sockaddr_in self, other;
        
    if (first == true) {
        
        /* Length contains size of sockaddr_in in bytes */ 
        len = sizeof(struct sockaddr_in);
        
        /* struct to store hostname */
        struct hostent *host;
        
        /* fill hostent struct */
        host = gethostbyname(hostname);
        if (host == NULL) {
    	    errexit("hostname unresolved\n");
        }
        
        /* convert portnum taken from command line as srting to integer */  
        port = atoi(other_portno);
        
        /* initialize socket */
        if ((send_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	        errexit("send socket not initialized\n");
        }
        /* set all bytes in struct sockaddr_in to 0 */
        memset((char *) &server, 0, sizeof(struct sockaddr_in));
        
        /* initialize server addr */
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr = *((struct in_addr*) host->h_addr);
        
        bzero(buf,BUF_SIZE);
        strcpy(buf,"hello");
        
        if (sendto(send_sock,buf,strlen(buf)+1, 0, (struct sockaddr *) &server, len) == -1) {
            errexit("send sock trying to send but failed in first");
        }
        
        /* receive a reply */
        n = recvfrom(send_sock, buf, BUF_SIZE, 0, (struct sockaddr *) &server, &len);
        if (n==-1) {
            errexit("send_socket not made in first");
        }
        
        // Now the recieve socket part
        
        
        if ((recv_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	        errexit("recv socket not estabilished at first\n");
        }
        
        port = atoi(self_portno);
        
        memset((char *) &self, 0, sizeof(struct sockaddr_in));
        
        self.sin_family = AF_INET;
        self.sin_port = htons(port);
        self.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY receives packets from all interfaces
        
        if (bind(recv_sock, (struct sockaddr *) &self, sizeof(self)) == -1) {
	        errexit("recv sock bind fail at first\n");
        }
        
        if ((n = recvfrom(recv_sock, buf, BUF_SIZE, 0, (struct sockaddr *) &other, &len)) != -1) {
            bzero(buf,BUF_SIZE);
            strcpy(buf,"1");
            n = sendto(recv_sock, buf, strlen(buf)+1, 0, (struct sockaddr *) &other, len);
            if (n == -1) {
                errexit("recv sock handshake failed from first\n");
            }
        }
        else {
            errexit("recv sock falied to connect to other in first\n");
        }
        printf("Connection estabilished from first side\n");
    }
    else {
        // First the recieve socket part
        
        /* initialize socket */
        if ((recv_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	        errexit("recv socket not estabilished at second\n");
        }
        
        /* get port from string */
        port = atoi(self_portno);
        
        /* set all bytes to 0 */
        memset((char *) &self, 0, sizeof(struct sockaddr_in));
        
        len = sizeof(struct sockaddr_in);
        
        /* initialize server family, port and ip address for server */  
        self.sin_family = AF_INET;
        self.sin_port = htons(port);
        self.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY receives packets from all interfaces
        
        /* bind server to port */
        if (bind(recv_sock, (struct sockaddr *) &self, sizeof(self)) == -1) {
	        errexit("recv sock bind fail at second\n");
        }
        bzero(buf,BUF_SIZE);
        if ((n = recvfrom(recv_sock, buf, BUF_SIZE, 0, (struct sockaddr *) &other, &len)) != -1) {
            bzero(buf,BUF_SIZE);
            strcpy(buf,"1");
            n = sendto(recv_sock, buf, strlen(buf)+1, 0, (struct sockaddr *) &other, len);
            if (n == -1) {
                errexit("recv sock handshake failed from second\n");
            }
        }
        else {
            errexit("recv sock failed to connect to other in second\n");
        }
        
        // Now for the send part
        sleep(1);
        
        len = sizeof(struct sockaddr_in);
        
        struct hostent *host;
        
        host = gethostbyname(hostname);
        if (host == NULL) {
    	    errexit("hostname unresolved\n");
        }
        
        port = atoi(other_portno);
        
        if ((send_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	        errexit("send socket not initialized in second\n");
        }
        
        memset((char *) &server, 0, sizeof(struct sockaddr_in));
        
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr = *((struct in_addr*) host->h_addr);
        
        bzero(buf,BUF_SIZE);
        strcpy(buf,"hello");
        
        if (sendto(send_sock,buf,strlen(buf)+1, 0, (struct sockaddr *) &server, len) == -1) {
            errexit("send sock trying to send but falied in second");
        }
        
        n = recvfrom(send_sock, buf, BUF_SIZE, 0, (struct sockaddr *) &server, &len);
        if (n==-1) {
            errexit("send_socket made in second");
        }
        printf("Connection estabilished from second side\n"); 
    }
    if (flag == true) {
        p = new ppm(send_sock,recv_sock, 50,total,server,other);
        ThreadPool thp(4);
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppp_poll_ftp,(void *)p);
        }
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppp_poll_telnet,(void *)p);
        }
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppp_poll_rdp,(void *)p);
        }
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppp_poll_dns,(void *)p);
        }
        struct timeval time;
        time.tv_sec = 2;
        time.tv_usec = 1000;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}

void ppp_poll_ftp(void *arg) {
    ppm *p = (ppm *) arg; 
    for(int i=0;i<TOT_SEND;i++) {
        //printf("sending Message %d\n",i);
        char msg [3];
        int one = i/10;
        int two = i - one*10;
        msg[0]='0'+one;
        msg[1]='0'+two;
        msg[2] ='\0';
        Message *m = new Message(msg,2);
        p->send(m,FTP_ID);
        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = 1000;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}
void ppp_poll_telnet(void *arg) {
    ppm *p = (ppm *) arg; 
    for(int i=0;i<TOT_SEND;i++) {
        //printf("sending Message %d\n",i);
        char msg [3];
        int one = i/10;
        int two = i - one*10;
        msg[0]='0'+one;
        msg[1]='0'+two;
        msg[2] ='\0';
        Message *m = new Message(msg,2);
        p->send(m,TELNET_ID);
        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = 1000;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}
void ppp_poll_rdp(void *arg) {
    ppm *p = (ppm *) arg; 
    for(int i=0;i<TOT_SEND;i++) {
        //printf("sending Message %d\n",i);
        char msg [3];
        int one = i/10;
        int two = i - one*10;
        msg[0]='0'+one;
        msg[1]='0'+two;
        msg[2] ='\0';
        Message *m = new Message(msg,2);
        p->send(m,RDP_ID);
        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = 1000;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}
void ppp_poll_dns(void *arg) {
    ppm *p = (ppm *) arg; 
    for(int i=0;i<TOT_SEND;i++) {
        //printf("sending Message %d\n",i);
        char msg [3];
        int one = i/10;
        int two = i - one*10;
        msg[0]='0'+one;
        msg[1]='0'+two;
        msg[2] ='\0';
        Message *m = new Message(msg,2);
        p->send(m,DNS_ID);
        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = 1000;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}
