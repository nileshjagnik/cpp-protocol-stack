void ip_deliver_ppp(void*);
void ftp_send_ppp(void*);
void telnet_send_ppp(void*);
void tcp_send_ppp(void*);
void ip_send_ppp(void*);
void dns_send_ppp(void*);
void rdp_send_ppp(void*);
void udp_send_ppp(void*);
void eth_send_ppp(void*);
void eth_deliver_ppp(void*);
void tcp_deliver_ppp(void *arg);
void ftp_deliver_ppp(void*);
void telnet_deliver_ppp(void*);
void udp_deliver_ppp(void*);
void rdp_deliver_ppp(void*);
void dns_deliver_ppp(void*);


int FTP_count2=0;
int Telnet_count2=0;
int RDP_count2=0;
int DNS_count2=0;

struct timeval start2, end2;
long mtime2, seconds2, useconds2;

class ppp {
    public:
    ppp(ppp_info *f);
    
    private:
    ThreadPool *pool;
};

ppp::ppp(ppp_info *f) {
    gettimeofday(&start2,NULL);
    pool = new ThreadPool(16);
    if(pool->thread_avail()) {
        pool->dispatch_thread(ftp_send_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(eth_send_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(eth_deliver_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(ip_deliver_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(tcp_send_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(ip_send_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(rdp_send_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(dns_send_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(udp_send_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(telnet_send_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(tcp_deliver_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(ftp_deliver_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(telnet_deliver_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(udp_deliver_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(rdp_deliver_ppp, (void*)f);
    }
    if(pool->thread_avail()) {
        pool->dispatch_thread(dns_deliver_ppp, (void*)f);
    }
}

void ftp_send_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k =1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.ftp[0], &afds);
	    select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
	    
        sem_wait(inf->s.ftpsem);
        Message *m = new Message();
        k = read(inf->p.ftp[0], &m, sizeof(Message *));
        sem_post(inf->s.ftpsem);
        ftp_hdr *hdr=new ftp_hdr();
        hdr->hlp = FTP_ID;
        strcpy(hdr->other_info,"");
        hdr->datalength = m->msgLen();
        m->msgAddHdr((char*)hdr,16);
        if(k > 0) {
            sem_wait(inf->s.tcpsem);
            write(inf->p.tcp[1],&m,sizeof(Message *));
            sem_post(inf->s.tcpsem);
        }
    }
}

void tcp_send_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k =1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.tcp[0], &afds);
	    FD_SET(inf->p.tcp2[0], &afds);
	    select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
	    Message *m = new Message();
	    tcp_hdr *hdr=new tcp_hdr();
        strcpy(hdr->other_info,"");
        hdr->datalength = m->msgLen();
	    if (FD_ISSET(inf->p.tcp[0], &afds)) {
            hdr->hlp = FTP_ID;
            sem_wait(inf->s.tcpsem);
            k = read(inf->p.tcp[0], &m, sizeof(Message *));
            sem_post(inf->s.tcpsem);
        }
        else if (FD_ISSET(inf->p.tcp2[0], &afds)) {
            hdr->hlp = TELNET_ID;
            sem_wait(inf->s.tcp2sem);
            k = read(inf->p.tcp2[0], &m, sizeof(Message *));
            sem_post(inf->s.tcp2sem);
        }
        m->msgAddHdr((char*)hdr,12);
        if(k > 0) {
            sem_wait(inf->s.ipsem);
            write(inf->p.ip[1],&m,sizeof(Message *));
            sem_post(inf->s.ipsem);
        }
    }
}

void telnet_send_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k =1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.telnet[0], &afds);
	    select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
	    
	    Message *m = new Message();
        sem_wait(inf->s.telnetsem);
        k = read(inf->p.telnet[0], &m, sizeof(Message *));
        sem_post(inf->s.telnetsem);
        telnet_hdr *hdr=new telnet_hdr();
        hdr->hlp = TELNET_ID;
        hdr->datalength = m->msgLen();
        strcpy(hdr->other_info,"");
        m->msgAddHdr((char*)hdr,16);
        if(k > 0) {
            sem_wait(inf->s.tcp2sem);
            write(inf->p.tcp2[1],&m,sizeof(Message *));
            sem_post(inf->s.tcp2sem);
        }
    }
}

void rdp_send_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k =1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.rdp[0], &afds);
	    select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
	    
	    Message *m = new Message();
        sem_wait(inf->s.rdpsem);
        k = read(inf->p.rdp[0], &m, sizeof(Message *));
        sem_post(inf->s.rdpsem);
        rdp_hdr *hdr=new rdp_hdr();
        hdr->hlp = RDP_ID;
        hdr->datalength = m->msgLen();
        strcpy(hdr->other_info,"");
        m->msgAddHdr((char*)hdr,20);
        if(k > 0) {
            sem_wait(inf->s.udpsem);
            write(inf->p.udp[1],&m,sizeof(Message *));
            sem_post(inf->s.udpsem);
        }
        //printf("RDP to UDP\n");
    }
}

void dns_send_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k =1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.dns[0], &afds);
	    select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
	    
	    Message *m = new Message();
        sem_wait(inf->s.dnssem);
        k = read(inf->p.dns[0], &m, sizeof(Message *));
        sem_post(inf->s.dnssem);
        dns_hdr *hdr=new dns_hdr();
        hdr->hlp = DNS_ID;
        hdr->datalength = m->msgLen();
        strcpy(hdr->other_info,"");
        m->msgAddHdr((char*)hdr,16);
        if(k > 0) {
            sem_wait(inf->s.udp2sem);
            write(inf->p.udp2[1],&m,sizeof(Message *));
            sem_post(inf->s.udp2sem);
        }
        //printf("DNS to UDP\n");
    }
    
}

void udp_send_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k =1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.udp[0], &afds);
	    FD_SET(inf->p.udp2[0], &afds);
	    select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
	    Message *m = new Message();
	    udp_hdr *hdr=new udp_hdr();
        strcpy(hdr->other_info,"");
        hdr->datalength = m->msgLen();
	    if (FD_ISSET(inf->p.udp[0], &afds)) {
            hdr->hlp = RDP_ID;
            sem_wait(inf->s.udpsem);
            k = read(inf->p.udp[0], &m, sizeof(Message *));
            sem_post(inf->s.udpsem);
            //printf("UDP from RDP\n");
        }
        else if (FD_ISSET(inf->p.udp2[0], &afds)) {
            hdr->hlp = DNS_ID;
            sem_wait(inf->s.udp2sem);
            k = read(inf->p.udp2[0], &m, sizeof(Message *));
            sem_post(inf->s.udp2sem);
            //printf("UDP from DNS\n");
        }
        m->msgAddHdr((char*)hdr,12);
        if(k > 0) {
            sem_wait(inf->s.ip2sem);
            write(inf->p.ip2[1],&m,sizeof(Message *));
            sem_post(inf->s.ip2sem);
        }
    }
}

void ip_send_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k =1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.ip[0], &afds);
	    FD_SET(inf->p.ip2[0], &afds);
	    select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
	    Message *m = new Message();
	    ip_hdr *hdr=new ip_hdr();
	    if(FD_ISSET(inf->p.ip[0], &afds)) {
            sem_wait(inf->s.ipsem);
            k = read(inf->p.ip[0], &m, sizeof(Message *));
            sem_post(inf->s.ipsem);
            hdr->hlp = TCP_ID;
            strcpy(hdr->other_info,"");
            hdr->datalength = m->msgLen();
        }
        else if(FD_ISSET(inf->p.ip2[0], &afds)) {
            sem_wait(inf->s.ip2sem);
            k = read(inf->p.ip2[0], &m, sizeof(Message *));
            sem_post(inf->s.ip2sem);
            hdr->hlp = UDP_ID;
            strcpy(hdr->other_info,"");
            hdr->datalength = m->msgLen();
        }
        m->msgAddHdr((char*)hdr,20);
        if(k > 0) {
            sem_wait(inf->s.ethsem);
            write(inf->p.eth[1],&m,sizeof(Message *));
            sem_post(inf->s.ethsem);
        }
    }
}

void eth_send_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k = 1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.eth[0], &afds);
        
        int res = select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        Message *m = new Message();
        sem_wait(inf->s.ethsem);
        k = read(inf->p.eth[0], &m, sizeof(Message *));
        eth_hdr *hdr=new eth_hdr();
        hdr->hlp = IP_ID;
        strcpy(hdr->other_info,"");
        hdr->datalength = m->msgLen();
        m->msgAddHdr((char*)hdr,16);
        sem_post(inf->s.ethsem);
        
        char buf [m->msgLen()];
        m->msgFlat(buf);
        if(k > 0) {
            int n = sendto(inf->sendsock, buf, m->msgLen(), 0, (struct sockaddr *) &(inf->server), sizeof(struct sockaddr_in));
            if (n == -1) {
                printf("Error in send\n");
            }
        }
    }
}

void eth_deliver_ppp(void *arg) {
    ppp_info *f = (struct ppp_info*) arg;
    int n=1;
    unsigned int len = sizeof(struct sockaddr_in);
    char *buf;
    while (n>0) {
        buf = new char [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        if ((n = recvfrom(f->recvsock, buf, BUF_SIZE, 0, (struct sockaddr *) &(f->other), &len)) != -1) {
            //sprintf("%s\n",buf);
            Message *m = new Message(buf,n);
            m->msgStripHdr(16);
            sem_wait(f->s.eth_upsem);
            write(f->p.eth_up[1], &m, sizeof(Message *));
            sem_post(f->s.eth_upsem);
        }
        else {
            printf("Failed to recv message");
            exit(0);
        }
    }   
}

void ip_deliver_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k = 1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.eth_up[0], &afds);
        
        int res = select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        Message *m = new Message();
        sem_wait(inf->s.eth_upsem);
        k = read(inf->p.eth_up[0], &m, sizeof(Message *));
        sem_post(inf->s.eth_upsem);
        /*
        char buf [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        m->msgFlat(buf);
        printf("IP got:%s\n",buf); */
        ip_hdr* hdr = (ip_hdr*)m->msgStripHdr(20);
        
        if(hdr->hlp == TCP_ID) {
            sem_wait(inf->s.ip_upsem);
            write(inf->p.ip_up[1], &m, sizeof(Message *));
            sem_post(inf->s.ip_upsem);
        }
        else if(hdr->hlp == UDP_ID) {
            sem_wait(inf->s.ip2_upsem);
            write(inf->p.ip2_up[1], &m, sizeof(Message *));
            sem_post(inf->s.ip2_upsem);
        }
    }
}

void udp_deliver_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k = 1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.ip2_up[0], &afds);
        
        int res = select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        Message *m = new Message();
        sem_wait(inf->s.ip2_upsem);
        k = read(inf->p.ip2_up[0], &m, sizeof(Message *));
        sem_post(inf->s.ip2_upsem);
        udp_hdr* hdr = (udp_hdr *) m->msgStripHdr(12);
        
        if (hdr->hlp == RDP_ID) {
            sem_wait(inf->s.udp_upsem);
            write(inf->p.udp_up[1], &m, sizeof(Message *));
            sem_post(inf->s.udp_upsem);
        }
        else if (hdr->hlp == DNS_ID) {
            sem_wait(inf->s.udp2_upsem);
            write(inf->p.udp2_up[1], &m, sizeof(Message *));
            sem_post(inf->s.udp2_upsem);
        }
    }
}

void dns_deliver_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k = 1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.udp2_up[0], &afds);
        
        int res = select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        Message *m = new Message();
        sem_wait(inf->s.udp2_upsem);
        k = read(inf->p.udp2_up[0], &m, sizeof(Message *));
        sem_post(inf->s.udp2_upsem);
        m->msgStripHdr(16);
        /*
        char buf [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        m->msgFlat(buf);
        printf("TELNET got:%s\n",buf);
        */
        DNS_count2++;
        if((FTP_count2==TOT_SEND) && (Telnet_count2==TOT_SEND)&&(RDP_count2==TOT_SEND) &&(DNS_count2==TOT_SEND)) {
            gettimeofday(&end2,NULL);
            useconds2 = end2.tv_usec - start2.tv_usec;
	        seconds2 = end2.tv_sec - start2.tv_sec;
	        if(useconds2<0) {
	            seconds2--;
	            useconds2 = 1000000 - useconds2;
	        }
	        cout << "elapsed time: "<< seconds2 << " seconds " << useconds2 << " microseconds\n" << endl;
        }
     }
}

void rdp_deliver_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k = 1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.udp_up[0], &afds);
        
        int res = select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        Message *m = new Message();
        sem_wait(inf->s.udp_upsem);
        k = read(inf->p.udp_up[0], &m, sizeof(Message *));
        sem_post(inf->s.udp_upsem);
        m->msgStripHdr(20);
        /*
        char buf [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        m->msgFlat(buf);
        printf("RDP got:%s\n",buf);
        */
        RDP_count2++;
        if((FTP_count2==TOT_SEND) && (Telnet_count2==TOT_SEND)&&(RDP_count2==TOT_SEND) &&(DNS_count2==TOT_SEND)) {
            gettimeofday(&end2,NULL);
            useconds2 = end2.tv_usec - start2.tv_usec;
	        seconds2 = end2.tv_sec - start2.tv_sec;
	        if(useconds2<0) {
	            seconds2--;
	            useconds2 = 1000000 - useconds2;
	        }
	        cout << "elapsed time: "<< seconds2 << " seconds " << useconds2 << " microseconds\n" << endl;
        }
     }
}

void tcp_deliver_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k = 1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.ip_up[0], &afds);
        
        int res = select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        Message *m = new Message();
        sem_wait(inf->s.ip_upsem);
        k = read(inf->p.ip_up[0], &m, sizeof(Message *));
        sem_post(inf->s.ip_upsem);
        tcp_hdr* hdr = (tcp_hdr *) m->msgStripHdr(12);
        
        if (hdr->hlp == FTP_ID) {
            sem_wait(inf->s.tcp_upsem);
            write(inf->p.tcp_up[1], &m, sizeof(Message *));
            sem_post(inf->s.tcp_upsem);
        }
        else if (hdr->hlp == TELNET_ID) {
            sem_wait(inf->s.tcp2_upsem);
            write(inf->p.tcp2_up[1], &m, sizeof(Message *));
            sem_post(inf->s.tcp2_upsem);
        }
    }
}

void ftp_deliver_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k = 1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.tcp_up[0], &afds);
        
        int res = select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        Message *m = new Message();
        sem_wait(inf->s.tcp_upsem);
        k = read(inf->p.tcp_up[0], &m, sizeof(Message *));
        sem_post(inf->s.tcp_upsem);
        m->msgStripHdr(16);
        
        /*
        char buf [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        m->msgFlat(buf);
        printf("FTP got:%s\n",buf);
        */
        FTP_count2++;
        if((FTP_count2==TOT_SEND) && (Telnet_count2==TOT_SEND)&&(RDP_count2==TOT_SEND) &&(DNS_count2==TOT_SEND)) {
            gettimeofday(&end2,NULL);
            useconds2 = end2.tv_usec - start2.tv_usec;
	        seconds2 = end2.tv_sec - start2.tv_sec;
	        if(useconds2<0) {
	            seconds2--;
	            useconds2 = 1000000 - useconds2;
	        }
	        cout << "elapsed time: "<< seconds2 << " seconds " << useconds2 << " microseconds\n" << endl;
        }
     }
}

void telnet_deliver_ppp(void *arg) {
    fd_set afds;
    ppp_info* inf = (ppp_info*) arg;
    int k = 1;
    int nfds = getdtablesize();
    while (k > 0) {
        FD_ZERO(&afds);
	    FD_SET(inf->p.tcp2_up[0], &afds);
        
        int res = select(nfds, &afds, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        Message *m = new Message();
        sem_wait(inf->s.tcp2_upsem);
        k = read(inf->p.tcp2_up[0], &m, sizeof(Message *));
        sem_post(inf->s.tcp2_upsem);
        m->msgStripHdr(16);
        /*
        char buf [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        m->msgFlat(buf);
        printf("TELNET got:%s\n",buf);
        */
        Telnet_count2++;
        if((FTP_count2==TOT_SEND) && (Telnet_count2==TOT_SEND)&&(RDP_count2==TOT_SEND) &&(DNS_count2==TOT_SEND)) {
            gettimeofday(&end2,NULL);
            useconds2 = end2.tv_usec - start2.tv_usec;
	        seconds2 = end2.tv_sec - start2.tv_sec;
	        if(useconds2<0) {
	            seconds2--;
	            useconds2 = 1000000 - useconds2;
	        }
	        cout << "elapsed time: "<< seconds2 << " seconds " << useconds2 << " microseconds\n" << endl;
        }
     }
}
