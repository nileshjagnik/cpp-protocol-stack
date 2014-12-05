void ftp_deliver_ppp(void*);
void ftp_send_ppp(void*);
void eth_send_ppp(void*);
void eth_deliver_ppp(void*);


class ppp {
    public:
    ppp(ppp_info *f);
    
    private:
    ThreadPool *pool;
};

ppp::ppp(ppp_info *f) {
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
        pool->dispatch_thread(ftp_deliver_ppp, (void*)f);
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
        char buf [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        m->msgFlat(buf);
        //printf("FTP:%s\n",buf);
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
        sem_post(inf->s.ethsem);
        char buf [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        m->msgFlat(buf);
        //printf("ETH:%s\n",buf);
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

void ftp_deliver_ppp(void *arg) {
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
        char buf [BUF_SIZE];
        bzero(buf,BUF_SIZE);
        m->msgFlat(buf);
        printf("FTP got:%s\n",buf);
    }
}
