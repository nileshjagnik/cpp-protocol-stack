struct info {
    int ssocket;
    int rsocket;
    sem_t *sendsem;
    sem_t *recvsem;
    sockaddr_in other;
    sockaddr_in server;
    int tot;
    ThreadPool *pool;
};

struct send_input {
    int protocol_id;
    Message *msg;
    info f;
};

struct ip_hdr {
    int hlp;
    char other_info[12];
    int datalength;
};

struct eth_hdr {
    int hlp;
    char other_info[8];
    int datalength;
};
