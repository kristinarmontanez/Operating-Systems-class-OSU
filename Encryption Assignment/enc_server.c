/*
Name:           Kristina Montanez
Date:           Nov 29, 2021
Class:          CS344
Project:        Assignment 5 - enc_server.c
Description:    Program that will encrypt and decrypt plaintext into "ciphertext", using a 
                key, with modulo 27 operations-The 27 characters are the 26 capital letters, 
                and the space character. All 27 characters will be encrypted and decrypted.
                The program consists of 5 small programs in C. 2 programs function as servers,
                and 2 will be clients. The clients will use the servers to perform work, and 
                the 5th program is a standalone keygen file. 

                Example code provided for assignment: 
                "client.c"
                "server.c"
                "compileall.sh"
                "p5testscript"

__________________________________________________________________________________________

Include the usual headers from 
Assignments 1, 2, and 3, 4, and 
the new headers that were included
in this assignment's modules.
Include GNU option just in case.
*/
#define _GNU_SOURCE
#include <stdio.h>      
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>     
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>     
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h>   
#include <signal.h>   
#include <pthread.h>      
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
/*

______________________________________________________________________________________________
TABLE OF CONTENTS:

1) Error
2) setupAddressStruct
3) encryptMessage
4) main 
______________________________________________________________________________________________






______________________________________________________________________________________________
1)  Error
______________________________________________________________________________________________ 
Error function used for reporting issues. Used from "server.c"
*/
void error(const char *msg) 
    {perror(msg);
    exit(1);}






/*
______________________________________________________________________________________________
2)  setupAddressStruct
______________________________________________________________________________________________
Set up the address struct for the server socket. Used from "client.c"   
*/
void setupAddressStruct(struct sockaddr_in* address,
                        int portNumber)
    // Clear out the address struct
    {memset((char*) address, '\0', sizeof(*address));
    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);
    // Allow a client at any address to connect to this server
    address->sin_addr.s_addr = INADDR_ANY;}







/*
______________________________________________________________________________________________
2)  encryptMessage
______________________________________________________________________________________________
Creating the encryption. Used from "server.c"   
*/
char encryptMessage(char key, char buffer) 
    // Variables for our ASCII translations.
    {int key_ASCII;  
    int buffer_ASCII;

    // First, take care of our special characters, new line
    // and null terminator.
    if (buffer == '\0') return '\0';
    if (buffer == '\n') return '\n';
    

    // If our buffer characters show spaces, we convert them back to
    // our zero placeholders. Otherwise, we put them back where they 
    // belong in ASCII.
    if (buffer == ' ') buffer_ASCII = 0;
    else buffer_ASCII = buffer - 64;
    
    // Next, we check the key to see if there are spaces, and put the
    // spaces into "zero", our placeholder. Otherwise, we get the 
    // correct ASCII translation.
    if (key == ' ') key_ASCII = 0;
    else key_ASCII = key - 64;

    // Next, we make sure that the combination will be put into the 
    // original ASCII range for the uppercase letters and space.
    buffer_ASCII = (buffer_ASCII + key_ASCII) % 27;
    if (buffer_ASCII == 0) return ' ';
    else return buffer_ASCII + 64;}






/*
______________________________________________________________________________________________
4)  main
______________________________________________________________________________________________
Most of main's code is from "server.c"   
*/
int main(int argc, char *argv[])
    // Just like in example code, create our variables for the
    // socket connection, the characters to read, the buffers for 
    // the key and the text being read. 
    {int connectionSocket, charsRead;
    char key[256];
    char buffer[256];
    // Create our struct for the client's address, and server's address.
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t sizeOfClientInfo = sizeof(clientAddress);



    // Just like in the example code provided, check usage and args.
    if (argc < 2) 
        {fprintf(stderr,"USAGE: %s port\n", argv[0]);
        exit(1);}



    // Create the socket that will listen for connections
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) 
        {error("ERROR opening socket");}


  
    // Set up the address struct for the server socket
    setupAddressStruct(&serverAddress, atoi(argv[1]));



    // Associate the socket to the port
    if (bind(listenSocket,
            (struct sockaddr *)&serverAddress,
            sizeof(serverAddress)) < 0)
            {error("ERROR on binding");}
  


    // Start listening for connections. Allow up to 5 connections to queue up
    listen(listenSocket, 5);


    // We need to send a message to client, this way, 
    // the client can detect which server is trying to connect, 
    // and can immediately terminate the connection if needed.
    // https://edstem.org/us/courses/14269/discussion/881559
    char client_id[16] = "enc_client_id\n\0";
    char server_id[16] = "enc_server_id\n\0";

    // Set up our placeholder for current character, the number 
    // of our processes, and our line feed.
    char current;
    int processes = 0;
    int line;

     
    // Start our loop through each 5 active processes. 
    for (size_t connection = 0; connection < 4; connection++)
        // SpawnPid create a new process, and check whether it's 
        // -1, 0, or above. Just like in our Assignment 3.
        {pid_t spawnPid = fork();
        // If it's -1, we throw an error. 
        if (spawnPid == -1)
            {error("ERROR child process failed\n");
            exit(1);}

        if (spawnPid == 0)
            // Just like in the example code, create child process instructions
            // Accept a connection, blocking if one is not available until one connects
                {while(1)
                    // Accept the connection request which creates a connection socket
                    {connectionSocket = accept(listenSocket,
                        (struct sockaddr *)&clientAddress,
                        &sizeOfClientInfo);
                    if (connectionSocket < 0) 
                        {error("ERROR on accept");}

                    // Get the message from the client and display it
                    memset(buffer, '\0', 256);
                    // Read the client's message from the socket
                    charsRead = recv(connectionSocket, buffer, 255, 0);
                    if (charsRead < 0) 
                        {error("ERROR reading from socket");}

                    if (strcmp(client_id, buffer) == 0) 
                        // if the client's handshake is correct, 
                        // provide the encoding service.
                        {charsRead = send(connectionSocket,
                                        server_id, sizeof(server_id), 0);
                        if (charsRead < 0)
                            {error("ERROR writing to socket");}

                        // Provide the encoding until the end 
                        // of the message.
                        line = 0;
                        while (!line) 
                        // Grab the client's next message.
                            {memset(buffer, '\0', 256);
                            charsRead = recv(connectionSocket, buffer, 255, 0);
                            if (charsRead < 0) 
                                {error("ERROR reading from socket");}

                           // Send the server signal to client.
                            charsRead = send(connectionSocket,
                                            server_id, 
                                            strlen(server_id), 0);
                            if (charsRead < 0)
                                {error("ERROR writing to socket");}
        
                            // Grab the next message from the client.
                            memset(key, '\0', 256);
                            charsRead = recv(connectionSocket, key, 255, 0);
                            if (charsRead < 0) 
                                {error("ERROR reading from socket");}

                            // Encrypt for each character in the length 
                            // of the buffer.
                            for (size_t l = 0; l < strlen(buffer); l++) 
                                {current = encryptMessage(key[l], buffer[l]);
                                // If we hit a newline, add to line.
                                if (current == '\n') 
                                    {line = 1;}
                                // Reset our current placeholder.
                                buffer[l] = current;}

                            // Send our buffer to the client.
                            charsRead = send(connectionSocket,
                            buffer, 255, 0);
                            if (charsRead < 0) 
                                {error("ERROR writing to socket");}}}

                        else 
                            // Still send a message to the client, but
                            // DO NOT send the handshake signal, this way
                            // client knows it's incorrect.
                            {charsRead = send(connectionSocket, "error", 5, 0);
                            if (charsRead < 0)
                                {error("ERROR writing to socket");}}
                        // Close the connection socket for this client
                        close(connectionSocket);}
                    exit(1);}

            if (spawnPid < 0)
                // increment our processes for the total active count.
                {processes = processes + 1;
                if (processes == 4) 
                    // Accept a connection, blocking if one is not available until one connects
                    {while(1)
                        // Accept the connection request which creates a connection socket
                        {connectionSocket = accept(listenSocket,
                                                (struct sockaddr *)&clientAddress,
                                                &sizeOfClientInfo);
                        if (connectionSocket < 0) error("ERROR on accept");

                        // Get the message from the client and check it
                        memset(buffer, '\0', 256);
                        charsRead = recv(connectionSocket, buffer, 255, 0);
                        if (charsRead < 0) 
                            {error("ERROR reading from socket");}

                        if (strcmp(client_id, buffer) == 0) 
                            // Continue the decryption, and make sure to 
                            // provide your unique handshake signal.
                            {charsRead = send(connectionSocket,
                                            server_id, sizeof(server_id), 0);
                            if (charsRead < 0)
                                {error("ERROR writing to socket");}
                            
                            line = 0;
                            while (!line) 
                                // Get the next message datagram from the client
                                {memset(buffer, '\0', 256);
                                charsRead = recv(connectionSocket, buffer, 255, 0);
                                if (charsRead < 0) 
                                    {error("ERROR reading from socket");}

                                // Send the unique handshake signal
                                charsRead = send(connectionSocket,
                                server_id, strlen(server_id) + 1, 0);
                                if (charsRead < 0)
                                    {error("ERROR writing to socket");}
        
                                // Grab the next key message from the client.
                                memset(key, '\0', 256);
                                charsRead = recv(connectionSocket, key, 255, 0);
                                if (charsRead < 0) 
                                     // If there is nothing, throw error.
                                    {error("ERROR reading from socket");}

                                // Encrypt for each character in the length 
                                // of the buffer.
                                for (size_t l = 0; l < strlen(buffer); l++) 
                                    {current = encryptMessage(key[l], buffer[l]);
                                    // If we hit a newline, add to line.
                                    if (current == '\n') 
                                        {line = 1;}
                                    // Reset our current placeholder.
                                    buffer[l] = current;}
                                
                                // Send our buffer to the client.
                                charsRead = send(connectionSocket,
                                                buffer, 255, 0);
                                if (charsRead < 0) 
                                    {error("ERROR writing to socket");}}}
                                                
                            else 
                                // Still send a message to the client, but
                                // DO NOT send the handshake signal, this way
                                // client knows it's incorrect.
                                {charsRead = send(connectionSocket, "error", 5, 0);
                                if (charsRead < 0)
                                    {error("ERROR writing to socket");}}  

                            // Close the connection socket for this client
                            close(connectionSocket);}}}}
    // Close the listening socket
    close(listenSocket);
    return 0;}












