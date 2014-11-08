#include<list>
using namespace std;
class Message
{
public:  
    Message( );
    Message(char* msg, size_t len);
    ~Message( );
    void msgAddHdr(char *hdr, size_t length);
    char* msgStripHdr(int len);
    int msgSplit(Message& secondMsg, size_t len);
    void msgJoin(Message& secondMsg);
    size_t msgLen( );
    void msgFlat(char *buffer);
private:
    size_t msglen;
    list<char*> *msglist=NULL;
};

Message::Message()
{
    msglen=0;
    msglist = new list<char*>();
}

Message::Message(char* msg, size_t len)
{
    msglen = len;
    msglist = new list<char*>();
    
    for (int i=0;i<(int)len;i++) 
    {
        msglist->push_back(msg+i);
    }    
}

Message::~Message( )
{
    delete msglist;
}

void Message::msgAddHdr(char *hdr, size_t length)
{
    for (int i=0;i<(int)length;i++) 
    {
        msglist->push_front(hdr+(int)length-i-1);
    }
    msglen += length;
}

char* Message::msgStripHdr(int len)
{
    char *stripped_content;
    if ((msglen < len) || (len == 0)) return NULL;
    stripped_content = new char[len];    
    for (int i=0;i<(int)len;i++)
    {
        stripped_content[i]=*(msglist->front());
        msglist->pop_front();
    }    
    msglen -= len;
    return stripped_content;
}

int Message::msgSplit(Message& secondMsg, size_t len)
{
    size_t length = msglen;
    if ((len < 0) || (len > msglen)) return 0;
    msglen = len; 
       
    for(int i=(int)length-1;i>(int)len-1;i--)
    {
        secondMsg.msglist->push_front(msglist->back());
        msglist->pop_back();
    }    
    secondMsg.msglen=length-len;
    return 1;
}

void Message::msgJoin(Message& secondMsg)
{   
    size_t len;
    len = secondMsg.msgLen();
    
    for(int i=0;i<(int)len;i++)
    {
        msglist->push_back(secondMsg.msglist->front());
        secondMsg.msglist->pop_front();
    }    
    msglen+=len;
}

size_t Message::msgLen( )
{
    return msglen;
}

void Message::msgFlat(char *buffer)
{
    int i=0;
    for (list<char*>::iterator it = msglist->begin(); it != msglist->end(); it++,i++) 
    {
        *(buffer+i) = *(*it);
    }
}

