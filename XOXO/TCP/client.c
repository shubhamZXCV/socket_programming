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

int sock = 0, valread;
struct sockaddr_in serv_addr;

int main()
{
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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


   

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    valread = recv(sock, buffer, BUFFER_SIZE, 0);
    buffer[valread] = '\0';
    printf("\n%s\n", buffer); // Print Player info

    while (true)
    {
        memset(buffer,0,sizeof(buffer));
        valread = recv(sock, buffer, BUFFER_SIZE, 0);

        buffer[valread] = '\0';
       printf("\n%s\n",buffer);

        if (strcmp(buffer, "turn") == 0 || strcmp(buffer,"invalid")==0)
        {
            printf("Your turn! Enter row and col (1-3): ");
            scanf("%d %d", &row, &col);

            // Send row and col to server
            sprintf(buffer, "%d %d", row, col);
            send(sock, buffer, strlen(buffer), 0);
        }
        else if (strcmp(buffer, "win") == 0)
        {
            printf("You Win!\n");
            char choice[BUFFER_SIZE];
            printf("Do you wish to play again?(y/n): ");
            scanf("%s", choice);
            send(sock, choice, sizeof(choice), 0);
        }
        else if (strcmp(buffer, "loser") == 0)
        {
            printf("You Lose.\n");
            char choice[BUFFER_SIZE];
            printf("Do you wish to play again?(y/n): ");
            scanf("%s", choice);
            send(sock, choice, sizeof(choice), 0);
        }
        else if (strcmp(buffer, "draw") == 0)
        {
            printf("It's a draw!\n");
            char choice[BUFFER_SIZE];
            printf("Do you wish to play again?(y/n): ");
            scanf("%s", choice);
            send(sock, choice, sizeof(choice), 0);
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
