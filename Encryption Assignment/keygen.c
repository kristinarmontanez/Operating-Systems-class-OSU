/*
Name:           Kristina Montanez
Date:           Nov 29, 2021
Class:          CS344
Project:        Assignment 5 - keygen.c
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



/*
______________________________________________________________________________________________
MAIN FUNCTION
______________________________________________________________________________________________   
*/
int main(int argc, char* argv[]) 
    // First, catch invalid inputs for running keygen.
    {if (argc < 2)
        // If the arguments given do not contain the length of the keygen, 
        // throw standard error message. 
        {fprintf(stderr,"USAGE: %s keylength\n", argv[0]);
        exit(0);}

    // If we have two parameters from the user, grab the length of the 
    // requested keygen.
    int length = atoi(argv[1]);

    if (length <= -1)
        // If the arguments given contain a negative length for the keygen, 
        // throw standard error message.
        {fprintf(stderr,"Please provide a positive number for keygen.\n");
        exit(0);}

    // Set up our variable for key characters.
    int key_char;

    // Set up our key string.
    char key[length + 2];

    // Set up our loop.
    int i;
    
    // Generate the random key with 27 allowed characters.
    // Start a rand() seed.
    srand(time(0));

    // Loop through each index of the length specified. 
    for (i = 0; i < length; i++) 
        // Just like the assignment states "using modulo 27 operations."
        {key_char = rand() % 27;
        // If the char is 0- meaning we will normally get ASCII "@" if
        // we offset by 64,then offset it with the space ASCII 32 for "space".
        if (key_char == 0) 
            {key_char = 32;}
        else 
            // If it's 1-26, offest with 64 (1+64= ASCII "A").
            {key_char = key_char + 64;}
        // Add the generated char to our string.
        key[i] = key_char;}
    // Make sure to add the null terminator at the end.
    key[length] = '\0';
    // Print out the generated key.
    printf("%s\n", key);
    // Exit program.
    return EXIT_SUCCESS;}