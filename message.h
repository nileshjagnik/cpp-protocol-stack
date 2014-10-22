/*********************/
/* This is the most primitive implementation of the Message library.
*/

/* Written by: Shiv Mishra on October 20, 2014 */
/* Last update: October 20, 2014 */

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
    char *msg_content;
};

    Message::Message()
    {
	msglen = 0;
	msg_content = NULL;
    }

    Message::Message(char* msg, size_t len)
    {
	msglen = len;
	msg_content = new char[len];
	memcpy(msg_content, msg, len);
    }

    Message::~Message( )
    {
            delete msg_content;
    }

    void Message::msgAddHdr(char *hdr, size_t length)
    {
	char *new_msg_content;

	new_msg_content = new char[msglen + length];
        memcpy(new_msg_content, hdr, length);
	memcpy(new_msg_content + length, msg_content, msglen);
	delete msg_content;
	msg_content = new_msg_content;
	msglen += length;
    }

    char* Message::msgStripHdr(int len)
    {
	char *new_msg_content;
	char *stripped_content;
	
        if ((msglen < len) || (len == 0)) return NULL;

	new_msg_content = new char[msglen - len];
	stripped_content = new char[len];
	memcpy(stripped_content, msg_content, len);
	memcpy(new_msg_content, msg_content + len, msglen - len);
	msglen -= len;
	delete msg_content;
	msg_content = new_msg_content;
	return stripped_content;
    }

    int Message::msgSplit(Message& secondMsg, size_t len)
    {
	char *content = msg_content;
	size_t length = msglen;

	if ((len < 0) || (len > msglen)) return 0;

	msg_content = new char[len];
	msglen = len;
	memcpy(msg_content, content, len);
	secondMsg.msglen = length - len;
	secondMsg.msg_content = new char[secondMsg.msglen];
	memcpy(secondMsg.msg_content, content + len, secondMsg.msglen);
	delete content;
	return 1;
    }

    void Message::msgJoin(Message& secondMsg)
    {
	char *content = msg_content;
	size_t length = msglen;
	
	msg_content = new char[msglen + secondMsg.msglen];
	msglen += secondMsg.msglen;
	memcpy(msg_content, content, length);
	memcpy(msg_content + length, secondMsg.msg_content, secondMsg.msglen);
	delete content;
	delete secondMsg.msg_content;
	secondMsg.msg_content = NULL;
	secondMsg.msglen = 0;
    }

    size_t Message::msgLen( )
    {
	return msglen;
    }

    void Message::msgFlat(char *buffer)
    {
	//Assume that sufficient memory has been allocated in buffer

	memcpy(buffer, msg_content, msglen);
    }

