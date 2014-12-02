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
    list<size_t> *size_list=NULL;
};

Message::Message()  //Default constructor
{
    msglen=0;
    msg_list = new list<char*>();
    size_list = new list<size_t>();
}

Message::Message(char* msg, size_t len)
{
    msglen = len;
    msg_list = new list<char*>();
    size_list = new list<size_t>();
    msg_list->push_back(msg);
    size_list->push_back(len);
}

Message::~Message( )
{
    delete msg_list;
    delete size_list;
}

void Message::msgAddHdr(char *hdr, size_t length)
{
    msg_list->push_front(hdr);
    size_list->push_front(length);
    msglen += length;
}

char* Message::msgStripHdr(int len)
{
    char *stripped_content;
    if ((msglen < len) || (len == 0)) return NULL;
    stripped_content = new char[len+1];
    size_t rem_len = len; 
    while(rem_len>=size_list->front()) {
        for(int i =0;i<size_list->front();i++){
            stripped_content[len-rem_len+i] = *(msg_list->front()+i);
        }
        msg_list->pop_front();
        rem_len-=size_list->front();
        size_list->pop_front();
    }
    if(rem_len>0) {
        for(int i =0;i<rem_len;i++){
            stripped_content[len-rem_len+i] = *(msg_list->front()+i);
        }
        char* frontmsg = msg_list->front();
        msg_list->pop_front();
        msg_list->push_front(frontmsg+rem_len);
        size_t frontlen = size_list->front();
        size_list->pop_front();
        size_list->push_front(frontlen-rem_len);
    }
    msglen -= len;
    return stripped_content;
}

int Message::msgSplit(Message& secondMsg, size_t len)
{
    size_t length = msglen;

    if ((len < 0) || (len > msglen)) return 0;

    msglen = len;
    
    int rem_len = length-len;
    while(rem_len >= size_list->back()) {
        
        secondMsg.msg_list->push_front(msg_list->back());
        msg_list->pop_back();
        secondMsg.size_list->push_front(size_list->back());
        rem_len -= size_list->back();
        size_list->pop_back();
        
        
    }
    
    if(rem_len > 0) {
        char *last_str = msg_list->back();
        size_t last_len = size_list->back();
        secondMsg.msg_list->push_front(last_str + (last_len - rem_len));
        size_list->pop_back();
        size_list->push_back(last_len - rem_len);
        secondMsg.size_list->push_front(rem_len);
    }
    
    secondMsg.msglen = length-len;
    return 1;
}

void Message::msgJoin(Message& secondMsg)
{   
    list<size_t>::iterator s_it = secondMsg.size_list->begin();
    for (list<char*>::iterator it = secondMsg.msg_list->begin(); it != secondMsg.msg_list->end(); it++,s_it++) {
        msg_list->push_back(*it);
        size_list->push_back(*s_it);
    }
    msglen+=secondMsg.msglen;
}

size_t Message::msgLen( )
{
    return msglen;
}

void Message::msgFlat(char *buffer)
{
    //Assume that sufficient memory has been allocated in buffer
    int j=0;
    list<size_t>::iterator s_it = size_list->begin();
    for (list<char*>::iterator it = msg_list->begin(); it != msg_list->end(); it++,s_it++) {
        for(int i=0;i<(*s_it);i++,j++) {
            *(buffer+j) = *(*it + i); 
        }
    }
}
