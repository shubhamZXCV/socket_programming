#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include<arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE] = {0};
int row, col;

int sock;
struct sockaddr_in serv_addr;
socklen_t addr_len = sizeof(serv_addr);

int main()
{

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
     if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }


    sendto(sock,"client is running..", 20, 0, (struct sockaddr *)&serv_addr, addr_len);
    int valread = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_len);
    buffer[valread] = '\0';
    printf("\n%s\n", buffer); 

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        valread = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&serv_addr, &addr_len);
        buffer[valread] = '\0';
        printf("\n%s\n", buffer); 

        if (strcmp(buffer, "turn") == 0 || strcmp(buffer, "invalid") == 0)
        {
            printf("Your turn! Enter row and col (1-3): ");
            scanf("%d %d", &row, &col);
         
            sprintf(buffer, "%d %d", row, col);
            sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&serv_addr, addr_len);
        }
        else if (strcmp(buffer, "win") == 0)
        {
            printf("You Win!\n");
            char choice[BUFFER_SIZE];
            printf("Do you wish to play again? (y/n): ");
            scanf("%s", choice);
            sendto(sock, choice, strlen(choice), 0, (struct sockaddr *)&serv_addr, addr_len);
        }
        else if (strcmp(buffer, "loser") == 0)
        {
            printf("You Lose.\n");
            char choice[BUFFER_SIZE];
            printf("Do you wish to play again? (y/n): ");
            scanf("%s", choice);
            sendto(sock, choice, strlen(choice), 0, (struct sockaddr *)&serv_addr, addr_len);
        }
        else if (strcmp(buffer, "draw") == 0)
        {
            printf("It's a draw!\n");
            char choice[BUFFER_SIZE];
            printf("Do you wish to play again? (y/n): ");
            scanf("%s", choice);
            sendto(sock, choice, strlen(choice), 0, (struct sockaddr *)&serv_addr, addr_len);
        }
        else if (strcmp(buffer, "end") == 0)
        {
            printf("Game over. Thanks for playing!\n");
            break;
        }
    }

    close(sock);
    return 0;
}
