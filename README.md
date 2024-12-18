# Tic Tac Toe: Socket Programming

This repository contains a Tic Tac Toe game implemented using socket programming in both TCP and UDP protocols. The project demonstrates the fundamentals of inter-process communication and networking using sockets.

## Features
- Two-player Tic Tac Toe game
- Implementation using both TCP and UDP protocols
- Real-time communication between client and server

## Getting Started

### Prerequisites
- GCC compiler
- Linux-based operating system (or an environment supporting socket programming)
- Basic understanding of socket programming

### Directory Structure
```
XOXO
├── TCP
│   ├── server.c
│   ├── client.c
├── UDP
    ├── server.c
    ├── client.c
```

## Compilation and Execution Guidelines

### TCP Protocol
1. Navigate to the `XOXO` directory:
   ```bash
   cd XOXO
   ```
2. Navigate to the `TCP` directory:
   ```bash
   cd TCP
   ```
3. Compile the server and client programs:
   ```bash
   gcc server.c -o server
   gcc client.c -o client
   ```
4. Open three terminal windows:
   - In the first terminal, run the server:
     ```bash
     ./server
     ```
   - In the second and third terminals, run the clients:
     ```bash
     ./client
     ```

### UDP Protocol
1. Navigate to the `XOXO` directory:
   ```bash
   cd XOXO
   ```
2. Navigate to the `UDP` directory:
   ```bash
   cd UDP
   ```
3. Compile the server and client programs:
   ```bash
   gcc server.c -o server
   gcc client.c -o client
   ```
4. Open three terminal windows:
   - In the first terminal, run the server:
     ```bash
     ./server
     ```
   - In the second and third terminals, run the clients:
     ```bash
     ./client
     ```

## How to Play
1. The server will initialize the game and manage the communication between two clients.
2. Players take turns to mark a cell in the Tic Tac Toe grid.
3. The first player to align three marks (horizontally, vertically, or diagonally) wins.
4. The server declares the winner or a draw at the end of the game.

## Notes
- Ensure that all terminals are connected to the same network for the game to function correctly.
- The UDP implementation does not guarantee message delivery, which may affect gameplay in unreliable networks.

<hr>


# Simulating TCP Functionality Using UDP

This project implements a simplified version of the TCP protocol using UDP sockets. The focus is on mimicking the acknowledgment (ACK) and sequence number (SEQ) functionalities of TCP over UDP, which is inherently connectionless.

## Features
- Implements TCP-like acknowledgment (ACK) and sequence number (SEQ) handling
- Reliable communication simulated over UDP
- Demonstrates key principles of TCP over a connectionless protocol

## Getting Started

### Prerequisites
- GCC compiler
- Linux-based operating system (or an environment supporting socket programming)
- Basic understanding of socket programming and TCP/UDP protocols

### Directory Structure
```
TCP
├── server.c
├── client.c
├── tcp.c
├── tcp.h
```

## Compilation and Execution Guidelines

1. Navigate to the `TCP` directory:
   ```bash
   cd TCP
   ```
2. Compile the server and client programs:
   ```bash
   gcc server.c tcp.c -o server
   gcc client.c tcp.c -o client
   ```
3. Open two terminal windows:
   - In the first terminal, run the server:
     ```bash
     ./server
     ```
   - In the second terminal, run the client:
     ```bash
     ./client
     ```

## How It Works
1. **Server and Client Communication**:
   - The server listens for incoming messages from the client over a UDP socket.
   - The client sends data packets with sequence numbers to the server.

2. **ACK and SEQ Simulation**:
   - The server acknowledges received packets by sending an ACK back to the client.
   - Lost or out-of-order packets are detected, and retransmissions are triggered as needed.

3. **Reliable Data Transfer**:
   - By implementing ACK and SEQ functionality, the project ensures data reliability similar to TCP.

## Notes
- This project is for educational purposes and does not implement all aspects of TCP (e.g., congestion control, windowing).
- Ensure the server and client are connected to the same network for proper functionality.



