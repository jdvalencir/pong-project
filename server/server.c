#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>


#define PORT 8080
#define BUFFER_SIZE 1024

struct GameState {
    int player1Y;
    int player2Y;
    int ballX;
    int ballY;
};

void* handleClient(void* args){
    int clientSocket = *(int*) args; 
    int read_size;
    char *message, buffer[BUFFER_SIZE];
    // struct GameState gameState;

    // Enviar una señal al cliente para indicar que el servidor está listo
    write(clientSocket, "READY", strlen("READY"));
    
    while ((read_size = recv(clientSocket, buffer, BUFFER_SIZE, 0) > 0)) {
        
        // Procese the message from the client and update the game state
        if (strcmp(buffer, "UP") == 0) {
            // Move the player up
            printf("UP\n");
            write(clientSocket, "UP", strlen("UP"));
            memset(buffer, 0, BUFFER_SIZE);
        } else if (strcmp(buffer, "DOWN") == 0) {
            // Move the player down
            printf("DOWN\n");
            write(clientSocket, "MOVE DOWN", strlen("MOVE DOWN"));
            memset(buffer, 0, BUFFER_SIZE);
        } else if (strcmp(buffer, "LEFT") == 0) {
            // Move the player left
            printf("LEFT\n");
            write(clientSocket, "LEFT", strlen("LEFT"));
            memset(buffer, 0, BUFFER_SIZE);
        } else if (strcmp(buffer, "RIGHT") == 0) {
            // Move the player right
            printf("RIGHT\n");
            write(clientSocket, "RIGHT", strlen("RIGHT"));
            memset(buffer, 0, BUFFER_SIZE);
        } else if (strcmp(buffer, "START") == 0) {
            // Start the game
            printf("START\n");
            write(clientSocket, "START", strlen("START"));
            memset(buffer, 0, BUFFER_SIZE);
        } else if (strcmp(buffer, "STOP") == 0) {
            // Stop the game
            printf("STOP\n");
            write(clientSocket, "STOP", strlen("STOP"));
            memset(buffer, 0, BUFFER_SIZE);
        } else if (strcmp(buffer, "QUIT") == 0) {
            // Quit the game
            printf("QUIT\n");
            write(clientSocket, "QUIT", strlen("QUIT"));
            memset(buffer, 0, BUFFER_SIZE);
        } else {
            // Invalid command
            printf("Invalid command\n");
            write(clientSocket, "INVALID", strlen("INVALID"));
            memset(buffer, 0, BUFFER_SIZE);
        }
    }
    if(read_size == 0) {
        puts("Client disconnected");
        fflush(stdout);
    } else if(read_size == -1) {
        perror("recv failed");
    }
    close(clientSocket);
    pthread_exit(NULL);
}

int main() {
    int serverSocket, clientSocket, c; 
    int opt = 1;
    struct sockaddr_in serverAddr, client;

    // Create the TCP socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket < 0) {
        perror("Error al crear el socket");
        exit(1);
    }
    if (setsockopt(serverSocket, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    // Bind the socket to the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error al hacer bind");
        exit(1);
    }
    
    // Listen the incoming connections
    if(listen(serverSocket, 10) < 0){
        perror("Error al escuchar");
        exit(1);
    }

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);


    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id; 

    printf("Listening on port %d\n", PORT);
    // Accept the incoming connections
    while((clientSocket = accept(serverSocket, (struct sockaddr*)&client, (socklen_t*)&c))){
        puts("Connection accepted");

        // Create a thread to handle the client
        if(pthread_create(&thread_id, NULL, handleClient, (void*) &clientSocket) < 0){
            perror("could not create thread");
            return 1;
        }

        puts("Handler assigned");
    }

    close(serverSocket);
    return 0; 
}