#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int board[3][3];
char charboard[3][3];
int playerTurn = 1;
char buffer[BUFFER_SIZE] = {0};
int row, col;

struct sockaddr_in player1_addr, player2_addr;
socklen_t addr_len = sizeof(struct sockaddr_in);
int serverFd;

void gameLoop();
void sendBoard(struct sockaddr_in *player1, struct sockaddr_in *player2);

void init_board()
{
    memset(board, 0, sizeof(board)); // Initialize board to 0 (empty)
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            charboard[i][j] = '.';
        }
    }
}

void display_board(int board[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] == 1)
                printf(" X ");
            else if (board[i][j] == 2)
                printf(" O ");
            else
                printf(" . ");
        }
        printf("\n");
    }
}

void sendBoard(struct sockaddr_in *player1, struct sockaddr_in *player2)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] == 0)
            {
                charboard[i][j] = '.';
            }
            else if (board[i][j] == 1)
            {
                charboard[i][j] = 'X';
            }
            else
            {
                charboard[i][j] = 'O';
            }
        }
    }

    char state[BUFFER_SIZE];
    memset(state, 0, sizeof(state));
    sprintf(state, "%c|%c|%c\n-----\n%c|%c|%c\n-----\n%c|%c|%c\n",
            charboard[0][0], charboard[0][1], charboard[0][2],
            charboard[1][0], charboard[1][1], charboard[1][2],
            charboard[2][0], charboard[2][1], charboard[2][2]);

    printf("%s", state);

    // Send the board state to both players
    sendto(serverFd, state, strlen(state), 0, (struct sockaddr *)player1, addr_len);
    sendto(serverFd, state, strlen(state), 0, (struct sockaddr *)player2, addr_len);
}

bool isWinner()
{
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == playerTurn && board[i][1] == playerTurn && board[i][2] == playerTurn)
            return true;
        if (board[0][i] == playerTurn && board[1][i] == playerTurn && board[2][i] == playerTurn)
            return true;
    }
    if (board[0][0] == playerTurn && board[1][1] == playerTurn && board[2][2] == playerTurn)
        return true;
    if (board[0][2] == playerTurn && board[1][1] == playerTurn && board[2][0] == playerTurn)
        return true;
    return false;
}

bool isDraw()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] == 0)
                return false;
        }
    }
    return true;
}

void gameLoop()
{
    bool invalidFlag = false;
    init_board();
    sendBoard(&player1_addr, &player2_addr);

    while (true)
    {
        struct sockaddr_in curr_player_addr;
        socklen_t curr_addr_len = addr_len;

        int curr_player_fd;
        if (playerTurn == 1)
        {
            curr_player_addr = player1_addr;
        }
        else if (playerTurn == 2)
        {
            curr_player_addr = player2_addr;
        }
        sleep(1);
        if (!invalidFlag)
        {
            sendto(serverFd, "turn", 5, 0, (struct sockaddr *)&curr_player_addr, curr_addr_len);
        }

        memset(buffer, 0, sizeof(buffer));
        int byte_recv = recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&curr_player_addr, &curr_addr_len);
        buffer[byte_recv] = '\0'; // Null-terminate received data

        sscanf(buffer, "%d %d", &row, &col);
        printf("Player %d entered: %d %d\n", playerTurn, row, col);

        // Check if the move is valid (empty cell)
        if (row > 0 && col > 0 && row <= 3 && col <= 3 && board[row - 1][col - 1] == 0)
        {
            board[row - 1][col - 1] = playerTurn;
            sendBoard(&player1_addr, &player2_addr);
            sleep(1);

            if (isWinner())
            {
                if (playerTurn == 1)
                {
                    sendto(serverFd, "win", 4, 0, (struct sockaddr *)&player1_addr, addr_len);
                    sendto(serverFd, "loser", 6, 0, (struct sockaddr *)&player2_addr, addr_len);
                }
                else
                {
                    sendto(serverFd, "win", 4, 0, (struct sockaddr *)&player2_addr, addr_len);
                    sendto(serverFd, "loser", 6, 0, (struct sockaddr *)&player1_addr, addr_len);
                }
                break;
            }

            if (isDraw())
            {
                sendto(serverFd, "draw", 5, 0, (struct sockaddr *)&player1_addr, addr_len);
                sendto(serverFd, "draw", 5, 0, (struct sockaddr *)&player2_addr, addr_len);
                break;
            }

            invalidFlag = false;
            playerTurn = (playerTurn == 1) ? 2 : 1;
        }
        else
        {
            sendto(serverFd, "invalid", 8, 0, (struct sockaddr *)&curr_player_addr, curr_addr_len);
            invalidFlag = true;
        }
    }
}

int main()
{
    printf("\t\t\t***TIC-TAC-TOE GAME SERVER (UDP)***\n\n\n");

    struct sockaddr_in server_addr;
    
    // Create server socket
    if ((serverFd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("ERROR: socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(serverFd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR: bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for players to connect...\n");

    // Wait for player 1
    recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&player1_addr, &addr_len);
    printf("%s\n",buffer);
    sendto(serverFd, "player 1 (X)", 13, 0, (struct sockaddr *)&player1_addr, addr_len);

    // Wait for player 2
    recvfrom(serverFd, buffer, sizeof(buffer), 0, (struct sockaddr *)&player2_addr, &addr_len);
    printf("%s\n",buffer);
    sendto(serverFd, "player 2 (O)", 13, 0, (struct sockaddr *)&player2_addr, addr_len);

    do
    {
        gameLoop();

        char playerResponse1[BUFFER_SIZE], playerResponse2[BUFFER_SIZE];

        int byte=recvfrom(serverFd, playerResponse1, sizeof(playerResponse1), 0, (struct sockaddr *)&player1_addr, &addr_len);
        playerResponse1[byte]='\0';
        printf("Player 1 said: %s %ld\n", playerResponse1,strlen(playerResponse1));
        byte=recvfrom(serverFd, playerResponse2, sizeof(playerResponse2), 0, (struct sockaddr *)&player2_addr, &addr_len);
        playerResponse2[byte]='\0';
        printf("Player 2 said: %s %ld\n", playerResponse2,strlen(playerResponse2));


        if (playerResponse1[0]=='n' || playerResponse2[0]=='n'){
              
            sendto(serverFd, "end", 4, 0, (struct sockaddr *)&player1_addr, addr_len);
            sendto(serverFd, "end", 4, 0, (struct sockaddr *)&player2_addr, addr_len);
            sleep(1);
            break;
        }
    } while (1);

    close(serverFd);
    return 0;
}
