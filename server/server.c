// Include all the libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>

// Define global variables
#define SERVER_PORT 8084
#define BUFFER_SIZE 1024
#define THREAD_POOL_SIZE 100
#define MAX_CURRENT_USERS 200
#define LENGTH_CODE_ROOM 6
#define MAX_PLAYER_PER_ROOM
// Welcome messages

// typedef Structs 
typedef struct sockaddr_in SA_IN; 
typedef struct sockaddr SA;

// Struct of GameState
typedef struct {
    /* data */
    int player1Y;
    int player2Y;
    int scorePlayer1;
    int scorePlayer2;
    int ballX;
    int ballY;
} GameState;

// Struct of players 
typedef struct {
    /* data */
    int clientSocket; 
} Player;

// Struct of Room
typedef struct {
    char code[LENGTH_CODE_ROOM];
    Player* player1;
    Player* player2; 
    GameState* gameState;
    pthread_t roomThread;
    pthread_mutex_t mutex; 
    pthread_cond_t cond;
} Room;

Room* rooms[THREAD_POOL_SIZE];
int numRooms = 0;

pthread_t thread_pool[THREAD_POOL_SIZE];

// Check function to use with the create socket, bind and listen.
int checkFunctions(int function, char* message);
// Function to handle all the incoming threads.
void* threadHandleFunction(void* args);
// Principal function.
void principalFunction(int clientSocket);
// Function to recieve messages from the client.
char* recieveFromClient(int clientSocket, int length_code);
// Function to proccess all that happen in the room
void* roomLogic(void* arg);

int main() {
    int serverSocket, clientSocket, addrSize; 
    SA_IN serverAddr, clientAddr;
    
    for(int i = 0; i < THREAD_POOL_SIZE; i++){
        rooms[i] = malloc(sizeof(Room)); // Asignas memoria para una nueva sala
        if (rooms[i] != NULL) {
            rooms[i]->player1 = NULL; // Inicializas player1 a NULL
            rooms[i]->player2 = NULL; // Inicializas player2 a NULL
            // ...
        } else {
            // Manejo de error si la asignación de memoria falla
            // ...
        }
    }
    // first off create the threads. We are looking for handle all the futures connections. 
    // for(int i = 0; i < THREAD_POOL_SIZE; i++){
    //     pthread_create(&thread_pool[i], NULL, threadHandleFunction, NULL); 
    // }
    // Create the TCP socket
    checkFunctions((serverSocket = socket(AF_INET, SOCK_STREAM, 0)), "Failed to create the socket!");

    // Initialize the address struct 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the IP and PORT
    checkFunctions((bind(serverSocket, (SA*)&serverAddr, sizeof(serverAddr))), "Bind Failed!");

    // Listen the incoming connections 
    checkFunctions((listen(serverSocket, MAX_CURRENT_USERS)), "Listen Failed!");
    printf("Listening on port %d\n", SERVER_PORT);
    addrSize = sizeof(SA_IN);

    // Accept the incoming connections
    while(1){
        printf("Waiting for connections...\n");
        //Waiting for, and eventually accept the incoming connections
        clientSocket = accept(serverSocket, (SA*)&clientAddr, (socklen_t*)&addrSize);
        checkFunctions(clientSocket, "Accept failed!");
        printf("Connected!\n");

        // Send the welcome message.
        principalFunction(clientSocket); 

    }
    
        // TODO
        // The player1 can create a room - He's gonna be the "Master"
        // When we create the room, the player2 has to enter the code room 
        // if the code room that the player entered is the same of the created room
        // The player2 enter to the player1 room
        // Here, they can establish they niicknames, etc etc etc
        // Now, with all established they can start the game
        // Here We can play, but how can we play?? 
        // We need to main sharing all the game information beetwen clients
            // 1. Here we need to pass the information with he respect header - and PDU 
            // 2. Now in the server, we need to parser the respect header - PDU information
            // 3. We need to refresh all the game objects 
            // 4. Share the information with the two clients 

    close(serverSocket);
    shutdown(serverSocket, SHUT_RDWR);

    return 0; 
}

// TODO
void* threadHandleFunction(void* args){
    return NULL;
}

// TODO
void principalFunction(int clientSocket){
    char* choice = recieveFromClient(clientSocket, 2);
    if (choice != NULL) {
        printf("%s\n", choice);
        if(choice[0] == '1') createRoom(clientSocket);
        else if (choice[0] == '2') joinRoom(clientSocket);
        else printf("La elección es diferente de 1");


        free(choice); // Libera la memoria cuando ya no necesites 'choice'
    } else {
        // Manejo de error si 'recieveFromClient' devuelve NULL
        // ... Realiza la gestión de errores adecuada ...
    }
    // if(strcmp(*choice, "1")) createRoom(clientSocket);
    // else if(strcmp(choice, "2") == 0) joinRoom(clientSocket, player);
    // else if(strcmp(choice, "3") == 0) RandomJoin(player);
}


// TODO
char* recieveFromClient(int clientSocket, int length_code) {
    char* buffer = malloc(length_code); // +1 para el carácter nulo
    if (buffer == NULL) {
        perror("Error: Memory in buffer");
        exit(1); // Maneja el error adecuadamente según tu aplicación
    }

    ssize_t bytes_received = recv(clientSocket, buffer, length_code, 0);
    if (bytes_received < 0) {
        perror("Error: Receiving data");
        free(buffer); // Liberar la memoria en caso de error
        return NULL;   // Maneja el error adecuadamente según tu aplicación
    }
    buffer[length_code - 1] = '\0'; // Agregamos el carácter nulo al final de la cadena
    return buffer;
}

// TODO
void createRoom(int clientSocket){ 
    // Logic to create a new room
    int foundRoom = -1; 
    for(int i =0; i < THREAD_POOL_SIZE; i++){
        if(rooms[i]->player1 == NULL && rooms[i]->player2 == NULL) {
            foundRoom = i; 
            break;
         }
    }
    if(foundRoom == - 1){
        const char* error_message = "Sorry, there are no more available rooms!";
        checkFunctions(send(clientSocket, error_message, strlen(error_message), 0), "Failed send!");
        close(clientSocket);
    } else { 
        char roomCode[LENGTH_CODE_ROOM];
        sprintf(roomCode, "ROO-%d", foundRoom);
        strcpy(rooms[foundRoom]->code, roomCode);
        Player* player1 = malloc(sizeof(Player)); 
        player1->clientSocket = clientSocket; 
        rooms[foundRoom]->player1 = player1;
        const char* success_message = "Success created room";
        checkFunctions(send(clientSocket, success_message, strlen(success_message), 0), "Send Failed!");
        printf("Found Room: %d\n", foundRoom);
        printf("Room code: %s\n", rooms[foundRoom]->code);
        printf("Client socket: %d\n", rooms[foundRoom]->player1->clientSocket);

        pthread_mutex_init(&rooms[foundRoom]->mutex, NULL);
        pthread_cond_init(&rooms[foundRoom]->mutex, NULL);
        pthread_create(&rooms[foundRoom]->roomThread, NULL, roomLogic, (void*)rooms[foundRoom]);
    }
}

void joinRoom(int clientSocket){
    // Receive the code room from the client
    char* code_room = recieveFromClient(clientSocket, LENGTH_CODE_ROOM);
    printf("Code room sent: %s\n", code_room);
    // Search the specific room with the code. 
    int foundRoom = -1; 
    for(int i = 0; i < THREAD_POOL_SIZE; i++){
        if(strcmp(code_room, rooms[i]->code) == 0){
            foundRoom = i;
            break;
        }
    }
    printf("Found room: %d\n", foundRoom); // DELETE THIS MESSAGE DEBUGGING
    // If the code room what not found we need to say that the code was invalidad
    if(foundRoom == -1) {
        const char* error_message = "Invalid code"; 
        checkFunctions(send(clientSocket, error_message, strlen(error_message), 0), "Failed send!");
        close(clientSocket); 
    // If the code was good, we assign the player to the specific toom, and 
    } else { 
        printf("Original code: %s\n", rooms[foundRoom]->code);
        Player* player2 = malloc(sizeof(Player));
        player2->clientSocket = clientSocket;
        rooms[foundRoom]->player2 = player2;
        const char* success_message = "Success join room";
        checkFunctions(send(clientSocket, success_message, strlen(success_message), 0), "Send Failed!");
        
        // DELETE MESSAGES - USE TO VERIFY THE ROOMS
        printf("Found room: %s\n", rooms[foundRoom]->code);
        printf("Client socket - Player1: %d\n", rooms[foundRoom]->player1->clientSocket);
        printf("Client socket - Player2: %d\n", rooms[foundRoom]->player2->clientSocket);

        pthread_cond_signal(&rooms[foundRoom]->cond);
        pthread_mutex_unlock(&rooms[foundRoom]->mutex);
    }
}

void* roomLogic(void* arg) {
    // Lógica de la sala de espera y el juego
    printf("WELCOME TO THE ROOM"); 
    Room* room = (Room*)arg;
    // Implementa la lógica de espera y el juego aquí
    char* message = "Waiting for the second player\n";
    pthread_mutex_lock(&room->mutex);
    while(room->player2 == NULL){
        printf("Player 2 CFD: %s", room->player2);
        send(room->player1->clientSocket, message, strlen(message), 0);
        pthread_cond_wait(&room->cond, &room->mutex);
    }
    pthread_mutex_unlock(&room->mutex);
    send(room->player1->clientSocket, "Hi player1", strlen("Hi player1"), 0);
    send(room->player2->clientSocket, "Hi player2", strlen("Hi player2"), 0);
    // Puedes usar room->player1 y room->player2 para interactuar con los jugadores

    destroyRoom(room);
}

int checkFunctions(int function, char* message){
    if(function < 0){
        perror(message);
        exit(1);
    }
    return 1;
}

void destroyRoom(Room* room) {
    pthread_mutex_destroy(&room->mutex);
    pthread_cond_destroy(&room->cond);
    pthread_exit(room);
}