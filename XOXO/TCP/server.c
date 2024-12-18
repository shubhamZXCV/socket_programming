#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define MAXPLAYER 2
#define BUFFER_SIZE 1024

int board[3][3];
char charboard[3][3];
int playerTurn = 1;
int player1, player2;
char buffer[BUFFER_SIZE] = {0};
int row, col;

void gameLoop();

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

void sendBoard()
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

    send(player1, state, strlen(state), 0);
    send(player2, state, strlen(state), 0);
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
    bool invalidFlag=false;
    init_board();
    sendBoard();
    while (true)
    {
        
        int curr_player;

        if (playerTurn == 1)
        {
            curr_player = player1;
        }
        else if(playerTurn == 2 )
        {
            curr_player = player2;
        }
        sleep(1);
        if(!invalidFlag){

        send(curr_player, "turn", 5, 0);
        }
        memset(buffer, 0, sizeof(buffer));
        int byte_recv = recv(curr_player, buffer, sizeof(buffer), 0);
        buffer[byte_recv] = '\0';  // Null-terminate received data

        sscanf(buffer, "%d %d", &row, &col);
        printf("Player %d entered: %d %d\n", playerTurn, row, col);

        // Check if the move is valid (empty cell)
        if (row > 0 && col > 0 && row <= 3 && col <= 3 && board[row - 1][col - 1] == 0)
        {
            if (playerTurn == 1)
            {
                board[row - 1][col - 1] = 1;
            }
            else
            {
                board[row - 1][col - 1] = 2;
            }
           sendBoard();
           sleep(1);

            if (isWinner())
            {
                if (playerTurn == 1)
                {
                    send(player1, "win", 4, 0);
                    send(player2, "loser", 6, 0);
                }
                else
                {
                    send(player2, "win", 4, 0);
                    send(player1, "loser", 6, 0);
                }
                break;
            }

            if (isDraw())
            {
                send(player1, "draw", 5, 0);
                send(player2, "draw", 5, 0);
                break;
            }
            invalidFlag=false;
            playerTurn = (playerTurn == 1) ? 2 : 1;
        }
        else
        {
            send(curr_player, "invalid", 8, 0); // Invalid move, ask for input again
            invalidFlag=true;
        }
    }
}

int main()
{
    printf("\t\t\t***TIC-TAC-TOE GAME SERVER***\n\n\n");
    int serverFd;

    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating server socket
    if ((serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("ERROR: socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

   

    // Binding socket
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("ERROR: bind failed");
        exit(EXIT_FAILURE);
    }

    // Listening for connections
    if (listen(serverFd, MAXPLAYER + 1) < 0)
    {
        perror("ERROR: listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for players to connect...\n");

    // Accept player 1 connection
    if ((player1 = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("ERROR: accept failed");
        exit(EXIT_FAILURE);
    }
    printf(">>> Player 1 connected\n");
    send(player1, "player 1 (X)", 13, 0);

    // Accept player 2 connection
    if ((player2 = accept(serverFd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("ERROR: accept failed");
        exit(EXIT_FAILURE);
    }
    printf(">>> Player 2 connected\n");
    send(player2, "player 2 (O)", 13, 0);

    char playerResponse1[BUFFER_SIZE];
    char playerResponse2[BUFFER_SIZE];

    do
    {
        gameLoop();
        
        // Receive responses from both players
        playerResponse1[recv(player1, playerResponse1, sizeof(playerResponse1), 0)] = '\0';
        playerResponse2[recv(player2, playerResponse2, sizeof(playerResponse2), 0)] = '\0';
        
        printf("Player 1 said: %s\n", playerResponse1);
        printf("Player 2 said: %s\n", playerResponse2);

        if (!strcmp(playerResponse1, "y") && !strcmp(playerResponse2, "y"))
        {
            printf("Game is restarting...\n\n");
            continue;
        }
        else
        {
            send(player1, "end", 4, 0);
            send(player2, "end", 4, 0);
            break;
        }

    } while (true);

    close(player1);
    close(player2);
    close(serverFd);
    return 0;
}
