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
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed\n");
        exit(EXIT_FAILURE);
    }

    printf("\t\tServer is running..\n\n");

    while (1) {
        int len = sizeof(cliaddr);

        // Receive data from the client in chunks and assemble it
        receive(sockfd, (struct sockaddr *)&cliaddr, &len);
        
        printf("\nType a response: \n");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Remove the newline character

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Send response to the client using deliver function
        deliver(sockfd, buffer, strlen(buffer), (struct sockaddr *)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}
