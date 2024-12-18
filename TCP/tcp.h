#define CHUNKDATASIZE 3
#define MAXCHUNKS 100
#define port 8080
#define BUFFER_SIZE 1024
#define TIMEOUT_USEC 100000  // 0.1 seconds

typedef struct CHUNK{
    int seqNum;
    int totalChunks;
    char chunkData[CHUNKDATASIZE];
}CHUNK;

typedef struct ACK{
    int seqNum;
}ACK;

void deliver(int sockfd,char * buffer, int buffer_len,struct sockaddr * serveraddr,int addr_len);
void receive(int sockfd,struct sockaddr * senderaddr,int * addr_len);
