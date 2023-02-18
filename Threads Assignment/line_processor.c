/*
    Name:           Kristina Montanez
    Date:           Nov 15, 2021
    Class:          CS344
    Project:        Assignment 4
    Description:    Program that will create 4 threads to take input and read in lines of 1000
                    characters max, 50 lines max from the standard input, call a second thread
                    for replacing line separators with spaces, a third thread to replace "++"
                    with "^", and a fourth thread to write the output to lines of 80 characters.
                    The 4 threads communicate with each other using the "Producer-Consumer"
                    approach. Input will only consist of ASCII characters (space-32 to tilde-126)
                    and the program will stop when only characters in the line are "STOP" followed
                    by the line separator. 

                    Example code provided for assignment: 
                    "6_5_prod_cons_pipeline.c"
______________________________________________________________________________________________

Include the usual headers from 
Assignments 1, 2, and 3, and 
the new headers that were included
in this assignment's modules, including
<pthread.h> for the pthread API shown 
in materials.
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
/*

______________________________________________________________________________________________
TABLE OF CONTENTS:

1) GLOBAL VARIABLES, FUNCTION DECLARATIONS, & BUFFERS:

    1.1) Definitions for the max requirements       
    1.2) Function Declarations         
    1.3) Buffers

2) COMMAND LINE INPUT:

    2.1) get_input()            - Function that the input thread will run.
    2.2) get_user_input()       - Function to get user input.
    2.3) write_output()         - Function that the output thread will run.

3)  GET BUFFER FUNCTIONS:

    3.1) get_buff_1()           - Function that gets the next item from buffer 1.
    3.2) get_buff_2()           - Function that gets the next item from buffer 2.
    3.3) get_buff_3()           - Function that gets the next item from buffer 3.

4)  PUT BUFFER FUNCTIONS:

    4.1) put_buff_1()           - Function that places a string in buffer 1.
    4.2) put_buff_2()           - Function that places a string in buffer 2.
    4.3) put_buff_3()           - Function that places a string in buffer 3.

5)  REPLACE FUNCTIONS:

    5.1) stop_processing()      - Function to check if input is the stop processing "STOP".
    5.2) line_separator()       - Function to replace each new line with a space character.
    5.3) plus_sign()            - Function to replace each pair of "++" with "^".

6)  MAIN FUNCTION:

    6.1) Thread Creation       
    6.2) Thread Completion         
    6.3) Exit Progam 
______________________________________________________________________________________________






______________________________________________________________________________________________
1)  GLOBAL VARIABLES, FUNCTION DECLARATIONS, & BUFFERS
______________________________________________________________________________________________
1.1) Definitions for the max requirements       
1.2) Function Declarations         
1.3) Buffers  
*/


/* 1.1) Definitions for the max requirements.
___________________________________________________________________
*/
// Size of the buffers
#define SIZE 50
// As the assignment states, the buffers hold "50 lines of 1000 
// characters each, you can model the problem as Producer-Consumer 
// with unbounded buffers."
#define NUM_ITEMS 1000




/* 1.2) Function Declarations.
___________________________________________________________________
*/
// Command Line Functions
void *get_input(void *args);
char* get_user_input();
void *write_output(void *args);

// Get Buffer Functions
char* get_buff_1();
char* get_buff_2();
char* get_buff_3();

// Put Buffer Functions
void put_buff_1(char* item);
void put_buff_2(char* item);
void put_buff_3(char* item);

// Replace Functions
int stop_processing(char* item);
void *line_separator(void *args);
void* plus_sign();





/* 1.3) Buffers.
___________________________________________________________________
*/

// BUFFER 1: INPUT TO LINE SEPARATOR:
// As shown in the example code given from the assignment, 
// Buffer 1, will share resource between input thread and 
// Line Separator Thread.
char buffer_1[SIZE][NUM_ITEMS];
// Number of items in the buffer
int count_1 = 0;
// Index where the Input Thread will put the next item
int prod_idx_1 = 0;
// Index where the Line Separator Thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;




// BUFFER 2: LINE SEPARATOR TO PLUS SIGN:
// As shown in the example code from the assignment, 
// Buffer 2 will share resource between Line Separator thread
// and Plus Sign Thread.
char buffer_2[SIZE][NUM_ITEMS];
// Number of items in the buffer
int count_2 = 0;
// Index where the Line Separator Thread will put the next item
int prod_idx_2 = 0;
// Index where the Plus Sign Thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;




// BUFFER 3: PLUS SIGN TO OUTPUT:
// As shown in the example code from the assignment, 
// Buffer 3 will share resource between Plus SIgn Thread 
// and Output Thread.
char buffer_3[SIZE][NUM_ITEMS];
// Number of items in the buffer
int count_3 = 0;
// Index where the Plus Sign Thread will put the next item
int prod_idx_3 = 0;
// Index where the Output Thread will pick up the next item
int con_idx_3 = 0;
// Initialize the mutex for buffer 3
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 3
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;








/*
______________________________________________________________________________________________
2)  COMMAND LINE INPUT
______________________________________________________________________________________________
2.1) get_input()            - Function that the input thread will run.
2.2) get_user_input()       - Function to get user input.
2.3) write_output()         - Function that the output thread will run.
*/





/* 2.1)_____________________________________________________________ 
Function that the input thread will run. The majority of the code in
this function is from the example code. The function is called from 
main() for pthread just as the example code, checks for the stop 
processing line.
*/
void *get_input(void *args)
    // Keep getting the input until the stop processing line
    // comes up.
    {while (1) 
        // Get the user input.
        {char* item = get_user_input();
        put_buff_1(item);
        // Compare each input with the stop processing line.
        if (stop_processing(item) == 1) 
            {break;}}
    return NULL;}






/* 2.2)_____________________________________________________________ 
Function to get user input and check if return value is an empty
line. The function is called from get_input() just as the example
code, and returns the user's input value back to get_input(). Just
like the example code, this fuction does not do error checking. As
stated in the assignment, input will only consist of ASCII characters 
(space-32 to tilde-126), and program does not need to check for 
correctness.
*/
char* get_user_input()
    // Just as in example code, create variable for 
    // returning value. 
    {char* value;
    // Make room for maximum characters plus null terminator.
    size_t char_per_line = NUM_ITEMS + 1;
    // Make variable for comparing string to an empty line.
    char *empty_line = NULL;
    value = calloc(char_per_line, sizeof(char));
    // Get the user's input value.
    getline(&empty_line, &char_per_line , stdin);
    // Check if the line was empty.
    strcpy(value,empty_line);
    if(strcmp(value, "\n") == 0 )
        {value = " ";}
    // Return the value back to get_input().
    return value;}





/* 2.3)_____________________________________________________________ 
Function that the output thread will run. Much of the code in 
this function is from the example code. The function is called from 
main() for pthread. Grabs item from buffer shared with the plus_sign()
thread, and prints the items.
*/
void *write_output(void *args)
    // Create variable for output printing.
    {char* item;
    // Make room for maximum characters plus null terminator.
    size_t char_per_line = NUM_ITEMS + 1;
    char* output = calloc(sizeof(char_per_line), sizeof(char));
    // Create variables for the count and full-line count.
    int count;
    int full_line = 80;
    while (1) 
        // Just like in the example code, grab buffer 3 for 
        // printing out.
        {item = get_buff_3();
        int buff_count;
        for (buff_count = 0; buff_count < strlen(item); buff_count++)
            //Place characters from buffer to variable.
            {output[count] = item[buff_count];  
            // Update our count to make sure we do not 
            // go over a full line.  
            count++;
            // If we reach our full line amount, print the 
            // line and reset our counter.
            if(count == full_line) 
                {int print_count;
                  for (print_count = 0; print_count < full_line; print_count++) 
                    // Print each one of the characters
                    // in the full line.
                    {printf("%c", output[print_count]);}
                // Print our line return and reset the count.
                printf("\n");
                count = 0;}}
            // Check to make sure we are not printing the 
            // stop processing at the end. 
            if (stop_processing(item) == 2) 
                {break;}}
        return NULL;}







/*
______________________________________________________________________________________________
3)  GET BUFFER FUNCTIONS
______________________________________________________________________________________________
3.1) get_buff_1()           - Function that gets the next item from buffer 1.
3.2) get_buff_2()           - Function that gets the next item from buffer 2.
3.3) get_buff_3()           - Function that gets the next item from buffer 3.
*/




/* 3.1)_____________________________________________________________ 
Function that gets the next item from buffer 1. The majority of the 
code in this function is from the example code. The function is 
called from line_separator() and returns buffer 1 index.
*/
char* get_buff_1()
    // Lock the mutex before checking if the buffer has data.
    {pthread_mutex_lock(&mutex_1);
    while (count_1 == 0)
        // Buffer is empty. Wait for the producer to signal that the 
        // buffer has data.
        pthread_cond_wait(&full_1, &mutex_1);
        char* item = buffer_1[con_idx_1];
        // Increment the index from which the item will be picked up.
        con_idx_1 = con_idx_1 + 1;
        count_1--;
        // Unlock the mutex.
        pthread_mutex_unlock(&mutex_1);
        // Return the item.
        return item;}




/* 3.2)_____________________________________________________________ 
Function to get the next item from buffer 2. The majority 
of the code in this function is from the example code. The function is 
called from plus_sign() and returns buffer 2 index.
*/
char* get_buff_2()
    // Lock the mutex before checking if the buffer has data.
    {pthread_mutex_lock(&mutex_2);
    while (count_2 == 0)
        // Buffer is empty. Wait for the producer to signal that the 
        // buffer has data.
        pthread_cond_wait(&full_2, &mutex_2);
        char* item = buffer_2[con_idx_2];
        // Increment the index from which the item will be picked up.
        con_idx_2 = con_idx_2 + 1;
        count_2--;
        // Unlock the mutex.
        pthread_mutex_unlock(&mutex_2);
        // Return the item.
        return item;}




/* 3.3)_____________________________________________________________ 
Function that gets the next item from buffer 3. The majority of the 
code in this function is from the example code, and is repeated from
get_buff_1/get_buff_2. The function is called from write_output() 
and returns buffer 3 index.
*/
char* get_buff_3()
    // Lock the mutex before checking if the buffer has data.
    {pthread_mutex_lock(&mutex_3);
    while (count_3 == 0)
    // Buffer is empty. Wait for the producer to signal that 
    // the buffer has data.
    pthread_cond_wait(&full_3, &mutex_3);
    char* item = buffer_3[con_idx_3];
    // Increment the index from which the item will be picked up.
    con_idx_3 = con_idx_3 + 1;
    count_3--;
    // Unlock the mutex.
    pthread_mutex_unlock(&mutex_3);
    // Return the item.
    return item;}





/*
______________________________________________________________________________________________
4)  PUT BUFFER FUNCTIONS
______________________________________________________________________________________________
4.1) put_buff_1()           - Function that places a string in buffer 1.
4.2) put_buff_2()           - Function that places a string in buffer 2.
4.3) put_buff_3()           - Function that places a string in buffer 3.
*/


/* 4.1)_____________________________________________________________ 
Function to place a string in buffer 1. The majority of the code in
this function is from the example code. The function is called from 
get_input() just as the example code, and signals to the consumer -
"line_separator()" that the buffer is not empty.
*/
void put_buff_1(char* item)
    // Lock the mutex before putting the item in the buffer.
    {pthread_mutex_lock(&mutex_1);
    // Put the item in the buffer.
    strcpy(buffer_1[prod_idx_1], item);
    // Increment the index where the next item will be put.
    prod_idx_1 = prod_idx_1 + 1;
    count_1++;
    // Signal to the consumer that the buffer is no longer empty.
    pthread_cond_signal(&full_1);
    // Unlock the mutex.
    pthread_mutex_unlock(&mutex_1);}






/* 4.2)_____________________________________________________________ 
Function that places a string in buffer 2. The majority of the 
code in this function is from the example code. The function is 
called from line_separator() and and signals to the consumer -
"plus_sign()" that the buffer is not empty.
*/
void put_buff_2(char* item)
    // Lock the mutex before putting the item in the buffer.
    {pthread_mutex_lock(&mutex_2);
    // Put the item in the buffer.
    strcpy(buffer_2[prod_idx_2], item);
    // Increment the index where the next item will be put.
    prod_idx_2 = prod_idx_2 + 1;
    count_2++;
    // Signal to the consumer that the buffer is no longer empty.
    pthread_cond_signal(&full_2);
    // Unlock the mutex.
    pthread_mutex_unlock(&mutex_2);}




/* 4.3)_____________________________________________________________ 
Function that places a string in buffer 3. The majority of the 
code in this function is from the example code and is repeated from
put_buff_1/put_buff_2. The function is called from plus_sign() and 
signals to the consumer - "write_output()" that the buffer is not empty.
*/
void put_buff_3(char* item)
    // Lock the mutex before putting the item in the buffer.
    {pthread_mutex_lock(&mutex_3);
    // Put the item in the buffer.
    strcpy(buffer_3[prod_idx_3], item);
    // Increment the index where the next item will be put.
    prod_idx_3 = prod_idx_3 + 1;
    count_3++;
    // Signal to the consumer that the buffer is no longer empty.
    pthread_cond_signal(&full_3);
    // Unlock the mutex.
    pthread_mutex_unlock(&mutex_3);}







/*
______________________________________________________________________________________________
5)  REPLACE FUNCTIONS
______________________________________________________________________________________________
5.1) stop_processing()      - Function to check if input is the stop processing "STOP".
5.2) line_separator()       - Function to replace each new line with a space character.
5.3) plus_sign()            - Function to replace each pair of "++" with "^".
*/



/* 5.1)_____________________________________________________________ 
Function to check if input is the stop processing "STOP". The function 
is called from line_separator(), plus_sign(), and write_out(). Returns
1 if the input matches the stop processing phrase for the input string, 
2 if the input matches the stop processing for line separators, plus 
sign and output, and 0 if it does not match any.
*/
int stop_processing(char* item)
    // Check for the user input to see if it's "STOP" only.
    {if (strncmp(item, "STOP\n", strlen("STOP\n")) == 0) 
        {return 1;}
    // For subsequent checks such as line separator, plus sign,
    // and write output, check for "STOP " with space so that 
    // it will break beforehand.
    if (strncmp(item, "STOP ", strlen("STOP ")) == 0) 
        {return 2;}
    // If "STOP" or "STOP " is not matched, return zero/false.
    else 
        {return 0;}}





/* 5.2)_____________________________________________________________ 
Function to replace each new line with a space character. Much of 
of the code in this function is from the example code. The function is 
called from main() for pthread and does not return a value.
*/
void *line_separator(void *args)
    // Create variable for comparing chars.
    {char* item;
    while (1) 
        {int count = 0;
        // Just like in the example code, get buffer 1 to compare
        // for new lines in order to replace with space char.
        item = get_buff_1();
        while(count != strlen(item))
            {if(item[count] == '\n')
                {item[count] = ' ';}
            count++;}
        // Once we are done checking and replacing, put buffer forward.
        put_buff_2(item);
        // Check if the stop processing "STOP" occurs.
        if (stop_processing(item) == 2) 
            {break;}}
        return NULL;}





/* 5.3)_____________________________________________________________ 
Function to replace each pair of "++" with "^". Much of the code in 
this function is from the example code. The function is called from 
main() for pthread and does not return a value.
*/
void* plus_sign()
    // Just like in the example code, create a variable to hold 
    // the buffer.
    {char* item;
    while (1) 
        // create our buffer length counter and the variable to hold
        // the next character for comparing "++".
        {int count = 0;
        int next;
        // Just like in teh example code, grab our buffer and 
        // get the length. 
        item = get_buff_2();
        int buffer_length = strlen(item);
        while(count != buffer_length)
            // If the character and the "next" character after it
            // are both "++", change to "^".
            {if((item[count] == '+') && (item[count + 1] == '+'))
                {item[count] = '^'; 
                // Move our count forward.     
                next = count + 1;
                while(next != buffer_length)
                    // Shift our "next" variable forward
                    // by one character with a placeholder.
                    {char placeholder;
                    placeholder = item[next + 1];
                    item[next] = placeholder;
                    next++;}
                // At the very end of our loop, place a new line 
                // after our last "next" character.
                item[next + 1] = '\0';}
            // Update our overall count.
            count++;}
        put_buff_3(item);
        // Check if the stop processing "STOP" occurs.
        if (stop_processing(item) == 2) 
            {break;}}
        return NULL;}





   

/*
______________________________________________________________________________________________
6)  MAIN FUNCTION
______________________________________________________________________________________________
6.1) Thread Creation       
6.2) Thread Completion         
6.3) Exit Progam   
*/


int main()
    // The majority of code in main() is from the example 
    // code provided in the assignment. 
    {pthread_t input_t, line_separator_t, plus_sign_t, output_t;


/* 6.1) Thread Creation.
___________________________________________________________________
*/
    // As shown in the example code, create the threads.
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&line_separator_t, NULL, line_separator, NULL);
    pthread_create(&plus_sign_t, NULL, plus_sign, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);


/* 6.2) Thread Completion.
___________________________________________________________________
*/
    // As shown in the example code, wait for the threads 
    // to terminate.
    pthread_join(input_t, NULL);
    pthread_join(line_separator_t, NULL);
    pthread_join(plus_sign_t, NULL);
    pthread_join(output_t, NULL);


/* 6.3) Exit Progam.
___________________________________________________________________
*/ 
    return EXIT_SUCCESS;}