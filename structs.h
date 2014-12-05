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

struct tcp_hdr {
    int hlp;
    char other_info[4];
    int datalength;
};

struct ftp_hdr {
    int hlp;
    char other_info[8];
    int datalength;
};

struct telnet_hdr {
    int hlp;
    char other_info[8];
    int datalength;
};

struct rdp_hdr {
    int hlp;
    char other_info[12];
    int datalength;
};

struct dns_hdr {
    int hlp;
    char other_info[8];
    int datalength;
};

struct udp_hdr {
    int hlp;
    char other_info[4];
    int datalength;
};

struct pipelist {
    int ftp[2];
    int ftp_up[2];
    int telnet[2];
    int telnet_up[2];
    int rdp[2];
    int rdp_up[2];
    int dns[2];
    int dns_up[2];
    int tcp[2];
    int tcp_up[2];
    int tcp2[2];
    int tcp2_up[2];
    int udp[2];
    int udp_up[2];
    int udp2[2];
    int udp2_up[2];
    int ip[2];
    int ip_up[2];
    int ip2[2];
    int ip2_up[2];
    int eth[2];
    int eth_up[2];
};

struct semlist {
    sem_t *ftpsem;
    sem_t *ftp_upsem;
    sem_t *telnetsem;
    sem_t *telnet_upsem;
    sem_t *rdpsem;
    sem_t *rdp_upsem;
    sem_t *dnssem;
    sem_t *dns_upsem;
    sem_t *tcpsem;
    sem_t *tcp_upsem;
    sem_t *tcp2sem;
    sem_t *tcp2_upsem;
    sem_t *udpsem;
    sem_t *udp_upsem;
    sem_t *udp2sem;
    sem_t *udp2_upsem;
    sem_t *ipsem;
    sem_t *ip_upsem;
    sem_t *ip2sem;
    sem_t *ip2_upsem;
    sem_t *ethsem;
    sem_t *eth_upsem;
};

struct ppp_info {
    int sendsock;
    int recvsock;
    pipelist p;
    semlist  s;
    struct sockaddr_in server;
    struct sockaddr_in other;
};
