/*
Name:           Kristina Montanez
Date:           Nov 29, 2021
Class:          CS344
Project:        Assignment 5 - dec_client.c
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
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
/*

______________________________________________________________________________________________
TABLE OF CONTENTS:

1) Error
2) setupAddressStruct
3) main 
______________________________________________________________________________________________






______________________________________________________________________________________________
1)  Error
______________________________________________________________________________________________ 
Error function used for reporting issues. Used from "client.c"
*/
void error(const char *msg) 
    {perror(msg);
    exit(0);}


/*
______________________________________________________________________________________________
2)  setupAddressStruct
______________________________________________________________________________________________
Set up the address struct. Used from "client.c"   
*/
void setupAddressStruct(struct sockaddr_in* address,
                        int portNumber,
                        char* hostname)

    // Clear out the address struct
    {memset((char*) address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) 
        {fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);}

    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*) &address->sin_addr.s_addr,
                    hostInfo->h_addr_list[0],
                    hostInfo->h_length);}



/*
______________________________________________________________________________________________
3)  main
______________________________________________________________________________________________
Set up the address struct. Most of main's code is from "client.c"   
*/
int main(int argc, char *argv[]) 
    {int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char buffer[256];
    char key[256];

    

    // Check usage & args
    if (argc < 4) 
        {fprintf(stderr,"USAGE: %s plaintext key port\n", argv[0]);
        exit(0);}

    // open file descriptors for plaintext and key
    int compare_plaintext = open(argv[1], O_RDONLY);
    int compare_key = open(argv[2], O_RDONLY);



    // Create a struct to compare what the text.
    struct stat check_plaintext;
    fstat(compare_plaintext, &check_plaintext);


    // Create a struct to compare our key.
    struct stat check_key;
    fstat(compare_key, &check_key);
  

    // If the key is not long enough to match, throw error.
    if (check_plaintext.st_size > check_key.st_size) 
        {fprintf(stderr, "Error: key \'%s\' is too short\n", argv[2]);
        exit(1);}

    // As shown in example code, create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0)
        {error("CLIENT: ERROR opening socket");}

    // Set up the server address struct
    setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
        {error("CLIENT: ERROR connecting");}




    // We need to send a message to client, this way, 
    // the client can detect which server is trying to connect, 
    // and can immediately terminate the connection if needed.
    // https://edstem.org/us/courses/14269/discussion/881559
    char client_id[16] = "dec_client_id\n\0";
    // We then send the initial handshake to the server.
    charsWritten = send(socketFD, client_id, strlen(client_id), 0);
    if (charsWritten < 0)
        {error("CLIENT: ERROR writing to socket");}
    



    // Next, recieve the message from the server
    // Clear out the buffer again for reuse
    memset(buffer, '\0', 256);
    // Read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, 255, 0);
    if (charsRead < 0) 
        {error("CLIENT: ERROR reading from socket");}

    // Set up our counters for checked characters and running total.
    int checked_chars;
    int total_read;

    // SET up our temp string to use for filling from reading in input,
    // And our signal from the client that we are the correct server.
    char temp_filler[256];
    char server_id[16] = "dec_server_id\n\0";
    
    // Variable for our characters lines read.
    int line;
    // If we get the correct handshake, then proceed.
    if (strcmp(server_id, buffer) == 0) 
        // Read in our key characters.
        {memset(key, '\0', 256);
        charsRead = read(compare_key, key, 254);
        if (charsRead < 0) 
            {error("CLIENT: ERROR reading from key");}
        // Read in our plaintext characters. 
        memset(buffer, '\0', 256);
        charsRead = read(compare_plaintext, buffer, 254);
        // Reset our total characters read.
        total_read = charsRead;




        // While we still have characters to read in, check characters 
        // for valid input.
        while (charsRead > 0)
            // check for input validity 
            {memset(temp_filler, '\0', 256);
            checked_chars = pread(compare_plaintext, temp_filler, 255, total_read);
            if (temp_filler[0] == '\0')
                // if the line is new, do not add. 
                {line = 0;}
            else 
                // Add 1 so we have correct determination of newline feed char "10".
                {line = 1;}
            int current;
            for (size_t each_char = 0; each_char < strlen(buffer); each_char++) 
                {current = buffer[each_char];
                // Check if our characters are within the correct ASCII range.
                if (current < 91 && current > 64) 
                    {continue;}
                // if our character is a new line feed, proceed.
                if (current == 10) 
                    {if (!line && each_char == strlen(buffer) - 1) 
                        {continue;}}
                // if our character is the space bar, proceed.
                if (current == 32) 
                    {continue;}
                // else, we have bad characters.
                else
                    fprintf(stderr, "dec_client error: input contains bad characters\n" );
                    exit(1);}



            // Just as the example code shows, send buffer to server
            // Once we have checked all the characters, we can send the message.
            charsWritten = send(socketFD, buffer, strlen(buffer), 0);
            if (charsWritten < 0) 
                // if there are no characters, throw error.
                {error("CLIENT: ERROR writing to socket");}
            // Clear the buffer to recieve next from socketFD.
            memset(buffer, '\0', 256);
            charsRead = recv(socketFD, buffer, 255, 0);
            if (charsRead < 0) 
                {error("CLIENT: ERROR reading from socket");}
            // Check our server to make sure it's the correct one.
            if (strcmp(buffer, server_id) != 0) 
                {error("CLIENT: ERROR failed to connect to dec_server");}





            // Just as the example code shows, send key to server
            // Write to the server
            charsWritten = send(socketFD, key, strlen(key), 0);
            if (charsWritten < 0) 
                {error("CLIENT: ERROR writing to socket");}
            // Once we check characters, read the message.
            charsRead = recv(socketFD, buffer, 255, 0);
            if (charsRead < 0) 
                {error("CLIENT: ERROR reading from socket");}
            // If we have the buffer recieved, print it out.
            fprintf(stdout, "%s", buffer);




            // Clear out the buffers.
            // Clear our key buffer and start over.
            memset(key, '\0', 256);
            charsRead = read(compare_key, key, 255);
            if (charsRead < 0) 
                {error("CLIENT: ERROR reading from key");}
            // Clear our buffer and start the next plaintext reading.
            memset(buffer, '\0', 256);
            charsRead = read(compare_plaintext, buffer, 255);



            // Tack on the characters to our total running count.
            total_read = total_read + charsRead;}
        // As the example code shows, if plaintext was not read, throw error. 
        if (charsRead < 0) 
            {error("CLIENT: ERROR reading from plaintext");}}
    else 
        {fprintf(stderr, "Error: could not contact dec_server on port %s\n", argv[3]);}
    // Close the socket
    close(socketFD);
    return 0;}