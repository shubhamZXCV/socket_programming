#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "tcp.h"

// Deliver the data in chunks, with retransmission logic
void deliver(int sockfd, char *buffer, int buffer_len, struct sockaddr *recvaddr, int addr_len)
{
    int rem = strlen(buffer) % CHUNKDATASIZE;
    int len = strlen(buffer);
    int totalChunks = (rem == 0) ? (len / CHUNKDATASIZE) : (len / CHUNKDATASIZE) + 1;

    CHUNK chunks[totalChunks];
    ACK ack;
    int ackReceived[totalChunks]; // To track ACKs
    memset(ackReceived, 0, sizeof(ackReceived));

    // Prepare chunks
    for (int i = 0; i < totalChunks; i++)
    {
        chunks[i].seqNum = i;
        chunks[i].totalChunks = totalChunks;
        strncpy(chunks[i].chunkData, buffer + CHUNKDATASIZE * i, CHUNKDATASIZE);
    }

    // Set the socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl get flags failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl set non-blocking failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    struct timeval timeout;
    fd_set read_fds;
    int resendCount;

    for (int i = 0; i < totalChunks; i++)
    {
        if(i%2==1){

        sendto(sockfd, &chunks[i], sizeof(CHUNK), 0, recvaddr, addr_len);
        }
    }

    while (1)
    {
        int ackCount = 0;
        for (int i = 0; i < totalChunks; i++)
        {
            if (ackReceived[i] == 1)
            {
                ackCount++;
                continue;
            }

            // Set timeout for ACK
            FD_ZERO(&read_fds);
            FD_SET(sockfd, &read_fds);

            timeout.tv_sec = 0;
            timeout.tv_usec = TIMEOUT_USEC;

            int result = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);
            if (result > 0 && FD_ISSET(sockfd, &read_fds))
            {
                // ACK received
                int bytes = recvfrom(sockfd, &ack, sizeof(ACK), 0, recvaddr, &addr_len);
                printf("ack %d recieved\n", ack.seqNum);
                if (bytes > 0)
                {
                    ackReceived[ack.seqNum] = 1;
                }
            }
            else
            {
                // Timeout, resend chunk
                printf("Resending chunk %d\n", i);
                sendto(sockfd, &chunks[i], sizeof(CHUNK), 0, recvaddr, addr_len);
            }
        }
      
        if (ackCount == totalChunks)
        {
            break;
        }
    }
}

// Receive the data and simulate skipping every third ACK
void receive(int sockfd, struct sockaddr *senderaddr, int *addr_len)
{
    CHUNK chunkRecv;
    ACK ack;
    int totalChunks = 0;
    int chunkRecvCounter = 0;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int skipAckCounter = 0;

    int chunkRecvHash[3000];
    memset(chunkRecvHash,0,3000);
    int ackCounter=0;
    while (1)
    {
        int bytes = recvfrom(sockfd, &chunkRecv, sizeof(CHUNK), 0, senderaddr, addr_len);
        if (bytes > 0)
        {
            int seq = chunkRecv.seqNum;
            totalChunks = chunkRecv.totalChunks;

            strncpy(buffer + seq * CHUNKDATASIZE, chunkRecv.chunkData, CHUNKDATASIZE);

            if (chunkRecvHash[seq] == 0)
            {
                chunkRecvCounter++;
                printf("chunk %d recieved\n", seq);
                printf("%s\n", chunkRecv.chunkData);
            }


            chunkRecvHash[seq]++;



          
         
            ack.seqNum = seq;
            if((seq+1)%3!=0 || chunkRecvHash[seq]>1){
                ackCounter++;
            sendto(sockfd, &ack, sizeof(ACK), 0, senderaddr, *addr_len);
            }

        

            if (chunkRecvCounter + ackCounter== 2*totalChunks)
            {
                break;
            }
        }
    }

    // Display received data
    printf("Received Data: %s\n", buffer);
}
