#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "tcp.h"

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    char message[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    printf("\t\tClient is running\n\n");

    int len = sizeof(servaddr);

    while (1) {
        printf("\nType a message: \n");
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = 0;  // Remove the newline character

        if (strcmp(message, "exit") == 0) {
            break;
        }

        // Use the deliver function to send the message in chunks
        deliver(sockfd, message, strlen(message), (struct sockaddr *)&servaddr, len);
        
        // Receive and reassemble the response from the server using receive function
        receive(sockfd, (struct sockaddr *)&servaddr, &len);
    }

    close(sockfd);
    return 0;
}
