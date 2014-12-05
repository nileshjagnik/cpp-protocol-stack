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
#include "ppp.h"

using namespace std;

extern int	errno;
int		errexit(const char *format, ...);
void ppp_poll_ftp(void *arg);
void ppp_poll_telnet(void *arg);
void ppp_poll_rdp(void *arg);
void ppp_poll_dns(void *arg);
void ppm_poll_ftp(void *arg);
void ppm_poll_telnet(void *arg);
void ppm_poll_rdp(void *arg);
void ppm_poll_dns(void *arg);

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
    ppp *pp;
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
            thp.dispatch_thread(ppm_poll_ftp,(void *)p);
        }
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppm_poll_telnet,(void *)p);
        }
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppm_poll_rdp,(void *)p);
        }
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppm_poll_dns,(void *)p);
        }
    }
    else {
        ppp_info p;
        p.sendsock = send_sock;
        p.recvsock = recv_sock;
        p.server = server;
        p.other = other;
        p.s.ftpsem = new sem_t();
        sem_init(p.s.ftpsem, 0, 1);
        p.s.ftp_upsem = new sem_t();
        sem_init(p.s.ftp_upsem, 0, 1);
        p.s.rdpsem = new sem_t();
        sem_init(p.s.rdpsem, 0, 1);
        p.s.rdp_upsem = new sem_t();
        sem_init(p.s.rdp_upsem, 0, 1);
        p.s.dnssem = new sem_t();
        sem_init(p.s.dnssem, 0, 1);
        p.s.dns_upsem = new sem_t();
        sem_init(p.s.dns_upsem, 0, 1);
        p.s.telnetsem = new sem_t();
        sem_init(p.s.telnetsem, 0, 1);
        p.s.telnet_upsem = new sem_t();
        sem_init(p.s.telnet_upsem, 0, 1);
        p.s.tcpsem = new sem_t();
        sem_init(p.s.tcpsem, 0, 1);
        p.s.tcp_upsem = new sem_t();
        sem_init(p.s.tcp_upsem, 0, 1);
        p.s.tcp2sem = new sem_t();
        sem_init(p.s.tcp2sem, 0, 1);
        p.s.tcp2_upsem = new sem_t();
        sem_init(p.s.tcp2_upsem, 0, 1);
        p.s.udpsem = new sem_t();
        sem_init(p.s.udpsem, 0, 1);
        p.s.udp_upsem = new sem_t();
        sem_init(p.s.udp_upsem, 0, 1);
        p.s.udp2sem = new sem_t();
        sem_init(p.s.udp2sem, 0, 1);
        p.s.udp2_upsem = new sem_t();
        sem_init(p.s.udp2_upsem, 0, 1);
        p.s.ipsem = new sem_t();
        sem_init(p.s.ipsem, 0, 1);
        p.s.ip_upsem = new sem_t();
        sem_init(p.s.ip_upsem, 0, 1);
        p.s.ip2sem = new sem_t();
        sem_init(p.s.ip2sem, 0, 1);
        p.s.ip2_upsem = new sem_t();
        sem_init(p.s.ip2_upsem, 0, 1);
        p.s.ethsem = new sem_t();
        sem_init(p.s.ethsem, 0, 1);
        p.s.eth_upsem = new sem_t();
        sem_init(p.s.eth_upsem, 0, 1);
        pipe(p.p.ftp);
        pipe(p.p.ftp_up);
        pipe(p.p.telnet);
        pipe(p.p.telnet_up);
        pipe(p.p.rdp);
        pipe(p.p.rdp_up);
        pipe(p.p.dns);
        pipe(p.p.dns_up);
        pipe(p.p.tcp);
        pipe(p.p.tcp_up);
        pipe(p.p.tcp2);
        pipe(p.p.tcp2_up);
        pipe(p.p.udp);
        pipe(p.p.udp_up);
        pipe(p.p.udp2);
        pipe(p.p.udp2_up);
        pipe(p.p.ip);
        pipe(p.p.ip_up);
        pipe(p.p.ip2);
        pipe(p.p.ip2_up);
        pipe(p.p.eth);
        pipe(p.p.eth_up);
        pp = new ppp(&p);
        ThreadPool thp(4);
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppp_poll_ftp,(void *)&p);
        }
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppp_poll_telnet,(void *)&p);
        }
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppp_poll_rdp,(void *)&p);
        }
        if (thp.thread_avail()) {
            thp.dispatch_thread(ppp_poll_dns,(void *)&p);
        }
    }
    struct timeval time;
    time.tv_sec = 2;
    time.tv_usec = TIME_OUT;
    int error;
    if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
        cout<<"Error in select, returned :"<<error<<endl;
    }
}

void ppm_poll_ftp(void *arg) {
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
        time.tv_usec = TIME_OUT;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}
void ppm_poll_telnet(void *arg) {
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
        time.tv_usec = TIME_OUT;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}
void ppm_poll_rdp(void *arg) {
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
        time.tv_usec = TIME_OUT;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}
void ppm_poll_dns(void *arg) {
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
        time.tv_usec = TIME_OUT;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}

void ppp_poll_ftp(void *arg) {
    ppp_info p = *((ppp_info *)arg) ;
    for(int i=0;i<TOT_SEND;i++) {
        //printf("sending Message %d\n",i);
        char msg [3];
        int one = i/10;
        int two = i - one*10;
        msg[0]='0'+one;
        msg[1]='0'+two;
        msg[2] ='\0';
        Message *m = new Message(msg,2);
        
        sem_wait(p.s.ftpsem);
        write(p.p.ftp[1],&m,sizeof(Message *));
        sem_post(p.s.ftpsem);
        
        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = TIME_OUT;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}

void ppp_poll_telnet(void *arg) {
    ppp_info p = *((ppp_info *)arg) ;
    for(int i=0;i<TOT_SEND;i++) {
        //printf("sending Message %d\n",i);
        char msg [3];
        int one = i/10;
        int two = i - one*10;
        msg[0]='0'+one;
        msg[1]='0'+two;
        msg[2] ='\0';
        Message *m = new Message(msg,2);
        
        sem_wait(p.s.telnetsem);
        write(p.p.telnet[1],&m,sizeof(Message *));
        sem_post(p.s.telnetsem);
        
        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = TIME_OUT;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}

void ppp_poll_rdp(void *arg) {
    ppp_info p = *((ppp_info *)arg) ;
    for(int i=0;i<TOT_SEND;i++) {
        //printf("sending Message %d\n",i);
        char msg [3];
        int one = i/10;
        int two = i - one*10;
        msg[0]='0'+one;
        msg[1]='0'+two;
        msg[2] ='\0';
        Message *m = new Message(msg,2);
        
        sem_wait(p.s.rdpsem);
        write(p.p.rdp[1],&m,sizeof(Message *));
        sem_post(p.s.rdpsem);
        
        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = TIME_OUT;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}

void ppp_poll_dns(void *arg) {
    ppp_info p = *((ppp_info *)arg) ;
    for(int i=0;i<TOT_SEND;i++) {
        //printf("sending Message %d\n",i);
        char msg [3];
        int one = i/10;
        int two = i - one*10;
        msg[0]='0'+one;
        msg[1]='0'+two;
        msg[2] ='\0';
        Message *m = new Message(msg,2);
        
        sem_wait(p.s.dnssem);
        write(p.p.dns[1],&m,sizeof(Message *));
        sem_post(p.s.dnssem);
        
        struct timeval time;
        time.tv_sec = 0;
        time.tv_usec = TIME_OUT;
        int error;
        if ((error = select(0, (fd_set *)0, (fd_set *)0, (fd_set *)0, &time)) != 0) {
            cout<<"Error in select, returned :"<<error<<endl;
        }
    }
}
