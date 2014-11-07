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
    list<char*> *msg_list=NULL;

};

Message::Message()
{
    msglen=0;
    msg_list = new list<char*>();
}

Message::Message(char* msg, size_t len)
{
    msglen = len;
    msg_list = new list<char*>();
    for (int i=0;i<(int)len;i++) {
        msg_list->push_back(msg+i);
    }
    
}

Message::~Message( )
{
    delete msg_list;
}

void Message::msgAddHdr(char *hdr, size_t length)
{
    for (int i=0;i<(int)length;i++) {
        msg_list->push_front(hdr+(int)length-i-1);
    }
    msglen += length;
}

char* Message::msgStripHdr(int len)
{
    char *stripped_content;
    if ((msglen < len) || (len == 0)) return NULL;
    stripped_content = new char[len];
    for (int i=0;i<(int)len;i++) {
        stripped_content[i]=*(msg_list->front());
        msg_list->pop_front();
    }
    msglen -= len;
    return stripped_content;
}

int Message::msgSplit(Message& secondMsg, size_t len)
{
    size_t length = msglen;

    if ((len < 0) || (len > msglen)) return 0;

    msglen = len;
    
    for(int i=(int)length-1;i>=(int)len;i--){
        secondMsg.msg_list->push_front(msg_list->back());
        msg_list->pop_back();
    }
    secondMsg.msglen = length-len;
    return 1;
}

void Message::msgJoin(Message& secondMsg)
{   
    size_t len;
    len = secondMsg.msgLen();
    for(int i=0;i<(int)len;i++){
        msg_list->push_back(secondMsg.msg_list->front());
        secondMsg.msg_list->pop_front();
    }
    msglen+=len;
}

size_t Message::msgLen( )
{
    return msglen;
}

void Message::msgFlat(char *buffer)
{
    //Assume that sufficient memory has been allocated in buffer
    int i=0;
    for (list<char*>::iterator it = msg_list->begin(); it != msg_list->end(); it++,i++) {
        *(buffer+i) = *(*it);
    }
}

